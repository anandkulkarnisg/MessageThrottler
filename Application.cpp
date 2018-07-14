#include "Application.h"

// Constructor initialize the input and output streams. Also initializes the policy for throttling with number of messages and amount of dealy interval.
Application::Application(const std::string& inputStream, const std::string& outputStream, const std::string& badMessageStreamName, 
		const int& numMessages, const long& milliSecondTimeWindow, const double& queueThresholdFactor, const long& evictionExcutePolicy) : 
	m_inputStreamName(inputStream),m_outputStreamName(outputStream) , m_badMessageStreamName(badMessageStreamName), 
	m_queueThresholdFactor(queueThresholdFactor), m_evictionExcutePolicy(evictionExcutePolicy) 
{
	SlidingWindowThrottlePolicy policy(numMessages, milliSecondTimeWindow);
	m_throttlePolicy = policy;
	m_maxQueueSize = static_cast<long>(queueThresholdFactor*numMessages);
	init();
}

void Application::init()
{
	// Initialize the streams here.
	m_inputFileStream.open(m_inputStreamName, std::ifstream::in);	
	m_outPutFileStream.open(m_outputStreamName);
	m_status = processingStatus::inprogress;
}

void Application::storeOrder(const Order& orderRef)
{
	// First thing is to lock the deque and push in the order.
	std::lock_guard<std::mutex> guard(m_mutex);
	m_InternalQueue.push_back(orderRef);
	std::sort(m_InternalQueue.begin(), m_InternalQueue.end(), less<Order>());
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
			std::tuple<std::pair<bool,std::string>,Order> message = validator.validateOrder(textLine);
			std::pair<bool,std::string> messageStatus = std::get<0>(message);
			Order sendInOrder = std::get<1>(message);
			if(messageStatus.first)
			{
				storeOrder(sendInOrder);
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
	bool publishStatus = false;	

	while(m_status != processingStatus::finished)
	{
		// set the condition variable to verify that queue size is greater than zero.
		while(m_InternalQueue.size() > 0)
		{
			std::unique_lock<std::mutex> lk(m_waitMutex);
			m_waitForCondition.wait(lk, [this]{ return (this->m_InternalQueue.size() > 0); });

			// First thing is to lock the deque and push in the order.
			{
				std::lock_guard<std::mutex> guard(m_mutex);
				now = posix_time::microsec_clock::universal_time();
				publishOrder = m_InternalQueue[0]; // Pick the front of the queue.
				m_InternalQueue.pop_front();
				m_throttlePolicy.storeTimeStamp(now);
				publishStatus = true;
			}

			// Now get the amount of time we have to wait in order to publish the message from policy.
			long waitTime = m_throttlePolicy.getWaitTimeMilliSeconds();
			m_outPutFileStream << "Waiting for " << waitTime << " MilliSeconds as per policy" << std::endl;
			std::this_thread::sleep_for(std::chrono::milliseconds(waitTime));

			// Now we are ready to publish.
			if(publishStatus)
			{
				m_outPutFileStream << "Publishing the Order = " << publishOrder.getOrderMessage() << std::endl;
			}
		}
	}
}

void Application::writeBadOrders()
{
	// This method writes out all bad messages and their reason of validation fail at the end of all processing.
	// In real world it can run in its own thread and keep logging in paralle. Here for demo this is kept at the end to demonstrate the concept/principle.
	m_badMessageFileStream.open(m_badMessageStreamName);	
	for(const auto& iter : m_badOrders)
	{
		m_badMessageFileStream << iter << std::endl;	
	}
	m_badMessageFileStream.close();
}

void Application::evict()
{
	while(m_status != processingStatus::finished)
	{
		if(m_InternalQueue.size()>m_maxQueueSize)
		{
			long messagesLossCount = m_InternalQueue.size()-m_maxQueueSize;
			std::lock_guard<std::mutex> guard(m_mutex);
			// Reject all messages from deque which are having index greater than m_maxQueueSize-1.	
			m_InternalQueue.resize(m_maxQueueSize);
			std::cout << "WARN : Total number of " << messagesLossCount << " Were lost due to queue overgrowth." << std::endl;
		}

		// Sleep for number of seconds specified by the config.
		std::this_thread::sleep_for(std::chrono::seconds(m_evictionExcutePolicy));
	}
}

Application::~Application()
{
	m_inputFileStream.close();
	m_outPutFileStream.close();
}

