#ifndef AV_H
#define AV_H

//! @tableofcontents
//! @brief The MediaWrapper namespace contains a set of high-level classes
//! written in C++17 that wrap the ffmpeg library.
namespace MediaWrapper::AV {

    /**
     * Init all subsustems needed by MediaWrapper. 
     * Must be call before any other functionality
     */
    void init();

} // ::av

#endif // AV_H
