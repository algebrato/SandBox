#ifndef _AV_TIME_H
#define _AV_TIME_H

#include "ffmpeg.h"

namespace MediaWrapper::AV {

    /**
     * Get the current time in microseconds.
     */
    int64_t gettime();


    /**
     * Sleep for a period of time.  Although the duration is expressed in
     * microseconds, the actual delay may be rounded to the precision of the
     * system timer.
     *
     * @param  usec Number of microseconds to sleep.
     * @return zero on success or (negative) error code.
     */
    int usleep(unsigned usec);


} // namespace av

#endif // _AV_TIME_H