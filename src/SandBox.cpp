#include <emscripten.h>
#include <emscripten/bind.h>
#include <inttypes.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include "MediaWrapper.h"
#include "../Media/IO/ByteArrayStream.h"
#include "../Media/Protocol/QADP/QADPVideoFrame.h"
#include "../Media/Protocol/QADP/QADPFrameSerializer.h"


typedef void (*FrameCallback)(unsigned char *videoBuffer, int length);
using namespace emscripten;
using namespace MediaWrapper::AV;

extern "C"
{
  typedef struct Decoder
  {
    MediaWrapper::AV::VideoDecoderContext v_dec;
    FrameCallback frameCallback;
    int frameLength;
  } Decoder;

  Decoder *decoder = nullptr;
  
  void OpenDecoder(long receiveCallback, int width, int height) 
  {
    printf("OpenDecoder %iX%i \n", width, height);
    MediaWrapper::AV::init();

    decoder = (Decoder*)av_mallocz(sizeof(Decoder));
    decoder->v_dec = MediaWrapper::AV::VideoDecoderContext();
    decoder->v_dec.setCodec(MediaWrapper::AV::findDecodingCodec(AV_CODEC_ID_H264));
    decoder->v_dec.setPixelFormat(AV_PIX_FMT_YUV420P);
    decoder->v_dec.setWidth(width);
    decoder->v_dec.setHeight(height);
    decoder->frameCallback = (FrameCallback)receiveCallback;

    decoder->frameLength = av_image_get_buffer_size(decoder->v_dec.pixelFormat().get(), width, height, 1);

    decoder->v_dec.open();
    
  }

  int decode(uint8_t* buffer, size_t nDim) 
  {

    std::error_code ec;
    
    std::vector<uint8_t> byteArray(buffer, buffer + nDim);
    std::unique_ptr<uint8_t[]> uniqueByteArray(new uint8_t[nDim]);
    std::copy(byteArray.begin(), byteArray.end(), uniqueByteArray.get());

    marchnetworks::media::io::ByteArrayStream bs(std::move(uniqueByteArray), nDim);

    auto QADPFrame = marchnetworks::media::protocol::qadp::QADPFrameSerializer::Deserialize(bs);
    auto& qadpVideoFrame = static_cast<marchnetworks::media::protocol::qadp::QADPVideoFrame&>(*QADPFrame);
    

    Packet packet;
    packet.setData(qadpVideoFrame.GetData(), qadpVideoFrame.GetDataSize());

    auto frame = decoder->v_dec.decode(packet, ec);

    if(ec)
    {
      printf("Error decoding frame: %s\n", ec.message().c_str());
      return -1;
    } else {
      printf("Decoded frame\n");
    }


    auto decodedVideoBuffer = (unsigned char *)av_mallocz(3*decoder->frameLength);
    av_image_copy_to_buffer(
      decodedVideoBuffer, 
      3*decoder->frameLength, 
      (const uint8_t * const *)frame.raw()->data, 
      frame.raw()->linesize, 
      decoder->v_dec.pixelFormat().get(), 
      decoder->v_dec.width(), 
      decoder->v_dec.height(), 
      1
    );

    decoder->frameCallback(decodedVideoBuffer, decoder->v_dec.width() * decoder->v_dec.height() * 3 / 2);

    av_free(decodedVideoBuffer);
    byteArray.clear();
    byteArray.shrink_to_fit();
    
   
    return 0;
    
  } 
} 







