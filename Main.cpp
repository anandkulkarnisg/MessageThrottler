#include<iostream>
#include<string>
#include<fstream>

#include "Order.h"
#include "OrderValidator.h"
#include "SlidingWindowThrottlePolicy.h"
#include "Application.h"

#include "ThreadPool.h"

using namespace std;

int main(int argc, char* argv[])
{
	// Create Application Layer. The parameters are input stream from where orders are read and processed.
	// The second parameter is the destination to which orders are written after waiting for specified period decided by policy of sliding window.
	// Third and fourth parameter are the number of messages allowed and window Time in milli seconds.

	Application	app("input.txt.full", "output.txt", "badmessages.txt", 100, 1000, 25, 5);

	// Now we try to run the application in three threads. One thread picks up readAndPublish and another runs the recieveAndProcess method.
	// We try to solve this via building three std functions and send them to a generic threadPool executor. 
	// Third threads runs eviction policy at specified periods of gap. In above case every 5 secs and evicts anything greater than size of 25 * 100 = 2500 messages in queue.

	ThreadPool pool(4);
	std::vector<std::future<void>> results;

	auto recieveThreadFunc = std::bind(&Application::recieve, &app); 
	auto sendThreadFunc = std::bind(&Application::send, &app);
	auto evictThreadFunc = std::bind(&Application::evict,&app);

	results.emplace_back(pool.enqueue(recieveThreadFunc));
	results.emplace_back(pool.enqueue(sendThreadFunc));
	results.emplace_back(pool.enqueue(evictThreadFunc));

	// Wait for the threads to finish.
	// Now wait for the results.
	for(auto&& iter : results)
		iter.get();

	// Towards the end dump all bad orders encountered to a file.
	app.writeBadOrders();

	// Exit the process.
	return(0);
}


