#include "Application.h"

// Constructor initialize the input and output streams. Also initializes the policy for throttling with number of messages and amount of dealy interval.
Application::Application(const std::string& inputStream, const std::string& outputStream, 
    const std::string& badMessageStreamName, const int& numMessages, 
    const long& milliSecondTimeWindow, const double& queueThresholdFactor, 
    const long& evictionExcutePolicy, const int& threadPoolSize,
    const int& numPublisherThreads, log4cpp::Category& logger) : 

  m_inputStreamName(inputStream),m_outputStreamName(outputStream) , m_badMessageStreamName(badMessageStreamName), 
  m_queueThresholdFactor(queueThresholdFactor), m_evictionExcutePolicy(evictionExcutePolicy), m_threadPoolSize(threadPoolSize), 
  m_numPublisherThreads(numPublisherThreads) , m_logger(logger) 
{
  std::shared_ptr<ThrottlePolicy> throttlePolicy(new SlidingWindowThrottlePolicy(numMessages, milliSecondTimeWindow));
  m_throttlePolicy = throttlePolicy;
  m_maxQueueSize = static_cast<long>(queueThresholdFactor*numMessages);
  init();
}

void Application::init()
{
  // Initialize the streams here.
  m_logger.info("init : Initializing the application layer. setting up the streams info and processing status.");
  m_inputFileStream.open(m_inputStreamName, std::ifstream::in);	
  m_badMessageFileStream.open(m_badMessageStreamName);
  m_status = processingStatus::inprogress;
  m_logger.info("init : Finished initializing the application layer.");
}

std::string Application::getThreadId(const std::thread::id& id)
{
  stringstream ss;
  ss << id;
  string myString = ss.str();
  return(myString);
}

void Application::storeOrder(const Order&& orderRef)
{
  // First thing is to lock the deque and push in the order.
  std::lock_guard<std::mutex> guard(m_mutex);

  // Slight improvement in performance may be achived if the cancel order is pushed at front rather than back [ in sorting ].
  // This will reduce the number of positions moves required by the sorting as cancels are supposed to be ahead in queue.

  if(orderRef.getOrderType() == Order::orderCancel)
  {
    // If we have a cancel order only then attempt a sort else all the incoming orders are naturally queued according to their seqId / arrival time.
    m_InternalQueue.emplace_front(orderRef);
    std::sort(m_InternalQueue.begin(), m_InternalQueue.end(), less<Order>());
  }
  else
    m_InternalQueue.emplace_back(orderRef);
}

void Application::pushBadOrder(const std::string& pushBadOrderMessage)
{
  m_badOrders.push_back(pushBadOrderMessage);
}

void Application::recieve()
{
  std::string textLine;
  std::string badOrderMessage;
  OrderValidator validator;

  while(!m_inputFileStream.eof())
  {
    getline(m_inputFileStream, textLine);
    if(textLine.length()>0)
    {
      std::tuple<std::pair<bool,std::string>,Order> message = std::move(validator.validateOrder(textLine));
      std::pair<bool,std::string> messageStatus = std::get<0>(message);

      if(messageStatus.first)
      {
        Order sendInOrder = std::get<1>(message);
        storeOrder(std::move(sendInOrder));
        m_waitForCondition.notify_all();
      }
      else
      {
        badOrderMessage = messageStatus.second;
        badOrderMessage += " | " ;
        badOrderMessage += textLine;
        pushBadOrder(badOrderMessage);
      }
    }
  }

  m_status = processingStatus::finished;
}

void Application::send()
{
  boost::posix_time::ptime now;
  Order publishOrder;
  std::string workerId = getThreadId(std::this_thread::get_id());

  while(m_status != processingStatus::finished || !m_InternalQueue.empty())
  {
    // set the condition variable to verify that queue size is greater than zero.
    std::unique_lock<std::mutex> lk(m_mutex);
    m_waitForCondition.wait(lk, [this]{ return(!this->m_InternalQueue.empty()); });

    if(!this->m_InternalQueue.empty())
    {
      // First thing is to lock the deque and push in the order.
      now = posix_time::microsec_clock::universal_time();
      publishOrder = m_InternalQueue[0]; // Pick the front of the queue.
      m_InternalQueue.pop_front();
      m_throttlePolicy->storeTimeStamp(now);

      // Now get the amount of time we have to wait in order to publish the message from policy.
      long waitTime = m_throttlePolicy->getWaitTimeMilliSeconds();
      lk.unlock();

      std::string logMessage = "Worker id : " + workerId;
      logMessage += " : Waiting for " + std::to_string(waitTime);
      logMessage += " MilliSeconds as per policy";
      m_logger.info(logMessage);

      std::this_thread::sleep_for(std::chrono::milliseconds(waitTime));

      // Now we are ready to publish.
      logMessage = "Worker id : " + workerId; 
      logMessage += " : Publishing the Order = " + publishOrder.getOrderMessage();
      m_logger.info(logMessage);
    }
  }

  m_logger.info("Finished from the worker thread." + workerId);
}

