#include<vector>
#include<deque>
#include<mutex>
#include<fstream>
#include<chrono>
#include<thread>
#include<functional>
#include<condition_variable>

#include "Order.h"
#include "OrderValidator.h"
#include "SlidingWindowThrottlePolicy.h"

#include "ThreadPool.h"

using namespace std;
using namespace boost;

#ifndef Application_H
#define Application_H

// This is the application layer. This layer has two parts. Each part runs in a seperate thread. The first part is responsible for bringing messages from a file [ In real application this may be from
// network layer / socket listner etc ]. Then store it in a deque structure. The layer also pushes in the timestamp of message arrival into ThrottlePolicy object. 
// The orders are sorted every time there is insertion into deque since the cancel orders must make it ahead of the queue for delivery.

// The second layer is running in another thread and is responsible for reading from deque and checking against the throttle policy.it waits for required delay time before sending the order.
// The order sending simulation is done by writing it to a file here. In real world this will involve writing it over network to the destination.

enum class processingStatus { inprogress, finished };

class Application
{

private:
	std::deque<Order> m_InternalQueue;					// This stores the incoming messages and sorts them in required order.
	std::mutex m_mutex;									// Used to enable synchronization.
	std::shared_ptr<ThrottlePolicy> m_throttlePolicy;	// The throttle policy is initialized here.
	std::ifstream m_inputFileStream;					// The file from where the Orders are read.
	std::ofstream m_outPutFileStream;					// The file to which dispatched orders are written.	
	std::vector<std::string> m_badOrders;				// dump all invalid orders into this vector and later perhaps print it out in a log ?
	std::string m_inputStreamName;
	std::string m_outputStreamName;
	std::string m_badMessageStreamName;
	std::ofstream m_badMessageFileStream;
	std::condition_variable m_waitForCondition;
	processingStatus m_status;
	double m_queueThresholdFactor;				
	long m_evictionExcutePolicy;	
	long m_maxQueueSize;
	void init();

public:
	Application(const std::string& , const std::string&, const std::string&, const int&, const long&, const double&, const long&);
	Application(const Application&) = delete;
	Application& operator=(const Application&) = delete;
	Application(Application&& ) = delete;
	Application& operator=(Application&& ) = delete;

	void storeOrder(const Order&&);					// Stores the incoming order into deque and marks its timestamp into the throttle policy.
	void send();									// Get order from input source and store it in the queue.
	void recieve();									// Get order from front of deque and publish it out.
	void pushBadOrder(const std::string&);			// store invalid or bad orders here.
	void writeBadOrders();
	void evict();									// This function evicts the messages from back of queue if the size grows more than specified threshold.
	void closeStreams();							// Close the file streams required during processing.
	void run();										// run layer runs all the logic of the application.

	~Application();

};

typedef std::vector<std::_Bind<std::_Mem_fn<void (Application::*)()> (Application*)>> bindCalls;

#endif
