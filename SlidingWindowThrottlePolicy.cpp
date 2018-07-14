#include "SlidingWindowThrottlePolicy.h"

SlidingWindowThrottlePolicy::SlidingWindowThrottlePolicy() : m_numMessages(100), m_milliSecondWindow(1000)
{
    // Set up the circular buffer here. The size is numMessages [ N ] because the difference of time between recent most message is 
    // t[99] - t[0] gives the amount of time spent in sending the m_numMessages.

    boost::circular_buffer<boost::posix_time::ptime> buffer(m_numMessages);
    cb = buffer;
}

// Constructor implementation.
SlidingWindowThrottlePolicy::SlidingWindowThrottlePolicy(const int& numMessages, const long& TimeIntervalInMilliSeconds) : m_numMessages(numMessages), m_milliSecondWindow(TimeIntervalInMilliSeconds) 
{ 
	// Set up the circular buffer here. The size is numMessages [ N ] because the difference of time between recent most message is 
	// t[99] - t[0] gives the amount of time spent in sending the m_numMessages.

	boost::circular_buffer<boost::posix_time::ptime> buffer(m_numMessages);
	cb = buffer;
}

// Copy constructor Implementation.
SlidingWindowThrottlePolicy::SlidingWindowThrottlePolicy(const SlidingWindowThrottlePolicy& copyRef)
{
	this->cb = copyRef.cb;	
	this->m_isBufferFull = copyRef.m_isBufferFull;
	this->m_numMessages = copyRef.m_numMessages;
	this->m_milliSecondWindow = copyRef.m_milliSecondWindow;
}

// Assignment operator.
SlidingWindowThrottlePolicy& SlidingWindowThrottlePolicy::operator=(const SlidingWindowThrottlePolicy& assignRef)
{
    this->cb = assignRef.cb;
    this->m_isBufferFull = assignRef.m_isBufferFull;
    this->m_numMessages = assignRef.m_numMessages;
    this->m_milliSecondWindow = assignRef.m_milliSecondWindow;
	return(*this);
}

// Method to add timestamps to the circular buffer.
void SlidingWindowThrottlePolicy::storeTimeStamp(const boost::posix_time::ptime& inputTimeStamp)
{
	cb.push_back(inputTimeStamp);
}

// Implement the virrtual methods now to implement the interface.
int SlidingWindowThrottlePolicy::getNumMessages() const 
{
	return(m_numMessages);
}

long SlidingWindowThrottlePolicy::getMilliSecondWindow() const
{
	return(m_milliSecondWindow);
}

bool SlidingWindowThrottlePolicy::isBufferFull()
{
	return(cb.size() == cb.capacity());
}

long SlidingWindowThrottlePolicy::getAvergaeMessageTime() const
{
	return(m_milliSecondWindow/m_numMessages);
}

// Actual specific policy/behaviour implementation is done here.
long SlidingWindowThrottlePolicy::getWaitTimeMilliSeconds()
{
	// Until the circular buffer is full we publish initially at a uniform rate. ie at rate derived by getAverageMessageTime. Here it is 10 milli seconds.
	long returnResult = 0;
	if(isBufferFull())
	{
		// If the buffer is full then simply check the time difference between the last and first item of buffer.
		long timeDiff = (cb[m_numMessages-1] - cb[0]).total_milliseconds();

		if(timeDiff >= m_milliSecondWindow)
		{
			// We can send the message queued immediately. No issues. return zero delay.
			return(0);
		}
		else
		{
			returnResult = m_milliSecondWindow - timeDiff;
			return(returnResult);
		}
	}
	else
	{
		// We have to initially send the m_numMessages at a uniform rate of getAvergaeMessageTime.
		return(getAvergaeMessageTime()); 
	}
}

