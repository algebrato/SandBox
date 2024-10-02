#include <stdexcept>

#include "avlog.h"
#include "avutils.h"
#include "averror.h"

#include "stream.h"
#include "frame.h"
#include "packet.h"
#include "dictionary.h"
#include "codec.h"

#include "codeccontext.h"

using namespace std;

// Global variable used by the callback function
static enum AVPixelFormat hw_pix_fmt;

// Callback function to get the correct pixel hardware format
static enum AVPixelFormat get_hw_format(AVCodecContext* ctx, const enum AVPixelFormat* pix_fmts)
{
	const enum AVPixelFormat* p;
	for (p = pix_fmts; *p != -1; p++) {
		if (*p == hw_pix_fmt)
			return *p;
	}
	return AV_PIX_FMT_NONE;
}

namespace MediaWrapper::AV {
namespace {

std::pair<int, const error_category*>
make_error_pair(Errors errc)
{
    return make_pair(static_cast<int>(errc), &avcpp_category());
}

std::pair<int, const error_category*>
make_error_pair(int status)
{
    if (status < 0)
        return make_pair(status, &ffmpeg_category());
    return make_pair(status, nullptr);
}

}

GenericCodecContext::GenericCodecContext(Stream st)
    : CodecContext(st, Codec(), st.direction(), st.mediaType(), false)
{
}

GenericCodecContext::GenericCodecContext(GenericCodecContext &&other)
    : GenericCodecContext()
{
    swap(other);
}

GenericCodecContext &GenericCodecContext::operator=(GenericCodecContext &&rhs)
{
    if (this == &rhs)
        return *this;
    GenericCodecContext(std::move(rhs)).swap(*this);
    return *this;
}

AVMediaType GenericCodecContext::codecType() const noexcept
{
    return codecType(stream().mediaType());
}

// ::anonymous
} // ::av

namespace {

    // Use avcodec_send_packet() and avcodec_receive_frame()
    int decode(AVCodecContext *avctx,
            AVFrame *picture,
            int *got_picture_ptr,
            const AVPacket *avpkt,
            bool hw_dec)
    {
        if (got_picture_ptr)
            *got_picture_ptr = 0;

        int ret;
        if (avpkt) {
            ret = avcodec_send_packet(avctx, avpkt);
            if (ret < 0 && ret != AVERROR_EOF)
                return ret;
        }
        
        if(hw_dec)
        {
            auto tmpFrame = av_frame_alloc();
            ret = avcodec_receive_frame(avctx, tmpFrame);
            if (ret < 0 && ret != AVERROR(EAGAIN))
                return ret;
            if (ret >= 0 && got_picture_ptr)
                *got_picture_ptr = 1;
            if(ret < 0 ) {
                av_frame_free(&tmpFrame);
                *got_picture_ptr = 0;
                return ret;
            }

            int result = av_hwframe_transfer_data(picture, tmpFrame, 0);
            if (result < 0 && ret != AVERROR(EAGAIN))
                return ret;
            if (result >= 0 && got_picture_ptr)
                *got_picture_ptr = 1;
                
            av_frame_copy_props(picture, tmpFrame);
            av_frame_free(&tmpFrame);

        }  else {
            ret = avcodec_receive_frame(avctx, picture);
            if (ret < 0 && ret != AVERROR(EAGAIN))
                return ret;
            if (ret >= 0 && got_picture_ptr)
                *got_picture_ptr = 1;
        }

        return 0;
    }

    // Use avcodec_send_frame() and avcodec_receive_packet()
    int encode(AVCodecContext *avctx,
            AVPacket *avpkt,
            const AVFrame *frame,
            int *got_packet_ptr)
    {
        if (got_packet_ptr)
            *got_packet_ptr = 0;

        int ret;
        ret = avcodec_send_frame(avctx, frame);
        if (ret < 0 && ret != AVERROR(EAGAIN) && ret != AVERROR_EOF)
            return ret;

        ret = avcodec_receive_packet(avctx, avpkt);
        if (ret < 0 && ret != AVERROR(EAGAIN) && ret != AVERROR_EOF)
            return ret;
        if (got_packet_ptr)
            *got_packet_ptr = 1;
        return 0;
    }

