// #include <stdio.h>

// typedef void (*FrameCallback)(unsigned char *videoBuffer, int length);

// #ifdef __cplusplus
// extern "C"
// {
// #endif

// #include "libavcodec/avcodec.h"
// #include "libavformat/avformat.h"
// #include "libavutil/imgutils.h"

//     typedef struct DoITDecoder
//     {
//         AVCodecContext *videoContext;
//         AVCodec *codec;
//         AVFrame *doitFrame;
//         unsigned char *decodedVideoBuffer;
//         FrameCallback receiveFrameCallback;
//         int frameLength;

//     } DoITDecoder;

//     DoITDecoder *orchidea = NULL;


//     // YUV420 Format:
//     // Size of data[0] is linesize[0] * AVFrame::height
//     // Size of data[1] is linesize[1] * (AVFrame::height / 2)
//     // Size of data[2] is linesize[2] * (AVFrame::height / 2)1
//     void postProcessYUV(AVFrame *doitFrame, unsigned char *videoBuffer, int w, int h)
//     {

//         unsigned char *fromBufferPointer = NULL;
//         unsigned char *toBufferPointer = videoBuffer;
//         int i = 0;

//         for (i = 0; i < h; i++)
//         {
//             fromBufferPointer = doitFrame->data[0] + i * doitFrame->linesize[0];
//             memcpy(toBufferPointer, fromBufferPointer, w);
//             toBufferPointer += w;
//         }

//         for (i = 0; i < h / 2; i++)
//         {
//             fromBufferPointer = doitFrame->data[1] + i * doitFrame->linesize[1];
//             memcpy(toBufferPointer, fromBufferPointer, w / 2);
//             toBufferPointer += w / 2;
//         }

//         for (i = 0; i < h / 2; i++)
//         {
//             fromBufferPointer = doitFrame->data[2] + i * doitFrame->linesize[2];
//             memcpy(toBufferPointer, fromBufferPointer, w / 2);
//             toBufferPointer += w / 2;
//         }
//     }

//     void decodeForOrchidea(AVPacket *packet)
//     {
//         int value = 0;

//         // printf("ORCHIDEA ==> Before send packet for me\n");
		
// 		AVCodecContext *codecContext = NULL;
		
// 		codecContext = orchidea->videoContext;
		
//         value = avcodec_send_packet(codecContext, packet);

//         if (value < 0)
//         {
//             printf("ORCHIDEA => Packet error %d. Flower won't grow :( \n", value);
// 			return;
//         }

//         // printf("ORCHIDEA ==> Before receive frame for me \n");

//         value = avcodec_receive_frame(codecContext, orchidea->doitFrame);
        
//         // printf("ORCHIDEA ==> After receive frame for me \n");
 
//     }

//     void openOrchidea(long receiveCallback, int width, int height)
//     {
//         printf("ORCHIDEA ==> open for me \n");

//         orchidea = (DoITDecoder *)av_mallocz(sizeof(DoITDecoder));
		
//         printf("ORCHIDEA ==> register all functions for me\n");

//         orchidea->videoContext = avcodec_alloc_context3(NULL);
		
// 		printf("ORCHIDEA ==> codec set strictly to h264\n");
//         orchidea->codec = avcodec_find_decoder(AV_CODEC_ID_H264);


//         orchidea->videoContext->pix_fmt = AV_PIX_FMT_YUV420P;
//         orchidea->videoContext->width = width;
//         orchidea->videoContext->height = height;


//         orchidea->frameLength = av_image_get_buffer_size(
//             orchidea->videoContext->pix_fmt,
//             orchidea->videoContext->width,
//             orchidea->videoContext->height,
//             16);

//         orchidea->decodedVideoBuffer = (unsigned char *)av_mallocz(3 * orchidea->frameLength);

//         orchidea->receiveFrameCallback = (FrameCallback)receiveCallback;

//         if (avcodec_open2(orchidea->videoContext, orchidea->codec, NULL) >= 0)
//             orchidea->doitFrame = av_frame_alloc();
//         else
//         {
//             printf("ORCHIDEA ==> avcodec_open2 was not initialized\n");
//             return;
//         }
		
// 		orchidea->doitFrame = av_frame_alloc();

//         orchidea->videoContext->pix_fmt = AV_PIX_FMT_YUV420P;
//         orchidea->videoContext->width = width;
//         orchidea->videoContext->height = height;
		
// 		printf("ORCHIDEA ==> opened for me \n");

//     }

//     void growOrchidea(unsigned char *h264buff, int length)
//     {

//         AVPacket packet = {0};

//         packet.data = h264buff;
//         packet.size = length;

		
// 		// printf("ORCHIDEA ==> Decode for me\n");

//         decodeForOrchidea(&packet);

//         postProcessYUV(orchidea->doitFrame, orchidea->decodedVideoBuffer, orchidea->videoContext->width, orchidea->videoContext->height);
//         orchidea->receiveFrameCallback(orchidea->decodedVideoBuffer, orchidea->frameLength);

//     }
	

//     void closeOrchidea()
//     {
//         if (orchidea == NULL)
//         {
//             return;
//         } else {
//             av_freep(&orchidea);
//         }
//         av_log_set_callback(NULL);
//     }

// #ifdef __cplusplus
// }
// #endif
