#include<iostream>
#include<string>
#include <boost/circular_buffer.hpp>
#include <utility>
#include <boost/date_time/posix_time/posix_time.hpp>

using namespace std;
using namespace boost;

#ifndef ThrottlePolicy_H
#define ThrottlePolicy_H

class ThrottlePolicy
{
	protected:
		boost::circular_buffer<boost::posix_time::ptime> cb;
		int m_numMessages;
		long m_milliSecondWindow;

	public:
		void init(const int& numMessages, const long& milliSecondWindow, const boost::circular_buffer<boost::posix_time::ptime>& cbRef)
		{
			this->cb = cbRef;
			this->m_numMessages = numMessages;
			this->m_milliSecondWindow = milliSecondWindow;
		}
		void storeTimeStamp(const boost::posix_time::ptime& timeTick)
		{
			this->cb.push_back(timeTick);		
		}
		int getNumMessages() { return(m_numMessages); }
		long getMilliSecondWindow() { return(m_milliSecondWindow); }
		bool isBufferFull() { return(cb.capacity() == cb.size()); }
		int getAvergaeMessageTime() { return(m_milliSecondWindow/m_numMessages); }
		virtual long getWaitTimeMilliSeconds() = 0;		// This is specific to each strategy of Throttling hence it is left for derived class to define it.
};

#endif
