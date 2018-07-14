#include <boost/circular_buffer.hpp>
#include <utility>
#include <boost/date_time/posix_time/posix_time.hpp>

#ifndef SlidingWindowThrottlePolicy_H
#define SlidingWindowThrottlePolicy_H

using namespace std;
using namespace boost;

class SlidingWindowThrottlePolicy
{
	private:
        boost::circular_buffer<boost::posix_time::ptime> cb;
        bool m_isBufferFull = false;
        int m_numMessages;
        long m_milliSecondWindow;

	public:
		// Constructors.
		SlidingWindowThrottlePolicy();
		SlidingWindowThrottlePolicy(const int&, const long&);
		SlidingWindowThrottlePolicy(const SlidingWindowThrottlePolicy&);
		SlidingWindowThrottlePolicy& operator=(const SlidingWindowThrottlePolicy&);

		// Method to add timestamp to the circular buffer.
		void storeTimeStamp(const boost::posix_time::ptime&);

		// The below public functions should be implemented.
        int getNumMessages() const;
        long getMilliSecondWindow() const;
        bool isBufferFull();
        long getAvergaeMessageTime() const ;

        // The below virtual function needs to implement the specific policy based on requirement. Here we implement the SlidingWindow Policy Logic.
		long getWaitTimeMilliSeconds();
};

#endif
