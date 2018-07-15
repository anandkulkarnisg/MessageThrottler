#include<iostream>
#include<string>
#include<fstream>
#include<functional>

#include "Order.h"
#include "OrderValidator.h"
#include "SlidingWindowThrottlePolicy.h"
#include "Application.h"

#include "ThreadPool.h"

using namespace std;

typedef std::vector<std::_Bind<std::_Mem_fn<void (Application::*)()> (Application*)>> bindCalls;

int main(int argc, char* argv[])
{
	// Create Application Layer. The parameters are input stream from where orders are read and processed.
	// The second parameter is the destination to which orders are written after waiting for specified period decided by policy of sliding window.
	// Third and fourth parameter are the number of messages allowed and window Time in milli seconds.
	// Fifth and sixth parameters are factor which will determine size of queue beyond which eviction will happen [ Ex:- In below case 25 * 100 = 2500 ]
	// and how often [ number of seconds ] during which eviction thread keeps running.

	// The below will invoke eviction policy frequently and will demo flushing of the queue often.
	//Application   app("input.txt.full", "output.txt", "badmessages.txt", 100, 1000, 25, 20);      

	// The below will almost does not invoke the eviction practically.
	Application app("input.txt.full", "output.txt", "badmessages.txt", 100, 1000, 25, 60);

	// Now we try to run the application in three threads. One thread picks up readAndPublish and another runs the recieveAndProcess method.
	// We try to solve this via building three std functions and send them to a generic threadPool executor. 
	// Third threads runs eviction policy at specified periods of gap. In above case every 5 secs and evicts anything greater than size of 25 * 100 = 2500 messages in queue.

	ThreadPool pool(4);
	std::vector<std::future<void>> results;

	auto recieveThreadFunc = std::bind(&Application::recieve, &app); 
	auto sendThreadFunc = std::bind(&Application::send, &app);
	auto evictThreadFunc = std::bind(&Application::evict,&app);

	bindCalls threadCalls;
	threadCalls.emplace_back(recieveThreadFunc);
	threadCalls.emplace_back(sendThreadFunc);
	threadCalls.emplace_back(evictThreadFunc);

	for(auto& iter : threadCalls)
		results.emplace_back(pool.enqueue(iter));

	// Wait for the threads to finish.
	// Now wait for the results.
	for(auto&& iter : results)
		iter.get();

	// Towards the end dump all bad orders encountered to a file.
	app.writeBadOrders();
	app.closeStreams();

	// Exit the process.
	return(0);
}


