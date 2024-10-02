#ifndef FRAME_H
#define FRAME_H

#include <vector>
#include <memory>
#include <stdexcept>

#include "ffmpeg.h"
#include "rational.h"
#include "timestamp.h"
#include "pixelformat.h"
#include "sampleformat.h"
#include "avutils.h"

extern "C" {
#include <libavutil/imgutils.h>
#include <libavutil/attributes.h>
}

namespace MediaWrapper::AV
{
    namespace frame {
        namespace priv {
            void channel_layout_copy(AVFrame &dst, const AVFrame &src);
        }

        static inline int64_t get_best_effort_timestamp(const AVFrame* frame) {
            return frame->best_effort_timestamp;
        }

        static inline uint64_t get_channel_layout(const AVFrame* frame) {
            return frame->ch_layout.order == AV_CHANNEL_ORDER_NATIVE ? frame->ch_layout.u.mask : 0;
        }

        static inline void set_channel_layout(AVFrame* frame, uint64_t layout) {
            av_channel_layout_uninit(&frame->ch_layout);
            av_channel_layout_from_mask(&frame->ch_layout, layout);
        }


        static inline int get_channels(const AVFrame* frame) {
            return frame->ch_layout.nb_channels;
        }

        static inline bool is_valid_channel_layout(const AVFrame *frame)
        {
            return av_channel_layout_check(&frame->ch_layout);
        }

        static inline int get_sample_rate(const AVFrame* frame) 
        {
            return frame->sample_rate;
        }

        static inline void set_sample_rate(AVFrame* frame, int sampleRate) 
        {
            frame->sample_rate = sampleRate;
        }


    } // ::frame

    template<typename T>
    class Frame : public FFWrapperPtr<AVFrame>
    {
    protected:
        T& assignOperator(const T &rhs) {
            if (this == &rhs)
                return static_cast<T&>(*this);
            T(rhs).swap(static_cast<T&>(*this));
            return static_cast<T&>(*this);
        }

        T& moveOperator(T &&rhs) {
            if (this == &rhs)
                return static_cast<T&>(*this);
            T(std::move(rhs)).swap(static_cast<T&>(*this));
            return static_cast<T&>(*this);
        }

    public:
        Frame() {
            m_raw = av_frame_alloc();
            m_raw->opaque = this;
        }

        ~Frame() {
            av_frame_free(&m_raw);
        }

        Frame(const AVFrame *frame) {
            if (frame) {
                m_raw = av_frame_alloc();
                m_raw->opaque = this;
                av_frame_ref(m_raw, frame);
            }
        }

        // Helper ctors to implement move/copy ctors
        Frame(const T& other) : Frame(other.m_raw) {
            if (m_raw)
                copyInfoFrom(other);
        }

        Frame(T&& other) : FFWrapperPtr<AVFrame>(nullptr) {
            if (other.m_raw) {
                m_raw = av_frame_alloc();
                m_raw->opaque = this;
                av_frame_move_ref(m_raw, other.m_raw);
                copyInfoFrom(other);
            }
        }

        static T null() {
            return T(nullptr);
        }

        // You must implement operators in deveritive classes using assignOperator() and moveOperator()
        void operator=(const Frame&) = delete;

        void swap(Frame &other) {
            using std::swap;
    #define FRAME_SWAP(x) swap(x, other.x)
            FRAME_SWAP(m_raw);
            FRAME_SWAP(m_timeBase);
            FRAME_SWAP(m_streamIndex);
            FRAME_SWAP(m_isComplete);
    #undef FRAME_SWAP
        }

        void copyInfoFrom(const T& other) {
            m_timeBase    = other.m_timeBase;
            m_streamIndex = other.m_streamIndex;
            m_isComplete  = other.m_isComplete;
        }

        bool isReferenced() const {
            return m_raw && m_raw->buf[0];
        }

        int refCount() const {
            if (m_raw && m_raw->buf[0])
                return av_buffer_get_ref_count(m_raw->buf[0]);
            else
                return 0;
        }

        AVFrame* makeRef() const {
            return m_raw ? av_frame_clone(m_raw) : nullptr;
        }

