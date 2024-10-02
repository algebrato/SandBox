#ifndef CODEC_CONTEXT_H
#define CODEC_CONTEXT_H

#include "ffmpeg.h"
#include "stream.h"
#include "avutils.h"
#include "averror.h"
#include "pixelformat.h"
#include "sampleformat.h"
#include "avlog.h"
#include "frame.h"
#include "codec.h"
#include "channellayout.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/version.h>
}


namespace MediaWrapper::AV {

    namespace codec_context::audio {
        void set_channels(AVCodecContext *obj, int channels);
        void set_channel_layout_mask(AVCodecContext *obj, uint64_t mask);
        int get_channels(const AVCodecContext *obj);
        uint64_t get_channel_layout_mask(const AVCodecContext *obj);
    }

    class CodecContext : public FFWrapperPtr<AVCodecContext>, public noncopyable
    {
    protected:
        void swap(CodecContext &other);

        //
        // No directly created
        //
        using BaseWrapper = FFWrapperPtr<AVCodecContext>;
        using BaseWrapper::BaseWrapper;

        CodecContext();

        //
        // Stream decoding/encoding
        //
        CodecContext(const class Stream &st,
                    const class Codec& codec,
                    Direction direction,
                    AVMediaType type,
                    bool hw_dec);
        //
        // Stream independ decoding/encoding
        //
        CodecContext(const class Codec &codec, Direction direction, AVMediaType type);


        //
        // DTOR
        //
        ~CodecContext();

        void setCodec(const class Codec &codec, bool resetDefaults, Direction direction, AVMediaType type, OptionalErrorCode ec = throws());
        AVMediaType codecType(AVMediaType contextType) const noexcept;

    public:

        using BaseWrapper::_log;

        //
        // Common
        //
        void open(OptionalErrorCode ec = throws());
        void open(const Codec &codec, OptionalErrorCode ec = throws());
        void open(class Dictionary &options, OptionalErrorCode ec = throws());
        void open(class Dictionary &&options, OptionalErrorCode ec = throws());
        void open(class Dictionary &options, const Codec &codec, OptionalErrorCode ec = throws());
        void open(class Dictionary &&options, const Codec &codec, OptionalErrorCode ec = throws());

        [[deprecated("Start from FFmpeg 4.0 it is recommended to destroy and recreate codec context insted of close")]]
        void close(OptionalErrorCode ec = throws());
        bool isOpened() const noexcept;
        bool isValid() const noexcept;

        /**
         * Copy codec context from codec context associated with given stream or other codec context.
         * This functionality useful for remuxing without deconding/encoding. In this case you need not
         * open codecs, only copy context.
         *
         * @param other  stream or codec context
         */
        /// @{
        void copyContextFrom(const CodecContext &other, OptionalErrorCode ec = throws());
        /// @}

        Rational timeBase() const noexcept;
        void setTimeBase(const Rational &value) noexcept;

        const Stream& stream() const noexcept;
        Codec codec() const noexcept;

        void setOption(const std::string &key, const std::string &val, OptionalErrorCode ec = throws());
        void setOption(const std::string &key, const std::string &val, int flags, OptionalErrorCode ec = throws());

        int frameSize() const noexcept;
        int64_t frameNumber() const noexcept;

        // Note, set ref counted to enable for multithreaded processing
        bool isRefCountedFrames() const noexcept;
        void setRefCountedFrames(bool refcounted) const noexcept;

        int strict() const noexcept;
        void setStrict(int strict) noexcept;

        int64_t bitRate() const noexcept;
        std::pair<int64_t, int64_t> bitRateRange() const noexcept;
        void setBitRate(int64_t bitRate) noexcept;
        void setBitRateRange(const std::pair<int64_t, int64_t> &bitRateRange) noexcept;

        // Flags
        /// Access to CODEC_FLAG_* flags
        /// @{
        void setFlags(int flags) noexcept;
        void addFlags(int flags) noexcept;
        void clearFlags(int flags) noexcept;
        int flags() noexcept;
        bool isFlags(int flags) noexcept;
        /// @}

        // Flags 2
        /// Access to CODEC_FLAG2_* flags
        /// @{
        void setFlags2(int flags) noexcept;
        void addFlags2(int flags) noexcept;
        void clearFlags2(int flags) noexcept;
        int flags2() noexcept;
        bool isFlags2(int flags) noexcept;
        /// @}


    protected:

        bool isValidForEncode(Direction direction, AVMediaType type) const noexcept;
        bool checkCodec(const Codec& codec, Direction direction, AVMediaType type, OptionalErrorCode ec);
        void open(const Codec &codec, AVDictionary **options, OptionalErrorCode ec);


