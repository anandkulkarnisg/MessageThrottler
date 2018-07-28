#include<iostream>
#include<string>

#include <log4cpp/Category.hh>
#include <log4cpp/FileAppender.hh>
#include <log4cpp/BasicLayout.hh>
#include <log4cpp/PatternLayout.hh>

#include <boost/core/noncopyable.hpp>

using namespace std;
using namespace boost;

#ifndef Logger_H
#define Logger_H

class Logger : private boost::noncopyable
{
	public:
		static log4cpp::Category& createLogger(const std::string&);
		static void shutdown();
};

#endif

