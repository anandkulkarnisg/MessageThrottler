#include "ThrottlePolicy.h"

using namespace std;
using namespace boost;

#ifndef SlidingWindowThrottlePolicy_H
#define SlidingWindowThrottlePolicy_H

class SlidingWindowThrottlePolicy : public ThrottlePolicy
{
	public:
		SlidingWindowThrottlePolicy(const int& numMessages, const long& milliSecondWindow)
		{
			boost::circular_buffer<boost::posix_time::ptime> buffer(numMessages);
			init(numMessages, milliSecondWindow, buffer);			
		}

		long getWaitTimeMilliSeconds()
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
};

#endif
