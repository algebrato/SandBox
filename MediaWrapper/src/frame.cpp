#include "frame.h"

using namespace std;

extern "C" {
#include <libavutil/version.h>
#include <libavutil/imgutils.h>
#include <libavutil/avassert.h>
}

using namespace MediaWrapper::AV;

VideoFrame::VideoFrame(PixelFormat pixelFormat, int width, int height, int align)
{
    m_raw->format = pixelFormat;
    m_raw->width  = width;
    m_raw->height = height;
    av_frame_get_buffer(m_raw, align);
}

VideoFrame::VideoFrame(const uint8_t *data, size_t size, PixelFormat pixelFormat, int width, int height, int align)
    : VideoFrame(pixelFormat, width, height, align)
{
    size_t calcSize = av_image_get_buffer_size(pixelFormat, width, height, align);
    if (calcSize != size)
        throw length_error("Data size and required buffer for this format/width/height/align not equal");

    uint8_t *src_buf[4];
    int      src_linesize[4];
    av_image_fill_arrays(src_buf, src_linesize, data, pixelFormat, width, height, align);

    // copy data
    av_image_copy(m_raw->data, m_raw->linesize,
                  const_cast<const uint8_t**>(src_buf), const_cast<const int*>(src_linesize),
                  pixelFormat, width, height);
}

VideoFrame::VideoFrame(const VideoFrame &other)
    : Frame<VideoFrame>(other)
{
}

VideoFrame::VideoFrame(VideoFrame &&other)
    : Frame<VideoFrame>(std::move(other))
{
}

VideoFrame &VideoFrame::operator=(const VideoFrame &rhs)
{
    return assignOperator(rhs);
}

VideoFrame &VideoFrame::operator=(VideoFrame &&rhs)
{
    return moveOperator(std::move(rhs));
}

PixelFormat VideoFrame::pixelFormat() const
{
    return static_cast<AVPixelFormat>(RAW_GET(format, AV_PIX_FMT_NONE));
}

int VideoFrame::width() const
{
    return RAW_GET(width, 0);
}

int VideoFrame::height() const
{
    return RAW_GET(height, 0);
}

bool VideoFrame::isKeyFrame() const
{
    return RAW_GET(key_frame, false);
}

void VideoFrame::setKeyFrame(bool isKey)
{
    RAW_SET(key_frame, isKey);
}

int VideoFrame::quality() const
{
    return RAW_GET(quality, 0);
}

void VideoFrame::setQuality(int quality)
{
    RAW_SET(quality, quality);
}

AVPictureType VideoFrame::pictureType() const
{
    return RAW_GET(pict_type, AV_PICTURE_TYPE_NONE);
}

void VideoFrame::setPictureType(AVPictureType type)
{
    RAW_SET(pict_type, type);
}

Rational VideoFrame::sampleAspectRatio() const
{
    return RAW_GET(sample_aspect_ratio, AVRational());
}

void VideoFrame::setSampleAspectRatio(const Rational& sampleAspectRatio)
{
    RAW_SET(sample_aspect_ratio, sampleAspectRatio);
}

size_t VideoFrame::bufferSize(int align, OptionalErrorCode ec) const
{
    clear_if(ec);
    auto sz = av_image_get_buffer_size(static_cast<AVPixelFormat>(m_raw->format), m_raw->width, m_raw->height, align);
    if (sz < 0) {
        throws_if(ec, sz, ffmpeg_category());
        return 0;
    }
    return static_cast<size_t>(sz);
}

bool VideoFrame::copyToBuffer(uint8_t *dst, size_t size, int align, OptionalErrorCode ec)
{
    clear_if(ec);
    auto sts = av_image_copy_to_buffer(dst, static_cast<int>(size), m_raw->data, m_raw->linesize, static_cast<AVPixelFormat>(m_raw->format), m_raw->width, m_raw->height, align);
    if (sts < 0) {
        throws_if(ec, sts, ffmpeg_category());
        return false;
    }
    return true;
}

bool VideoFrame::copyToBuffer(std::vector<uint8_t> &dst, int align, OptionalErrorCode ec)
{
    auto sz = bufferSize(align, ec);
    if (ec && *ec)
        return false;
    dst.resize(sz);
    return copyToBuffer(dst.data(), dst.size(), align, ec);
}


AudioSamples::AudioSamples(SampleFormat sampleFormat, int samplesCount, uint64_t channelLayout, int sampleRate, int align)
{
    init(sampleFormat, samplesCount, channelLayout, sampleRate, align);
}