        T clone(int align = 1) const {
            T result;

            // Setup data required for buffer allocation
            result.m_raw->format         = m_raw->format;
            result.m_raw->width          = m_raw->width;
            result.m_raw->height         = m_raw->height;
            result.m_raw->nb_samples     = m_raw->nb_samples;

            frame::priv::channel_layout_copy(*result.m_raw, *m_raw);

            result.copyInfoFrom(static_cast<const T&>(*this));

            av_frame_get_buffer(result.m_raw, align);
            av_frame_copy(result.m_raw, m_raw);
            av_frame_copy_props(result.m_raw, m_raw);
            return result;
        }

        Timestamp pts() const
        {
            return {RAW_GET(pts, MediaWrapper::AV::NoPts), m_timeBase};
        }

        attribute_deprecated void setPts(int64_t pts, Rational ptsTimeBase)
        {
            RAW_SET(pts, ptsTimeBase.rescale(pts, m_timeBase));
        }

        void setPts(const Timestamp &ts)
        {
            if (m_timeBase == Rational())
                m_timeBase = ts.timebase();
            RAW_SET(pts, ts.timestamp(m_timeBase));
        }

        const Rational& timeBase() const { return m_timeBase; }

        void setTimeBase(const Rational &value) {
            if (m_timeBase == value)
                return;

            if (!m_raw)
                return;

            int64_t rescaledPts          = NoPts;
            int64_t rescaledBestEffortTs = NoPts;

            if (m_timeBase != Rational() && value != Rational()) {
                if (m_raw->pts != MediaWrapper::AV::NoPts)
                    rescaledPts = m_timeBase.rescale(m_raw->pts, value);

                if (m_raw->best_effort_timestamp != MediaWrapper::AV::NoPts)
                    rescaledBestEffortTs = m_timeBase.rescale(m_raw->best_effort_timestamp, value);
            } else {
                rescaledPts          = m_raw->pts;
                rescaledBestEffortTs = m_raw->best_effort_timestamp;
            }

            if (m_timeBase != Rational()) {
                m_raw->pts                   = rescaledPts;
                m_raw->best_effort_timestamp = rescaledBestEffortTs;
            }

            m_timeBase = value;
        }

        int streamIndex() const {
            return m_streamIndex;
        }

        void setStreamIndex(int streamIndex) {
            m_streamIndex = streamIndex;
        }

        void setComplete(bool isComplete) {
            m_isComplete = isComplete;
        }

        bool isComplete() const { return m_isComplete; }

        bool isValid() const { return (!isNull() && m_raw->data[0] && m_raw->linesize[0]); }

        operator bool() const { return isValid() && isComplete(); }

        uint8_t *data(size_t plane = 0) {
            if (!m_raw || plane >= size_t(AV_NUM_DATA_POINTERS + m_raw->nb_extended_buf))
                return nullptr;
            return m_raw->extended_data[plane];
        }

        const uint8_t *data(size_t plane = 0) const {
            if (!m_raw || plane >= size_t(AV_NUM_DATA_POINTERS + m_raw->nb_extended_buf))
                return nullptr;
            return m_raw->extended_data[plane];;
        }

        size_t size(size_t plane) const {
            if (!m_raw || plane >= size_t(AV_NUM_DATA_POINTERS + m_raw->nb_extended_buf))
                return 0;
            AVBufferRef *buf = plane < AV_NUM_DATA_POINTERS ?
                                m_raw->buf[plane] :
                                m_raw->extended_buf[plane - AV_NUM_DATA_POINTERS];
            if (buf == nullptr)
                return 0;
            return size_t(buf->size);
        }

