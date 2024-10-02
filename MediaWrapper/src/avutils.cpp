#ifdef _WIN32
#  include <windows.h>
#endif

#include <signal.h>

#include "av.h"
#include "avutils.h"
#include "packet.h"
#include "frame.h"
using namespace std;

//
// Functions
//
namespace MediaWrapper::AV {

void set_logging_level(int32_t level)
{
    if (level < AV_LOG_PANIC)
        av_log_set_level(AV_LOG_QUIET);
    else if (level < AV_LOG_FATAL)
        av_log_set_level(AV_LOG_PANIC);
    else if (level < AV_LOG_ERROR)
        av_log_set_level(AV_LOG_FATAL);
    else if (level < AV_LOG_WARNING)
        av_log_set_level(AV_LOG_ERROR);
    else if (level < AV_LOG_INFO)
        av_log_set_level(AV_LOG_WARNING);
    else if (level < AV_LOG_VERBOSE)
        av_log_set_level(AV_LOG_INFO);
    else if (level < AV_LOG_DEBUG)
        av_log_set_level(AV_LOG_VERBOSE);
    else
        av_log_set_level(AV_LOG_DEBUG);
}


void set_logging_level(const string &level)
{
    if (level == "quiet")
        av_log_set_level(AV_LOG_QUIET);
    else if (level == "panic")
        av_log_set_level(AV_LOG_PANIC);
    else if (level == "fatal")
        av_log_set_level(AV_LOG_FATAL);
    else if (level == "error")
        av_log_set_level(AV_LOG_ERROR);
    else if (level == "warning")
        av_log_set_level(AV_LOG_WARNING);
    else if (level == "info")
        av_log_set_level(AV_LOG_INFO);
    else if (level == "verbose")
        av_log_set_level(AV_LOG_VERBOSE);
    else if (level == "debug")
        av_log_set_level(AV_LOG_DEBUG);
    else if (level == "trace")
        av_log_set_level(AV_LOG_TRACE);
    else    
    {
        try
        {
            set_logging_level(lexical_cast<int32_t>(level));
        }
        catch (...)
        {}
    }
}


void dumpBinaryBuffer(uint8_t *buffer, int buffer_size, int width)
{
    bool needNewLine = false;
    for (int i = 0; i < buffer_size; ++i)
    {
        needNewLine = true;
        printf("%.2X", buffer[i]);
        if ((i + 1) % width == 0 && i != 0)
        {
            printf("\n");
            needNewLine = false;
        }
        else
        {
            printf(" ");
        }
    }

    if (needNewLine)
    {
        printf("\n");
    }
}

#if LIBAVCODEC_VERSION_MAJOR < 58 // FFmpeg 4.0
#if !defined(__MINGW32__) || defined(_GLIBCXX_HAS_GTHREADS)
static int avcpp_lockmgr_cb(void **ctx, enum AVLockOp op)
{
    if (!ctx)
        return 1;

    std::mutex *mutex = static_cast<std::mutex*>(*ctx);

    int ret = 0;
    switch (op)
    {
        case AV_LOCK_CREATE:
            mutex = new std::mutex();
            *ctx  = mutex;
            ret   = !mutex;
            break;
        case AV_LOCK_OBTAIN:
            if (mutex)
                mutex->lock();
            break;
        case AV_LOCK_RELEASE:
            if (mutex)
                mutex->unlock();
            break;
        case AV_LOCK_DESTROY:
            delete mutex;
            *ctx = 0;
            break;
    }

    return ret;
}
#elif _WIN32
// MinGW with Win32 thread model
static int avcpp_lockmgr_cb(void **ctx, enum AVLockOp op)
{
    if (!ctx)
        return 1;

    CRITICAL_SECTION *sec = static_cast<CRITICAL_SECTION*>(*ctx);

    int ret = 0;
    switch (op)
    {
        case AV_LOCK_CREATE:
            sec = new CRITICAL_SECTION;
            InitializeCriticalSection(sec);
            *ctx = sec;
            break;
        case AV_LOCK_OBTAIN:
            if (sec)
                EnterCriticalSection(sec);
            break;
        case AV_LOCK_RELEASE:
            if (sec)
                LeaveCriticalSection(sec);
            break;
        case AV_LOCK_DESTROY:
            if (ctx) {
                DeleteCriticalSection(sec);
                delete sec;
            }
            *ctx = nullptr;
            break;
    }

    return ret;
}
#else
#  error "Unknown Threading model"
#endif
#endif

// void my_log_callback(void *ptr, int level, const char *fmt, va_list vargs)
// {
//     vprintf(fmt, vargs);
// }

void init()
{
#if LIBAVFORMAT_VERSION_MAJOR < 58 // FFmpeg 4.0
    av_register_all();
#endif
    avformat_network_init();
#if LIBAVCODEC_VERSION_MAJOR < 58 // FFmpeg 4.0
    avcodec_register_all();
#endif
#if LIBAVFILTER_VERSION_MAJOR < 7 // FFmpeg 4.0
    avfilter_register_all();
#endif
    avdevice_register_all();

#if LIBAVCODEC_VERSION_MAJOR < 58 // FFmpeg 4.0
    av_lockmgr_register(avcpp_lockmgr_cb);
#endif

    // switch(spdlog::get_level())
    // {
    //     case SPDLOG_LEVEL_TRACE:
    //         set_logging_level(AV_LOG_TRACE);
    //         break;
    //     case SPDLOG_LEVEL_DEBUG:
    //         spdlog::trace("Set logging level to debug");   
    //         set_logging_level(AV_LOG_DEBUG);
    //         break;
    //     case SPDLOG_LEVEL_INFO:
    //         spdlog::trace("Set logging level to info");
    //         set_logging_level(AV_LOG_INFO);
    //         break;
    //     case SPDLOG_LEVEL_WARN:
    //         spdlog::trace("Set logging level to warning");
    //         set_logging_level(AV_LOG_WARNING);
    //         break;
    //     case SPDLOG_LEVEL_ERROR:
    //         spdlog::trace("Set logging level to error");
    //         set_logging_level(AV_LOG_ERROR);
    //         break;
    //     case SPDLOG_LEVEL_CRITICAL:
    //         spdlog::trace("Set logging level to fatal");
    //         set_logging_level(AV_LOG_FATAL);
    //         break;
    // }

    // Ignore sigpipe by default
#ifdef __unix__
    signal(SIGPIPE, SIG_IGN);
#endif
    // Set callback for logging

    av_log_set_callback(
        [](void *ptr, int level, const char *fmt, va_list vargs)
        {
            std::mutex mutex;
            std::unique_lock<std::mutex> lock(mutex);
            char line[1024];
            vsnprintf(line, sizeof(line), fmt, vargs);
            auto line_s = std::string(line);
            line_s.erase(std::remove(line_s.begin(), line_s.end(), '\n'), line_s.end());

            if (level > av_log_get_level()){
                return;
            }

            // switch (level)
            // {
            //     case AV_LOG_PANIC:
            //     case AV_LOG_FATAL:
            //     case AV_LOG_ERROR:
            //         spdlog::error("[FFmpeg] {}", line_s.c_str());
            //         break;
            //     case AV_LOG_WARNING:
            //         spdlog::warn("[FFmpeg] {}", line_s.c_str());
            //         break;
            //     case AV_LOG_INFO:
            //         spdlog::info("[FFmpeg] {}", line_s.c_str());
            //         break;
            //     case AV_LOG_VERBOSE:
            //     case AV_LOG_DEBUG:
            //         spdlog::debug("[FFmpeg] {}", line_s.c_str());
            //         break;
            //     case AV_LOG_TRACE:
            //         spdlog::trace("[FFmpeg] {}", line_s.c_str());
            //         break;
            // }
        }
    );
}

string error2string(int error)
{
    char errorBuf[AV_ERROR_MAX_STRING_SIZE] = {0};
    av_strerror(error, errorBuf, AV_ERROR_MAX_STRING_SIZE);
    return string(errorBuf);
}

namespace v1 {
bool AvDeleter::operator()(SwsContext *&swsContext)
{
    sws_freeContext(swsContext);
    swsContext = nullptr;
    return true;
}

bool AvDeleter::operator()(AVCodecContext *&codecContext)
{
#if LIBAVCODEC_VERSION_MAJOR >= 58
    avcodec_free_context(&codecContext);
#else
    avcodec_close(codecContext);
    av_free(codecContext);
    codecContext = nullptr;
#endif
    return true;
}

bool AvDeleter::operator()(AVOutputFormat *&format)
{
    // Only set format to zero, it can'be freed by user
    format = 0;
    return true;
}

bool AvDeleter::operator()(AVFormatContext *&formatContext)
{
    avformat_free_context(formatContext);
    formatContext = nullptr;
    return true;
}

bool AvDeleter::operator()(AVFrame *&frame)
{
    av_frame_free(&frame);
    return true;
}

bool AvDeleter::operator()(AVPacket *&packet)
{
    av_packet_free(&packet);
    return true;
}

bool AvDeleter::operator()(AVDictionary *&dictionary)
{
    av_dict_free(&dictionary);
    dictionary = nullptr;
    return true;
}

bool AvDeleter::operator ()(AVFilterInOut *&filterInOut)
{
    avfilter_inout_free(&filterInOut);
    return true;
}
}
} // ::av


