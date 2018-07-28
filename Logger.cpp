#include<iostream>
#include<string>

#include "Logger.h"

log4cpp::Category& Logger::createLogger(const std::string& outputStreamName)
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

Logger::~Logger()
{
	// Shutdown the application logging.
	log4cpp::Category::shutdown();
}