        std::pair<int, const std::error_category*>
        decodeCommon(AVFrame *outFrame, const class Packet &inPacket, size_t offset, int &frameFinished, bool hw_dec,
                    int (*decodeProc)(AVCodecContext*, AVFrame*,int *, const AVPacket *, bool hw_dec)) noexcept;

        std::pair<int, const std::error_category*>
        encodeCommon(class Packet &outPacket, const AVFrame *inFrame, int &gotPacket,
                            int (*encodeProc)(AVCodecContext*, AVPacket*,const AVFrame*, int*)) noexcept;

    public:
        template<typename T>
        std::pair<int, const std::error_category*>
        decodeCommon(T &outFrame,
                    const class Packet &inPacket,
                    size_t offset,
                    int &frameFinished, bool hw_dec,
                    int (*decodeProc)(AVCodecContext *, AVFrame *, int *, const AVPacket *, bool hw_dec));

        template<typename T>
        std::pair<int, const std::error_category*>
        encodeCommon(class Packet &outPacket,
                    const T &inFrame,
                    int &gotPacket,
                    int (*encodeProc)(AVCodecContext *, AVPacket *, const AVFrame *, int *));

    private:
        Stream m_stream;
    };


    /**
     * @brief The GenericCodecContext class to copy contexts from input streams to output one.
     *
     * We should omit strong direction checking in this case. Only when we cast it to the appropriate
     * encoding coder.
     *
     */
    class GenericCodecContext : public CodecContext
    {
    protected:
        using CodecContext::codecType;

    public:
        GenericCodecContext() = default;
        GenericCodecContext(Stream st);
        GenericCodecContext(GenericCodecContext&& other);

        GenericCodecContext& operator=(GenericCodecContext&& rhs);

        AVMediaType codecType() const noexcept;
    };


    template<typename Clazz, Direction _direction, AVMediaType _type>
    class CodecContextBase : public CodecContext
    {
    protected:
        Clazz& moveOperator(Clazz &&rhs)
        {
            if (this == &rhs)
                return static_cast<Clazz&>(*this);
            Clazz(std::forward<Clazz>(rhs)).swap(static_cast<Clazz&>(*this));
            return static_cast<Clazz&>(*this);
        }

        using CodecContext::setCodec;

    public:

        using CodecContext::_log;

        CodecContextBase()
            : CodecContext()
        {
        }

        // Stream decoding/encoding
        explicit CodecContextBase(const class Stream &st, const class Codec& codec = Codec(), bool hw_dec = false)
            : CodecContext(st, codec, _direction, _type, hw_dec)
        {

        }

        // Stream independ decoding/encoding
        explicit CodecContextBase(const Codec &codec)
            : CodecContext(codec, _direction, _type)
        {
            if (checkCodec(codec, _direction, _type, throws()))
            {
                m_raw = avcodec_alloc_context3(codec.raw());

            }

        }

        //
        // Disable copy/Activate move
        //
        CodecContextBase(CodecContextBase &&other)
            : CodecContextBase()
        {
            swap(other);
        }
        //


        void setCodec(const Codec &codec, OptionalErrorCode ec = throws())
        {
            setCodec(codec, false, _direction, _type, ec);
        }

        void setCodec(const Codec &codec, bool resetDefaults, OptionalErrorCode ec = throws())
        {
            setCodec(codec, resetDefaults, _direction, _type, ec);
        }

        AVMediaType codecType() const noexcept
        {
            return codecType(_type);
        }
    };


    template<typename Clazz, Direction _direction>
    class VideoCodecContext : public CodecContextBase<Clazz, _direction, AVMEDIA_TYPE_VIDEO>
    {
    public:
        using Parent = CodecContextBase<Clazz, _direction, AVMEDIA_TYPE_VIDEO>;
        using Parent::Parent;
        using Parent::isValid;
        using Parent::isOpened;

        int width() const
        {
            return RAW_GET2(isValid(), width, 0);
        }

        int height() const
        {
            return RAW_GET2(isValid(), height, 0);
        }

        int codedWidth() const
        {
            return RAW_GET2(isValid(), coded_width, 0);
        }

        int codedHeight() const
        {
            return RAW_GET2(isValid(), coded_height, 0);
        }

        PixelFormat pixelFormat() const
        {
            return RAW_GET2(isValid(), pix_fmt, AV_PIX_FMT_NONE);
        }

        int32_t globalQuality() const
        {
            return RAW_GET2(isValid(), global_quality, FF_LAMBDA_MAX);
        }

        int32_t gopSize() const
        {
            return RAW_GET2(isValid(), gop_size, 0);
        }

        int bitRateTolerance() const
        {
            return RAW_GET2(isValid(), bit_rate_tolerance, 0);
        }