    int avcodec_decode_video_legacy(AVCodecContext *avctx, AVFrame *picture,
                                    int *got_picture_ptr,
                                    const AVPacket *avpkt, 
                                    bool hw_dec)
    {
        return decode(avctx, picture, got_picture_ptr, avpkt, hw_dec);
    }

    int avcodec_encode_video_legacy(AVCodecContext *avctx, AVPacket *avpkt,
                                    const AVFrame *frame, int *got_packet_ptr)
    {
        return encode(avctx, avpkt, frame, got_packet_ptr);
    }

    int avcodec_decode_audio_legacy(AVCodecContext *avctx, AVFrame *frame,
                                    int *got_frame_ptr, const AVPacket *avpkt, bool hw_dec)
    {
        return decode(avctx, frame, got_frame_ptr, avpkt, hw_dec);
    }

    int avcodec_encode_audio_legacy(AVCodecContext *avctx, AVPacket *avpkt,
                            const AVFrame *frame, int *got_packet_ptr)
    {
        return encode(avctx, avpkt, frame, got_packet_ptr);
    }

} 

namespace MediaWrapper::AV {


VideoDecoderContext::VideoDecoderContext(VideoDecoderContext &&other)
    : Parent(std::move(other))
{
}

VideoDecoderContext &VideoDecoderContext::operator=(VideoDecoderContext&& other)
{
    return moveOperator(std::move(other));
}

VideoFrame VideoDecoderContext::decode(const Packet &packet, OptionalErrorCode ec, bool autoAllocateFrame, bool hw_dec)
{
    return decodeVideo(ec, packet, 0, nullptr, autoAllocateFrame, hw_dec);
}

VideoFrame VideoDecoderContext::decode(const Packet &packet, size_t offset, size_t &decodedBytes, OptionalErrorCode ec, bool autoAllocateFrame, bool hw_dec)
{
    return decodeVideo(ec, packet, offset, &decodedBytes, autoAllocateFrame, hw_dec);
}


VideoFrame VideoDecoderContext::decodeVideo(OptionalErrorCode ec, const Packet &packet, size_t offset, size_t *decodedBytes, bool autoAllocateFrame, bool hw_dec)
{
    clear_if(ec);

    VideoFrame outFrame;
    if (!autoAllocateFrame)
    {
        outFrame = {pixelFormat(), width(), height(), 32};

        if (!outFrame.isValid())
        {
            throws_if(ec, Errors::FrameInvalid);
            return VideoFrame();
        }
    }

    int gotFrame = 0;
    auto st = decodeCommon(outFrame.raw(), packet, offset, gotFrame, hw_dec, avcodec_decode_video_legacy);

    if (get<1>(st)) {
        throws_if(ec, get<0>(st), *get<1>(st));
        return VideoFrame();
    }

    if (!gotFrame)
        return VideoFrame();

    outFrame.setPictureType(AV_PICTURE_TYPE_I);

    if (decodedBytes)
        *decodedBytes = get<0>(st);

    return outFrame;
}

VideoEncoderContext::VideoEncoderContext(VideoEncoderContext &&other)
    : Parent(std::move(other))
{
}

VideoEncoderContext &VideoEncoderContext::operator=(VideoEncoderContext&& other)
{
    return moveOperator(std::move(other));
}

Packet VideoEncoderContext::encode(OptionalErrorCode ec)
{
    return encode(VideoFrame(nullptr), ec);
}

Packet VideoEncoderContext::encode(const VideoFrame &inFrame, OptionalErrorCode ec)
{
    clear_if(ec);

    int gotPacket = 0;
    Packet packet;
    auto st = encodeCommon(packet, inFrame, gotPacket, avcodec_encode_video_legacy);

    if (get<1>(st)) {
        throws_if(ec, get<0>(st), *get<1>(st));
        return Packet();
    }

    if (!gotPacket) {
        packet.setComplete(false);
        return packet;
    }

    packet.setComplete(true);
    return packet;
}

void CodecContext::swap(CodecContext &other)
{
    using std::swap;
    swap(m_stream, other.m_stream);
    swap(m_raw, other.m_raw);
}

CodecContext::CodecContext()
{
    m_raw = avcodec_alloc_context3(nullptr);
}

CodecContext::CodecContext(const Stream &st, const Codec &codec, Direction direction, AVMediaType type, bool hw_dec)
    : m_stream(st)
{
    if (st.direction() != direction)
        throw MediaWrapper::AV::Exception(make_avcpp_error(Errors::CodecInvalidDirection));

    if (st.mediaType() != type)
        throw MediaWrapper::AV::Exception(make_avcpp_error(Errors::CodecInvalidMediaType));


    auto const codecId = st.raw()->codecpar->codec_id;

    Codec c = codec;
    if (codec.isNull())
    {
        if (st.direction() == Direction::Decoding)
            c = findDecodingCodec(codecId);
        else
            c = findEncodingCodec(codecId);
    }

    m_raw = avcodec_alloc_context3(c.raw());
    enum AVHWDeviceType hw_ctx;
    if (hw_dec)
    {
        for (int i = 0;; i++)
        {
            const AVCodecHWConfig* config = avcodec_get_hw_config(c.raw(), i); // get the i-th HW config
            if (!config) 
            {
                return;
            }
            if (config->methods & AV_CODEC_HW_CONFIG_METHOD_HW_DEVICE_CTX && config->device_type == AV_HWDEVICE_TYPE_DXVA2 )
            {
                // If the config->decive_type mathces the compatible type, set the hw_pix_format 
                // (future perspective: can be exploit to perform the render with pix_shader on device, in order to avoid
                // a massive data exchange between CPU and GPU)
                hw_ctx = AV_HWDEVICE_TYPE_DXVA2;
                hw_pix_fmt = config->pix_fmt;
                break;
            }
            if (config->methods & AV_CODEC_HW_CONFIG_METHOD_HW_DEVICE_CTX && config->device_type == AV_HWDEVICE_TYPE_CUDA )
            {
                // If the config->decive_type mathces the compatible type, set the hw_pix_format 
                // (future perspective: can be exploit to perform the render with pix_shader on device, in order to avoid
                // a massive data exchange between CPU and GPU)
                hw_ctx = AV_HWDEVICE_TYPE_CUDA;
                hw_pix_fmt = config->pix_fmt;
                break;
            }

        }

        m_raw->get_format = get_hw_format;
        AVBufferRef* hwDeviceContext = nullptr;
        int result = av_hwdevice_ctx_create(&hwDeviceContext, hw_ctx, "auto_any", NULL, 0);

        m_raw->hw_device_ctx = av_buffer_ref(hwDeviceContext);
        av_buffer_unref(&hwDeviceContext);

        m_raw->flags |= AV_CODEC_FLAG_TRUNCATED;
        m_raw->strict_std_compliance = FF_COMPLIANCE_EXPERIMENTAL;

    }



    if (m_raw) {
        avcodec_parameters_to_context(m_raw, st.raw()->codecpar);
    }
}

CodecContext::CodecContext(const Codec &codec, Direction direction, AVMediaType type)
{
    if (checkCodec(codec, direction, type, throws()))
        m_raw = avcodec_alloc_context3(codec.raw());
}

CodecContext::~CodecContext()
{
    //
    // Do not track stream-oriented codec:
    //  - Stream always owned by FormatContext
    //  - Stream always can be obtained from the FormatContext
    //  - CodecContext can be obtained at any time from the Stream
    //  - If FormatContext closed, all streams destroyed: all opened codec context closed too.
    // So only stream-independ CodecContext's must be tracked and closed in ctor.
    // Note: new FFmpeg API declares, that AVStream not owned by codec and deals only with codecpar
    //
    avcodec_free_context(&m_raw);
}

void CodecContext::setCodec(const Codec &codec, bool resetDefaults, Direction direction, AVMediaType type, OptionalErrorCode ec)
{
    clear_if(ec);

    if (!m_raw)
    {
        fflog(AV_LOG_WARNING, "Codec context does not allocated\n");
        throws_if(ec, Errors::Unallocated);
        return;
    }

    if (!m_raw || (!m_stream.isValid() && !m_stream.isNull()))
    {
        fflog(AV_LOG_WARNING, "Parent stream is not valid. Probably it or FormatContext destroyed\n");
        throws_if(ec, Errors::CodecStreamInvalid);
        return;
    }

    if (codec.isNull())
    {
        fflog(AV_LOG_WARNING, "Try to set null codec\n");
    }

    if (!checkCodec(codec, direction, type, ec))
        return;

    if (resetDefaults) {
        avcodec_free_context(&m_raw);
        m_raw = avcodec_alloc_context3(codec.raw());
    } else {
        m_raw->codec_id   = !codec.isNull() ? codec.raw()->id : AV_CODEC_ID_NONE;
        m_raw->codec_type = type;
        m_raw->codec      = codec.raw();

        if (!codec.isNull()) {
            if (codec.raw()->pix_fmts != 0)
                m_raw->pix_fmt = *(codec.raw()->pix_fmts); // assign default value
            if (codec.raw()->sample_fmts != 0)
                m_raw->sample_fmt = *(codec.raw()->sample_fmts);
        }
    }

    // TBD: need a check
    if (m_stream.isValid())
        avcodec_parameters_from_context(m_stream.raw()->codecpar, m_raw);
}

AVMediaType CodecContext::codecType(AVMediaType contextType) const noexcept
{
    if (isValid())
    {
        if (m_raw->codec && (m_raw->codec_type != m_raw->codec->type || m_raw->codec_type != contextType))
            fflog(AV_LOG_ERROR, "Non-consistent AVCodecContext::codec_type and AVCodec::type and/or context type\n");

        return m_raw->codec_type;
    }
    return contextType;
}

void CodecContext::open(OptionalErrorCode ec)
{
    open(Codec(), ec);
}

void CodecContext::open(const Codec &codec, OptionalErrorCode ec)
{
    open(codec, nullptr, ec);
}

void CodecContext::open(Dictionary &&options, OptionalErrorCode ec)
{
    open(std::move(options), Codec(), ec);
}

void CodecContext::open(Dictionary &options, OptionalErrorCode ec)
{
    open(options, Codec(), ec);
}

void CodecContext::open(Dictionary &&options, const Codec &codec, OptionalErrorCode ec)
{
    open(options, codec, ec);
}

void CodecContext::open(Dictionary &options, const Codec &codec, OptionalErrorCode ec)
{
    auto prt = options.release();
    open(codec, &prt, ec);
    options.assign(prt);
}

void CodecContext::close(OptionalErrorCode ec)
{
    clear_if(ec);
    if (isOpened())
    {
        avcodec_close(m_raw);
        return;
    }
    throws_if(ec, Errors::CodecNotOpened);
}

bool CodecContext::isOpened() const noexcept
{
    return isValid() ? avcodec_is_open(m_raw) : false;
}

bool CodecContext::isValid() const noexcept
{
    // Check parent stream first
    return ((m_stream.isValid() || m_stream.isNull()) && m_raw && m_raw->codec);
}

void CodecContext::copyContextFrom(const CodecContext &other, OptionalErrorCode ec)
{
    clear_if(ec);
    if (!isValid()) {
        fflog(AV_LOG_ERROR, "Invalid target context\n");
        throws_if(ec, Errors::CodecInvalid);
        return;
    }
    if (!other.isValid()) {
        fflog(AV_LOG_ERROR, "Invalid source context\n");
        throws_if(ec, Errors::CodecInvalid);
        return;
    }
    if (isOpened()) {
        fflog(AV_LOG_ERROR, "Try to copy context to opened target context\n");
        throws_if(ec, Errors::CodecAlreadyOpened);
        return;
    }
    // TODO: need to be checked
    if (m_raw->codec_type != AVMEDIA_TYPE_UNKNOWN &&
        m_raw->codec_type != other.m_raw->codec_type)
    {
        fflog(AV_LOG_ERROR, "Context media types not same");
        throws_if(ec, Errors::CodecInvalidMediaType);
        return;
    }
    if (this == &other) {
        fflog(AV_LOG_WARNING, "Same context\n");
        // No error here, simple do nothig
        return;
    }

    AVCodecParameters params{};
    avcodec_parameters_from_context(&params, other.m_raw);
    avcodec_parameters_to_context(m_raw, &params);
    m_raw->codec_tag = 0;
}

Rational CodecContext::timeBase() const noexcept
{
    return RAW_GET2(isValid(), time_base, AVRational());
}

void CodecContext::setTimeBase(const Rational &value) noexcept
{
    RAW_SET2(isValid() && !isOpened(), time_base, value.getValue());
}

const Stream &CodecContext::stream() const noexcept
{
    return m_stream;
}

Codec CodecContext::codec() const noexcept
{
    if (isValid())
        return Codec(m_raw->codec);
    else
        return Codec();
}

void CodecContext::setOption(const string &key, const string &val, OptionalErrorCode ec)
{
    setOption(key, val, 0, ec);
}

void CodecContext::setOption(const string &key, const string &val, int flags, OptionalErrorCode ec)
{
    clear_if(ec);
    if (isValid())
    {
        auto sts = av_opt_set(m_raw->priv_data, key.c_str(), val.c_str(), flags);
        if (sts) {
            throws_if(ec, sts, ffmpeg_category());
        }
    }
    else
    {
        throws_if(ec, Errors::CodecInvalid);
    }
}

int CodecContext::frameSize() const noexcept
{
    return RAW_GET2(isValid(), frame_size, 0);
}

int64_t CodecContext::frameNumber() const noexcept
{
    return RAW_GET2(isValid(), frame_number, 0);
}

bool CodecContext::isRefCountedFrames() const noexcept
{
    if (!isValid())
        return false;
    int64_t val;
    av_opt_get_int(m_raw, "refcounted_frames", 0, &val);
    return !!val;
}

void CodecContext::setRefCountedFrames(bool refcounted) const noexcept
{
    if (isValid() && !isOpened()) {
        av_opt_set_int(m_raw, "refcounted_frames", refcounted, 0);
    }
}

int CodecContext::strict() const noexcept
{
    return RAW_GET2(isValid(), strict_std_compliance, 0);
}

void CodecContext::setStrict(int strict) noexcept
{
    if (strict < FF_COMPLIANCE_EXPERIMENTAL)
        strict = FF_COMPLIANCE_EXPERIMENTAL;
    else if (strict > FF_COMPLIANCE_VERY_STRICT)
        strict = FF_COMPLIANCE_VERY_STRICT;

    RAW_SET2(isValid(), strict_std_compliance, strict);
}

int64_t CodecContext::bitRate() const noexcept
{
    return RAW_GET2(isValid(), bit_rate, int64_t(0));
}

std::pair<int64_t, int64_t> CodecContext::bitRateRange() const noexcept
{
    if (isValid())
        return std::make_pair(m_raw->rc_min_rate, m_raw->rc_max_rate);
    else
        return std::make_pair(0, 0);
}

void CodecContext::setBitRate(int64_t bitRate) noexcept
{
    RAW_SET2(isValid(), bit_rate, bitRate);
}

void CodecContext::setBitRateRange(const std::pair<int64_t, int64_t> &bitRateRange) noexcept
{
    if (isValid())
    {
        m_raw->rc_min_rate = std::get<0>(bitRateRange);
        m_raw->rc_max_rate = std::get<1>(bitRateRange);
    }
}

void CodecContext::setFlags(int flags) noexcept
{
    RAW_SET2(isValid(), flags, flags);
}

void CodecContext::addFlags(int flags) noexcept
{
    if (isValid())
        m_raw->flags |= flags;
}

void CodecContext::clearFlags(int flags) noexcept
{
    if (isValid())
        m_raw->flags &= ~flags;
}

int CodecContext::flags() noexcept
{
    return RAW_GET2(isValid(), flags, 0);
}

bool CodecContext::isFlags(int flags) noexcept
{
    if (isValid())
        return (m_raw->flags & flags);
    return false;
}

void CodecContext::setFlags2(int flags) noexcept
{
    RAW_SET2(isValid(), flags2, flags);
}

void CodecContext::addFlags2(int flags) noexcept
{
    if (isValid())
        m_raw->flags2 |= flags;
}

void CodecContext::clearFlags2(int flags) noexcept
{
    if (isValid())
        m_raw->flags2 &= ~flags;
}

int CodecContext::flags2() noexcept
{
    return RAW_GET2(isValid(), flags2, 0);
}

bool CodecContext::isFlags2(int flags) noexcept
{
    if (isValid())
        return (m_raw->flags2 & flags);
    return false;
}

bool CodecContext::isValidForEncode(Direction direction, AVMediaType /*type*/) const noexcept
{
    if (!isValid())
    {
        fflog(AV_LOG_WARNING,
              "Not valid context: codec_context=%p, stream_valid=%d, stream_isnull=%d, codec=%p\n",
              m_raw,
              m_stream.isValid(),
              m_stream.isNull(),
              codec().raw());
        return false;
    }

    if (!isOpened())
    {
        fflog(AV_LOG_WARNING, "You must open coder before encoding\n");
        return false;
    }

    if (direction == Direction::Decoding)
    {
        fflog(AV_LOG_WARNING, "Decoding coder does not valid for encoding\n");
        return false;
    }

    if (!codec().canEncode())
    {
        fflog(AV_LOG_WARNING, "Codec can't be used for Encode\n");
        return false;
    }

    return true;
}

bool CodecContext::checkCodec(const Codec &codec, Direction direction, AVMediaType type, OptionalErrorCode ec)
{
    if (direction == Direction::Encoding && !codec.canEncode())
    {
        fflog(AV_LOG_WARNING, "Encoding context, but codec does not support encoding\n");
        throws_if(ec, Errors::CodecInvalidDirection);
        return false;
    }

    if (direction == Direction::Decoding && !codec.canDecode())
    {
        fflog(AV_LOG_WARNING, "Decoding context, but codec does not support decoding\n");
        throws_if(ec, Errors::CodecInvalidDirection);
        return false;
    }

    if (type != codec.type())
    {
        fflog(AV_LOG_ERROR, "Media type mismatch\n");
        throws_if(ec, Errors::CodecInvalidMediaType);
        return false;
    }

    return true;
}

void CodecContext::open(const Codec &codec, AVDictionary **options, OptionalErrorCode ec)
{
    clear_if(ec);

    if (isOpened() || !isValid()) {
        throws_if(ec, isOpened() ? Errors::CodecAlreadyOpened : Errors::CodecInvalid);
        return;
    }

    int stat = avcodec_open2(m_raw, codec.raw(), options);
    if (stat < 0)
        throws_if(ec, stat, ffmpeg_category());
}

std::pair<int, const error_category *> CodecContext::decodeCommon(AVFrame *outFrame, const Packet &inPacket, size_t offset, int &frameFinished, bool hw_dec, int (*decodeProc)(AVCodecContext *, AVFrame *, int *, const AVPacket *, bool hw_dec)) noexcept
{
    if (!isValid())
        return make_error_pair(Errors::CodecInvalid);

    if (!isOpened())
        return make_error_pair(Errors::CodecNotOpened);

    if (!decodeProc)
        return make_error_pair(Errors::CodecInvalidDecodeProc);

    if (offset && inPacket.size() && offset >= inPacket.size())
        return make_error_pair(Errors::CodecDecodingOffsetToLarge);

    frameFinished = 0;

    AVPacket pkt = *inPacket.raw();
    pkt.data += offset;
    pkt.size -= offset;

    int decoded = decodeProc(m_raw, outFrame, &frameFinished, &pkt, hw_dec);
    return make_error_pair(decoded);
}

std::pair<int, const error_category *> CodecContext::encodeCommon(Packet &outPacket, const AVFrame *inFrame, int &gotPacket, int (*encodeProc)(AVCodecContext *, AVPacket *, const AVFrame *, int *)) noexcept
{
    if (!isValid()) {
        fflog(AV_LOG_ERROR, "Invalid context\n");
        return make_error_pair(Errors::CodecInvalid);
    }

    //        if (!isValidForEncode()) {
    //            fflog(AV_LOG_ERROR, "Context can't be used for encoding\n");
    //            return make_error_pair(Errors::CodecInvalidForEncode);
    //        }

    if (!encodeProc) {
        fflog(AV_LOG_ERROR, "Encoding proc is null\n");
        return make_error_pair(Errors::CodecInvalidEncodeProc);
    }

    int stat = encodeProc(m_raw, outPacket.raw(), inFrame, &gotPacket);
    if (stat) {
        fflog(AV_LOG_ERROR, "Encode error: %d, %s\n", stat, error2string(stat).c_str());
    }
    return make_error_pair(stat);
}

AudioDecoderContext::AudioDecoderContext(AudioDecoderContext &&other)
    : Parent(std::move(other))
{
}

AudioDecoderContext &AudioDecoderContext::operator=(AudioDecoderContext &&other)
{
    return moveOperator(std::move(other));
}

AudioSamples AudioDecoderContext::decode(const Packet &inPacket, OptionalErrorCode ec, bool hw_dec)
{
    return decode(inPacket, 0u, ec);
}

AudioSamples AudioDecoderContext::decode(const Packet &inPacket, size_t offset, OptionalErrorCode ec, bool hw_dec)
{
    clear_if(ec);

    AudioSamples outSamples;

    int gotFrame = 0;
    auto st = decodeCommon(outSamples.raw(), inPacket, offset, gotFrame, hw_dec, avcodec_decode_audio_legacy);
    if (get<1>(st))
    {
        throws_if(ec, get<0>(st), *get<1>(st));
        return AudioSamples();
    }

    if (!gotFrame)
    {
        outSamples.setComplete(false);
    }

    return outSamples;
}

AudioEncoderContext::AudioEncoderContext(AudioEncoderContext &&other)
    : Parent(std::move(other))
{
}

AudioEncoderContext &AudioEncoderContext::operator=(AudioEncoderContext &&other)
{
    return moveOperator(std::move(other));
}

Packet AudioEncoderContext::encode(OptionalErrorCode ec)
{
    return encode(AudioSamples(nullptr), ec);
}

Packet AudioEncoderContext::encode(const AudioSamples &inSamples, OptionalErrorCode ec)
{
    clear_if(ec);

    Packet outPacket;

    int gotFrame = 0;
    auto st = encodeCommon(outPacket, inSamples, gotFrame, avcodec_encode_audio_legacy);
    if (get<1>(st))
    {
        throws_if(ec, get<0>(st), *get<1>(st));
        return Packet();
    }

    if (!gotFrame)
    {
        outPacket.setComplete(false);
        return outPacket;
    }

    return outPacket;
}

template<typename T>
std::pair<int, const std::error_category*>
CodecContext::decodeCommon(T &outFrame,
             const Packet &inPacket,
             size_t offset,
             int &frameFinished, bool hw_dec,
             int (*decodeProc)(AVCodecContext *, AVFrame *, int *, const AVPacket *, bool hw_dec))
{
    auto st = decodeCommon(outFrame, inPacket, offset, frameFinished, hw_dec, decodeProc);
    if (std::get<1>(st))
        return st;

    if (!frameFinished)
        return std::make_pair(0u, nullptr);

    // Dial with PTS/DTS in packet/stream timebase

    if (inPacket.timeBase() != Rational())
        outFrame.setTimeBase(inPacket.timeBase());
    else
        outFrame.setTimeBase(m_stream.timeBase());

    AVFrame *frame = outFrame.raw();

    if (frame->pts == MediaWrapper::AV::NoPts)
        frame->pts = MediaWrapper::AV::frame::get_best_effort_timestamp(frame);

#if LIBAVCODEC_VERSION_MAJOR < 57
    if (frame->pts == av::NoPts)
        frame->pts = frame->pkt_pts;
#endif

    if (frame->pts == MediaWrapper::AV::NoPts)
        frame->pts = frame->pkt_dts;

    // Convert to decoder/frame time base. Seems not nessesary.
    outFrame.setTimeBase(timeBase());

    if (inPacket)
        outFrame.setStreamIndex(inPacket.streamIndex());
    else
        outFrame.setStreamIndex(m_stream.index());

    outFrame.setComplete(true);

    return st;
}

template<typename T>
std::pair<int, const std::error_category*>
CodecContext::encodeCommon(Packet &outPacket,
             const T &inFrame,
             int &gotPacket,
             int (*encodeProc)(AVCodecContext *, AVPacket *, const AVFrame *, int *))
{
    auto st = encodeCommon(outPacket, inFrame.raw(), gotPacket, encodeProc);
    if (std::get<1>(st))
        return st;
    if (!gotPacket)
        return std::make_pair(0u, nullptr);

    if (inFrame && inFrame.timeBase() != Rational()) {
        outPacket.setTimeBase(inFrame.timeBase());
        outPacket.setStreamIndex(inFrame.streamIndex());
    } else if (m_stream.isValid()) {
        outPacket.setTimeBase(av_stream_get_codec_timebase(m_stream.raw()));
        outPacket.setStreamIndex(m_stream.index());
    }

    // Recalc PTS/DTS/Duration
    if (m_stream.isValid()) {
        outPacket.setTimeBase(m_stream.timeBase());
    }

    outPacket.setComplete(true);

    return st;
}


#undef warnIfNotAudio
#undef warnIfNotVideo

namespace codec_context::audio {

