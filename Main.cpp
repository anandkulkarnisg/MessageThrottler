#include<iostream>
#include<string>

#include "Application.h"
#include "CppXMLConfigReader.h"

using namespace std;

log4cpp::Category& createLogger(const std::string& outputStreamName)
{
	//Setting up Appender, layout and Category.
	log4cpp::Appender *appender = new log4cpp::FileAppender("FileAppender", outputStreamName.c_str());
	log4cpp::Layout *myLayout = new log4cpp::PatternLayout();
	((log4cpp::PatternLayout*) myLayout)->setConversionPattern("%d %-5p [[%c]] - %m%n");
	log4cpp::Category& category = log4cpp::Category::getInstance("ThrottlePolicy");

	appender->setLayout(myLayout);
	category.setAppender(appender);
	category.setPriority(log4cpp::Priority::INFO);
	return(category);
}

int main(int argc, char* argv[])
{

	// Synopsis  below.
	// Application layer call parameter are described below.
	// First item  : input file  from which orders are played.
	// Second item : output file to which orders are written [ as if published ].
	// Third item  : The file to which invalid/bad orders are dumped at end of processing.
	// Fourth item : Max number of Messages per internal allowed [ M ].
	// Fifth item  : Time window or internal in which M messages are allowed in milli seconds.
	// Six parameter : This is factor which multiplied to M will determine max elements allowed in the queue.
	// Seventh parameter : This is the factor which will determine period in secs the eviction policy is run.
	// Last parameter : This is the size of thread pool the Application needs.

	// The below will invoke eviction policy frequently and will demo flushing of the queue often.
	// Application   app("input.txt.full", "output.txt", "badmessages.txt", 100, 1000, 25, 20);      

	// The below will almost does not invoke the eviction practically.

	// Load the configuration from the xml config file.
	CppXMLConfigReader configReader("config.xml", "ThrottleAppSettings", "root");
	configReader.init();

	std::string inputFeedFile = configReader.getStringValue("inputFeedFile", "input.txt.full");
	std::string outputPublishFile = configReader.getStringValue("outputPublishFile", "output.txt");
	std::string badMessagesFile = configReader.getStringValue("badMessagesFile", "badmessages.txt");

	int numMessages = configReader.getIntValue("numMessages", 100);
	long delayInMilliSeconds = configReader.getLongValue("delayInMilliSeconds", 1000);
	double queueFactor = configReader.getDoubleValue("queueFactor", 25);
	long evictionDelayInSecs = configReader.getLongValue("evictionDelayInSecs", 60);
	int threadPoolSize = configReader.getIntValue("threadPoolSize", 4);		
	int publishThreads= configReader.getIntValue("numPublishers", 4);

	log4cpp::Category& logger = createLogger(outputPublishFile); 
	Application app(inputFeedFile, outputPublishFile, badMessagesFile, numMessages, delayInMilliSeconds, queueFactor, evictionDelayInSecs, threadPoolSize, publishThreads, logger);

	// Now we try to run the application in three threads. One thread picks up readAndPublish and another runs the recieveAndProcess method.
	// We try to solve this via building three std functions and send them to a generic threadPool executor. 
	// Third threads runs eviction policy at specified periods of gap. In above case every 60 secs and evicts anything greater than size of 25 * 100 = 2500 messages in queue.

	app.run();

	// Shutdown the application logging.
	log4cpp::Category::shutdown();

	// Exit the process.
	return(0);
}