        int maxBFrames() const
        {
            return RAW_GET2(isValid(), max_b_frames, 0);
        }

        Rational sampleAspectRatio() const
        {
            return RAW_GET(sample_aspect_ratio, AVRational());
        }

        void setWidth(int w) // Note, it also sets coded_width
        {
            if (isValid() & !isOpened())
            {
                m_raw->width       = w;
                m_raw->coded_width = w;
            }
        }

        void setHeight(int h) // Note, it also sets coded_height
        {
            if (isValid() && !isOpened())
            {
                m_raw->height       = h;
                m_raw->coded_height = h;
            }
        }

        void setCodedWidth(int w)
        {
            RAW_SET2(isValid() && !isOpened(), coded_width, w);
        }

        void setCodedHeight(int h)
        {
            RAW_SET2(isValid() && !isOpened(), coded_height, h);
        }

        void setPixelFormat(PixelFormat pixelFormat)
        {
            if (pixelFormat == AV_PIX_FMT_YUVJ420P){
                av_log(NULL, AV_LOG_WARNING, "AV_PIX_FMT_YUVJ420P is deprecated, using AV_PIX_FMT_YUV420P instead\n");
                pixelFormat = AV_PIX_FMT_YUV420P;
            }
            RAW_SET2(isValid(), pix_fmt, pixelFormat);
        }

        void setGlobalQuality(int32_t quality)
        {
            if (quality < 0 || quality > FF_LAMBDA_MAX)
                quality = FF_LAMBDA_MAX;

            RAW_SET2(isValid(), global_quality, quality);
        }

        void setGopSize(int32_t size)
        {
            RAW_SET2(isValid(), gop_size, size);
        }

        void setBitRateTolerance(int bitRateTolerance)
        {
            RAW_SET2(isValid(), bit_rate_tolerance, bitRateTolerance);
        }

        void setMaxBFrames(int maxBFrames)
        {
            RAW_SET2(isValid(), max_b_frames, maxBFrames);
        }

        void setSampleAspectRatio(const Rational& sampleAspectRatio)
        {
            RAW_SET(sample_aspect_ratio, sampleAspectRatio);
        }

    protected:
        using Parent::moveOperator;
        using Parent::m_raw;
    };


    class VideoDecoderContext : public VideoCodecContext<VideoDecoderContext, Direction::Decoding>
    {
    public:
        using Parent = VideoCodecContext<VideoDecoderContext, Direction::Decoding>;
        using Parent::Parent;

        VideoDecoderContext() = default;
        VideoDecoderContext(VideoDecoderContext&& other);

        VideoDecoderContext& operator=(VideoDecoderContext&& other);

        /**
         * @brief decodeVideo  - decode video packet
         *
         * @param packet   packet to decode
         * @param[in,out] ec     this represents the error status on exit, if this is pre-initialized to
         *                       av#throws the function will throw on error instead
         * @param autoAllocateFrame  it true - output will be allocated at the ffmpeg internal, otherwise
         *                           it will be allocated before decode proc call.
         * @return encoded video frame, if error: exception thrown or error code returns, in both cases
         *         output undefined.
         */
        VideoFrame decode(const Packet    &packet,
                        OptionalErrorCode ec = throws(),
                        bool             autoAllocateFrame = true,
                        bool             hw_dec = false);

        /**
         * @brief decodeVideo - decode video packet with additional parameters
         *
         * @param[in] packet         packet to decode
         * @param[in] offset         data offset in packet
         * @param[out] decodedBytes  amount of decoded bytes
         * @param[in,out] ec     this represents the error status on exit, if this is pre-initialized to
         *                       av#throws the function will throw on error instead
         * @param autoAllocateFrame  it true - output will be allocated at the ffmpeg internal, otherwise
         *                           it will be allocated before decode proc call.
         * @return encoded video frame, if error: exception thrown or error code returns, in both cases
         *         output undefined.
         */
        VideoFrame decode(const Packet &packet,
                        size_t offset,
                        size_t &decodedBytes,
                        OptionalErrorCode ec = throws(),
                        bool    autoAllocateFrame = true,
                        bool    hw_dec = false);


    private:
        VideoFrame decodeVideo(OptionalErrorCode ec,
                            const Packet &packet,
                            size_t offset,
                            size_t *decodedBytes,
                            bool    autoAllocateFrame,
                            bool hw_dec = false);

    };


    class VideoEncoderContext : public VideoCodecContext<VideoEncoderContext, Direction::Encoding>
    {
    public:
        using Parent = VideoCodecContext<VideoEncoderContext, Direction::Encoding>;
        using Parent::Parent;

        VideoEncoderContext() = default;
        VideoEncoderContext(VideoEncoderContext&& other);