    //
    // TBD: make a generic function
    //
    void set_channels(AVCodecContext *obj, int channels)
    {
    #if API_NEW_CHANNEL_LAYOUT
        if (!av_channel_layout_check(&obj->ch_layout) || (obj->ch_layout.nb_channels != channels)) {
            av_channel_layout_uninit(&obj->ch_layout);
            av_channel_layout_default(&obj->ch_layout, channels);
        }
    #else
        obj->channels = channels;
        if (obj->channel_layout != 0 || av_get_channel_layout_nb_channels(obj->channel_layout) != channels) {
            obj->channel_layout = av_get_default_channel_layout(channels);
        }
    #endif
    }

    void set_channel_layout_mask(AVCodecContext *obj, uint64_t mask)
    {
    #if API_NEW_CHANNEL_LAYOUT
        if (!av_channel_layout_check(&obj->ch_layout) ||
            (obj->ch_layout.order != AV_CHANNEL_ORDER_NATIVE) ||
            ((obj->ch_layout.order == AV_CHANNEL_ORDER_NATIVE) && (obj->ch_layout.u.mask != mask))) {
            av_channel_layout_uninit(&obj->ch_layout);
            av_channel_layout_from_mask(&obj->ch_layout, mask);
        }
    #else
        obj->channel_layout = mask;

        // Make channels and channel_layout sync
        if (obj->channels == 0 ||
            (uint64_t)av_get_default_channel_layout(obj->channels) != mask)
        {
            obj->channels = av_get_channel_layout_nb_channels(mask);
        }
    #endif
    }

    int get_channels(const AVCodecContext *obj)
    {
        return obj->ch_layout.nb_channels;
    }


    uint64_t get_channel_layout_mask(const AVCodecContext *obj)
    {
        return obj->ch_layout.order == AV_CHANNEL_ORDER_NATIVE ? obj->ch_layout.u.mask : 0;
    }

}

} // namespace av
