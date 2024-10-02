#include <emscripten.h>
#include <emscripten/bind.h>
#include <inttypes.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include "../Media/Protocol/QADP/QADPVideoFrame.h"
#include "../Media/IO/ByteArrayStream.h"
#include "../Media/Protocol/QADP/QADPFrameSerializer.h"
typedef void (*FrameCallback)(unsigned char *videoBuffer, int length);
using namespace emscripten;
using namespace marchnetworks::media::protocol::qadp;

extern "C"
{
  #include <libavformat/avformat.h>
  #include <libavcodec/avcodec.h>
  #include <libswscale/swscale.h>
  #include "libavutil/imgutils.h"

  typedef struct Decoder {
    AVCodecContext *videoContext;
    const AVCodecID codecId;
    const AVCodec *codec;
    AVFrame *doitFrame;
    unsigned char *decodedVideoBuffer;
    FrameCallback receiveFrameCallback;
    int frameLength;
  } Decoder;

  Decoder* decoder = NULL;

  void postProcessYUV(AVFrame *doitFrame, unsigned char *videoBuffer, int w, int h)
  {

      unsigned char *fromBufferPointer = NULL;
      unsigned char *toBufferPointer = videoBuffer;
      int i = 0;

      for (i = 0; i < h; i++)
      {
          fromBufferPointer = doitFrame->data[0] + i * doitFrame->linesize[0];
          memcpy(toBufferPointer, fromBufferPointer, w);
          toBufferPointer += w;
      }

      for (i = 0; i < h / 2; i++)
      {
          fromBufferPointer = doitFrame->data[1] + i * doitFrame->linesize[1];
          memcpy(toBufferPointer, fromBufferPointer, w / 2);
          toBufferPointer += w / 2;
      }

      for (i = 0; i < h / 2; i++)
      {
          fromBufferPointer = doitFrame->data[2] + i * doitFrame->linesize[2];
          memcpy(toBufferPointer, fromBufferPointer, w / 2);
          toBufferPointer += w / 2;
      }
  }


  void OpenDecoder(long receiveCallback, int width, int height) {
    printf("OpenDecoder %iX%i \n", width, height);
    decoder->codec = avcodec_find_decoder(AV_CODEC_ID_H264);
    decoder = (Decoder*)av_mallocz(sizeof(Decoder));
    decoder->videoContext = avcodec_alloc_context3(decoder->codec);
    decoder->videoContext->pix_fmt = AV_PIX_FMT_YUV420P;
    decoder->codec = avcodec_find_decoder(AV_CODEC_ID_H264);
    decoder->videoContext->width = width;
    decoder->videoContext->height = height;

    decoder->frameLength = av_image_get_buffer_size(
      decoder->videoContext->pix_fmt,
      decoder->videoContext->width,
      decoder->videoContext->height,
      16);

    decoder->decodedVideoBuffer = (unsigned char *)av_mallocz(3 * decoder->frameLength);
    decoder->receiveFrameCallback = (FrameCallback)receiveCallback;
    avcodec_open2(decoder->videoContext, decoder->codec, NULL);
    decoder->doitFrame = av_frame_alloc();

  }

  int decode(uint8_t* buffer, size_t nDim) {
    AVCodecContext* codecContext = NULL;
    codecContext = decoder->videoContext;
    AVCodecParserContext *parser = av_parser_init(AV_CODEC_ID_H264);
    AVPacket* codedPacket = av_packet_alloc();
    AVFrame* decodedFrame = av_frame_alloc();
    
    int result = 0;

    codedPacket->data = buffer;
    codedPacket->size = nDim;


    // std::unique_ptr<uint8_t[]> byteArray(new uint8_t(*buffer));
    // marchnetworks::media::io::ByteArrayStream bs(std::move(byteArray), nDim);
    // try{
    //   auto qadpFrame = marchnetworks::media::protocol::qadp::QADPFrameSerializer::Deserialize(bs);
    //   // auto qadpFrameType = qadpFrame->GetType();
    //   // std::cout << "QADP Frame Type: " << static_cast<int>(qadpFrameType) << std::endl;

    // } catch(...) {
    //   std::cout << "Error: cannot serialize qadp frame." << std::endl;
    // }
    

    
    // {
    //   std::cout << "Video Frame" << std::endl;
    //   auto& qadpVideoFrame = static_cast<marchnetworks::media::protocol::qadp::QADPVideoFrame&>(*qadpFrame);
    //   auto codecName = qadpVideoFrame.GetCodecName();
            
    //   codedPacket->data = const_cast<uint8_t*>(qadpVideoFrame.GetData());
    //   codedPacket->size = qadpVideoFrame.GetDataSize();
    // }
    // while(nDim>0){
    // av_parser_parse2(parser, 
    //   codecContext, 
    //   &codedPacket->data, 
    //   &codedPacket->size, 
    //   buffer, 
    //   nDim, 
    //   AV_NOPTS_VALUE, AV_NOPTS_VALUE, 
    //   0);

    // buffer += codedPacket->size;
    // nDim -= codedPacket->size;

    printf("Wasm Decoding...\n");
    result = avcodec_send_packet(codecContext, codedPacket);
    if(result < 0) 
      printf("Error sending packet: err number:%s\n", av_err2str(result));


    result = avcodec_receive_frame(codecContext, decoder->doitFrame);
    if(result < 0) 
      printf("Error receiving frame: err number:%s\n", av_err2str(result));

    //decoder->videoContext = codecContext;
    av_packet_free(&codedPacket);

    if(result < 0) {
      printf("Error decoding frame: err number:%s\n", av_err2str(result));
      return -1;
    } else {
      printf("Decoded Frame!!!\n");
      av_image_copy_to_buffer(
        decoder->decodedVideoBuffer,
        1280 * 720 * 1.5,
        decoder->doitFrame->data,
        decoder->doitFrame->linesize,
        (AVPixelFormat)decoder->doitFrame->format,
        1280,
        720,
        1);


      //postProcessYUV(decoder->doitFrame, decoder->decodedVideoBuffer, decoder->videoContext->width, decoder->videoContext->height);
      decoder->receiveFrameCallback(decoder->decodedVideoBuffer, 1280*720*1.5);
    }
    // }
    return 0;
    
  } // close decode
} // close extern "C"