        size_t size() const {
            if (!m_raw)
                return 0;
            size_t total = 0;
            if (m_raw->buf[0]) {
                for (auto i = 0; i < AV_NUM_DATA_POINTERS && m_raw->buf[i]; i++) {
                    total += m_raw->buf[i]->size;
                }

                for (auto i = 0; i < m_raw->nb_extended_buf; ++i) {
                    total += m_raw->extended_buf[i]->size;
                }
            } else if (m_raw->data[0]) {
                if (m_raw->width && m_raw->height) {
                    uint8_t data[4] = {0};
                    int     linesizes[4] = {
                        m_raw->linesize[0],
                        m_raw->linesize[1],
                        m_raw->linesize[2],
                        m_raw->linesize[3],
                    };
                    total = av_image_fill_pointers(reinterpret_cast<uint8_t**>(&data),
                                                static_cast<AVPixelFormat>(m_raw->format),
                                                m_raw->height,
                                                nullptr,
                                                linesizes);
                } else if (m_raw->nb_samples && frame::is_valid_channel_layout(m_raw)) {
                    for (auto i = 0; i < m_raw->nb_extended_buf + AV_NUM_DATA_POINTERS && m_raw->extended_data[i]; ++i) {
                        // According docs, all planes must have same size
                        total += m_raw->linesize[0];
                    }
                }
            }
            return total;
        }

        void dump() const {
            if (!m_raw)
                return;
            if (m_raw->buf[0]) {
                for (size_t i = 0; i < AV_NUM_DATA_POINTERS && m_raw->buf[i]; i++) {
                    av_hex_dump(stdout, m_raw->buf[i]->data, m_raw->buf[i]->size);
                }
            } else if (m_raw->data[0]) {
                av_hex_dump(stdout, m_raw->data[0], size());
            }
        }

    protected:
        Rational             m_timeBase{};
        int                  m_streamIndex {-1};
        bool                 m_isComplete  {false};
    };


    class VideoFrame : public Frame<VideoFrame>
    {
    public:
        using Frame<VideoFrame>::Frame;

        VideoFrame() = default;
        VideoFrame(PixelFormat pixelFormat, int width, int height, int align = 1);
        VideoFrame(const uint8_t *data, size_t size, PixelFormat pixelFormat, int width, int height, int align = 1);

        VideoFrame(const VideoFrame &other);
        VideoFrame(VideoFrame &&other);

        VideoFrame& operator=(const VideoFrame &rhs);
        VideoFrame& operator=(VideoFrame &&rhs);

        PixelFormat            pixelFormat() const;
        int                    width() const;
        int                    height() const;

        bool                   isKeyFrame() const;
        void                   setKeyFrame(bool isKey);

        int                    quality() const;
        void                   setQuality(int quality);

        AVPictureType          pictureType() const;
        void                   setPictureType(AVPictureType type = AV_PICTURE_TYPE_NONE);

        Rational               sampleAspectRatio() const;
        void                   setSampleAspectRatio(const Rational& sampleAspectRatio);

        size_t                 bufferSize(int align = 1, OptionalErrorCode ec = throws()) const;
        bool                   copyToBuffer(uint8_t *dst, size_t size, int align = 1, OptionalErrorCode ec = throws());
        bool                   copyToBuffer(std::vector<uint8_t>& dst, int align = 1, OptionalErrorCode ec = throws());
    };


    class AudioSamples : public Frame<AudioSamples>
    {
    public:
        using Frame<AudioSamples>::Frame;

        AudioSamples() = default;
        AudioSamples(SampleFormat sampleFormat, int samplesCount, uint64_t channelLayout, int sampleRate, int align = SampleFormat::AlignDefault);
        AudioSamples(const uint8_t *data, size_t size,
                    SampleFormat sampleFormat, int samplesCount, uint64_t channelLayout, int sampleRate, int align = SampleFormat::AlignDefault);

        AudioSamples(const AudioSamples &other);
        AudioSamples(AudioSamples &&other);

        AudioSamples& operator=(const AudioSamples &rhs);
        AudioSamples& operator=(AudioSamples &&rhs);

        int            init(SampleFormat sampleFormat, int samplesCount, uint64_t channelLayout, int sampleRate, int align = SampleFormat::AlignDefault);

        SampleFormat sampleFormat() const;
        int            samplesCount() const;
        int            channelsCount() const;
        uint64_t channelsLayout() const;
        int            sampleRate() const;
        size_t         sampleBitDepth(OptionalErrorCode ec = throws()) const;
        bool           isPlanar() const;

        std::string    channelsLayoutString() const;
    };
} // namespace av

#endif // FRAME_H