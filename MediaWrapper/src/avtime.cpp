#include "avtime.h"

extern "C"
{
  #include <libavutil/time.h>
}

namespace MediaWrapper::AV {

    // Get current time in us
    int64_t gettime()
    {
        return av_gettime();
    }

    // Sleep given amount of us
    int usleep(unsigned usec)
    {
        return av_usleep(usec);
    }

} 