void Application::writeBadOrders()
{
  // This method writes out all bad messages and their reason of validation fail at the end of all processing.
  // In real world it can run in its own thread and keep logging in paralle. Here for demo this is kept at the end to demonstrate the concept/principle.
  std::string logMessage;
  logMessage += "writeBadOrders : There are a total of ";
  logMessage += std::to_string(m_badOrders.size());
  logMessage += " orders which are invalid. dumping them now the seperate file";
  m_logger.info(logMessage); 
  for(const auto& iter : m_badOrders)
  {
    m_badMessageFileStream << iter << std::endl;	
  }
  m_logger.info("writeBadOrders : Finished dumping all the bad orders to the file.Exiting the writeBadOrders thread.");
}

void Application::evict()
{
  std::string logMessage;
  while(m_status != processingStatus::finished)
  {
    if(m_InternalQueue.size()>m_maxQueueSize)
    {
      long messageLossCount = m_InternalQueue.size()-m_maxQueueSize;
      m_logger.info("EvictionPolicy : Attempting to take a lock of the queue");
      std::lock_guard<std::mutex> guard(m_mutex);
      m_logger.info("EvictionPolicy : Took lock of the queue successfully.");
      // Reject all messages from deque which are having index greater than m_maxQueueSize-1.	
      m_InternalQueue.resize(m_maxQueueSize);
      logMessage += "EvicationPolicy : Warning : Total number of ";
      logMessage += std::to_string(messageLossCount);
      logMessage += " were lost due to queue overgrowth.";
      m_logger.warn(logMessage);
    }

    // Sleep for number of seconds specified by the config.
    logMessage = "EvictionPolicy : Starting to now sleep for ";
    logMessage += std::to_string(m_evictionExcutePolicy);
    logMessage += " seconds before attempting again.";
    m_logger.info(logMessage);
    std::this_thread::sleep_for(std::chrono::seconds(m_evictionExcutePolicy));
    m_logger.info("EvictionPolicy : Finished sleep quota.");
  }
  m_logger.info("EvictionPolicy : Finishing the eviction policy thread as input queue has indicated status as finished.");
}

void Application::closeStreams()
{
  m_logger.info("closeStreams : Attempting to close the file streams for output and bad messages.");
  m_inputFileStream.close();
  m_badMessageFileStream.close();
  m_logger.info("closeStreams : Finished closing the file streams.");
}

void Application::run()
{
  ThreadPool pool(m_threadPoolSize);
  std::vector<std::future<void>> results;

  std::string logMessage;
  m_logger.info("Run : Entering the application run area.");

  auto recieveThreadFunc = std::bind(&Application::recieve, this);
  auto evictThreadFunc = std::bind(&Application::evict, this);

  bindCalls threadCalls = { recieveThreadFunc, evictThreadFunc };

  logMessage = "Run : Atempting to create";
  logMessage += std::to_string(m_numPublisherThreads);
  logMessage += " Number of threads for processing the data.";
  m_logger.info(logMessage);

  for(int i=0; i<m_numPublisherThreads; ++i)
  {
    auto sendThreadFunc = std::bind(&Application::send, this);
    threadCalls.emplace_back(sendThreadFunc);
  }

  m_logger.info("Run : Submitting all the threads to the thread pool.");

  // Submit the application calls to different threads in a thread pool.
  for(auto& iter : threadCalls)
    results.emplace_back(pool.enqueue(iter));

  m_logger.info("Run : Awaiting for the finishing of all the threads from the pool.");

  // Wait for the threads to finish.
  // Now wait for the results.
  for(auto&& iter : results)
    iter.get();

  m_logger.info("Run : Finished waiting for all the threads to join from the pool.");

  // Towards the end dump all bad orders encountered to a file.
  this->writeBadOrders();
  this->closeStreams();
}

Application::~Application()
{

}