int AudioSamples::init(SampleFormat sampleFormat, int samplesCount, uint64_t channelLayout, int sampleRate, int align)
{
    if (!m_raw) {
        m_raw = av_frame_alloc();
        m_raw->opaque = this;
    }

    if (m_raw->data[0]) {
        av_frame_free(&m_raw);
    }

    m_raw->format      = sampleFormat;
    m_raw->nb_samples  = samplesCount;

    MediaWrapper::AV::frame::set_sample_rate(m_raw, sampleRate);
    MediaWrapper::AV::frame::set_channel_layout(m_raw, channelLayout);

    av_frame_get_buffer(m_raw, align);
    return 0;
}

AudioSamples::AudioSamples(const uint8_t *data, size_t size, SampleFormat sampleFormat, int samplesCount, uint64_t channelLayout, int sampleRate, int align)
    : AudioSamples(sampleFormat, samplesCount, channelLayout, sampleRate, align)
{
    auto const channels = [](uint64_t mask) -> int {
#if API_NEW_CHANNEL_LAYOUT
        AVChannelLayout layout{};
        av_channel_layout_from_mask(&layout, mask);
        return layout.nb_channels;
#else
        return av_get_channel_layout_nb_channels(mask);
#endif
    } (channelLayout);

    auto calcSize = sampleFormat.requiredBufferSize(channels, samplesCount, align);

    if (calcSize > size)
        throw length_error("Data size and required buffer for this format/nb_samples/nb_channels/align not equal");

    uint8_t *buf[AV_NUM_DATA_POINTERS];
    int      linesize[AV_NUM_DATA_POINTERS];
    SampleFormat::fillArrays(buf, linesize, data, channels, samplesCount, sampleFormat, align);

    // copy data
    for (size_t i = 0; i < size_t(channels) && i < size_t(AV_NUM_DATA_POINTERS); ++i) {
        std::copy(buf[i], buf[i]+linesize[i], m_raw->data[i]);
    }
}

AudioSamples::AudioSamples(const AudioSamples &other)
    : Frame<AudioSamples>(other)
{
}

AudioSamples::AudioSamples(AudioSamples &&other)
    : Frame<AudioSamples>(std::move(other))
{
}

AudioSamples &AudioSamples::operator=(const AudioSamples &rhs)
{
    return assignOperator(rhs);
}

AudioSamples &AudioSamples::operator=(AudioSamples &&rhs)
{
    return moveOperator(std::move(rhs));
}

SampleFormat AudioSamples::sampleFormat() const
{
    return static_cast<AVSampleFormat>(RAW_GET(format, AV_SAMPLE_FMT_NONE));
}

int AudioSamples::samplesCount() const
{
    return RAW_GET(nb_samples, 0);
}

int AudioSamples::channelsCount() const
{
    return m_raw ? MediaWrapper::AV::frame::get_channels(m_raw) : 0;
}

uint64_t AudioSamples::channelsLayout() const
{
    return m_raw ? MediaWrapper::AV::frame::get_channel_layout(m_raw) : 0;
}

int AudioSamples::sampleRate() const
{
    return m_raw ? MediaWrapper::AV::frame::get_sample_rate(m_raw) : 0;
}

size_t AudioSamples::sampleBitDepth(OptionalErrorCode ec) const
{
    return m_raw
            ? SampleFormat(static_cast<AVSampleFormat>(m_raw->format)).bitsPerSample(ec)
            : 0;
}

bool AudioSamples::isPlanar() const
{
    return m_raw ? av_sample_fmt_is_planar(static_cast<AVSampleFormat>(m_raw->format)) : false;
}

string AudioSamples::channelsLayoutString() const
{
    if (!m_raw)
        return "";
    char buf[128] = {0};
#if API_NEW_CHANNEL_LAYOUT
    av_channel_layout_describe(&m_raw->ch_layout, buf, sizeof(buf));
#else
    av_get_channel_layout_string(buf,
                                 sizeof(buf),
                                 av::frame::get_channels(m_raw),
                                 av::frame::get_channel_layout(m_raw));
#endif
    return string(buf);
}

void frame::priv::channel_layout_copy(AVFrame &dst, const AVFrame &src)
{
#if API_NEW_CHANNEL_LAYOUT
    av_channel_layout_copy(&dst.ch_layout, &src.ch_layout);
#else
    dst.channel_layout = src.channel_layout;
    dst.channels       = src.channels;
#endif
}