        VideoEncoderContext& operator=(VideoEncoderContext&& other);

        /**
         * @brief encodeVideo - Flush encoder
         *
         * Stop flushing when returns empty packets
         *
         * @param[in,out] ec     this represents the error status on exit, if this is pre-initialized to
         *                       av#throws the function will throw on error instead
         * @return
         */
        Packet encode(OptionalErrorCode ec = throws());

        /**
         * @brief encodeVideo - encode video frame
         *
         * @note Some encoders need some amount of frames before beginning encoding, so it is normal,
         *       that for some amount of frames returns empty packets.
         *
         * @param inFrame  frame to encode
         * @param[in,out] ec     this represents the error status on exit, if this is pre-initialized to
         *                       av#throws the function will throw on error instead
         * @return
         */
        Packet encode(const VideoFrame &inFrame, OptionalErrorCode ec = throws());

    };


    template<typename Clazz, Direction _direction>
    class AudioCodecContext : public CodecContextBase<Clazz, _direction, AVMEDIA_TYPE_AUDIO>
    {
    public:
        using Parent = CodecContextBase<Clazz, _direction, AVMEDIA_TYPE_AUDIO>;
        using Parent::Parent;
        using Parent::isValid;
        using Parent::isOpened;
        using Parent::_log;

        int sampleRate() const noexcept
        {
            return RAW_GET2(isValid(), sample_rate, 0);
        }

        int channels() const noexcept
        {
            if (!isValid())
                return 0;
            return codec_context::audio::get_channels(m_raw);
        }

        SampleFormat sampleFormat() const noexcept
        {
            return RAW_GET2(isValid(), sample_fmt, AV_SAMPLE_FMT_NONE);
        }

        uint64_t channelLayout() const noexcept
        {
            if (!isValid())
                return 0;
            return codec_context::audio::get_channel_layout_mask(m_raw);
        }

        ChannelLayoutView channelLayout2() const noexcept
        {
            if (!isValid())
                return ChannelLayoutView{};
            return ChannelLayoutView{m_raw.ch_layout};
        }

        void setSampleRate(int sampleRate) noexcept
        {
            if (!isValid())
                return;
            int sr = guessValue(sampleRate, m_raw->codec->supported_samplerates, EqualComparator<int>(0));
            if (sr != sampleRate)
            {
                fflog(AV_LOG_INFO, "Guess sample rate %d instead unsupported %d\n", sr, sampleRate);
            }
            if (sr > 0)
                m_raw->sample_rate = sr;
        }

        void setChannels(int channels) noexcept
        {
            if (!isValid() || channels <= 0)
                return;
            codec_context::audio::set_channels(m_raw, channels);
        }

        void setSampleFormat(SampleFormat sampleFormat) noexcept
        {
            RAW_SET2(isValid(), sample_fmt, sampleFormat);
        }

        void setChannelLayout(uint64_t layout) noexcept
        {
            if (!isValid() || layout == 0)
                return;
            codec_context::audio::set_channel_layout_mask(m_raw, layout);
        }

        void setChannelLayout(ChannelLayout layout) noexcept
        {
            if (!isValid() || !layout.isValid())
                return;
            m_raw.ch_layout = *layout.raw();
            layout.release(); // is controlled by the CodecContext
        }

    protected:
        using Parent::moveOperator;
        using Parent::m_raw;
    };


    class AudioDecoderContext : public AudioCodecContext<AudioDecoderContext, Direction::Decoding>
    {
    public:
        using Parent = AudioCodecContext<AudioDecoderContext, Direction::Decoding>;
        using Parent::Parent;

        AudioDecoderContext() = default;
        AudioDecoderContext(AudioDecoderContext&& other);

        AudioDecoderContext& operator=(AudioDecoderContext&& other);

        AudioSamples decode(const Packet &inPacket, OptionalErrorCode ec = throws(), bool hw_dec = false);
        AudioSamples decode(const Packet &inPacket, size_t offset, OptionalErrorCode ec = throws(), bool hw_dec = false);

    };


    class AudioEncoderContext : public AudioCodecContext<AudioEncoderContext, Direction::Encoding>
    {
    public:
        using Parent = AudioCodecContext<AudioEncoderContext, Direction::Encoding>;
        using Parent::Parent;

        AudioEncoderContext() = default;
        AudioEncoderContext(AudioEncoderContext&& other);

        AudioEncoderContext& operator=(AudioEncoderContext&& other);

        Packet encode(OptionalErrorCode ec = throws());
        Packet encode(const AudioSamples &inSamples, OptionalErrorCode ec = throws());

    };
    
} // namespace av

#endif // CODEC_CONTEXT_H
