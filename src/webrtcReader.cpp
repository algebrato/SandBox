// #include <iostream>
// #include <emscripten/emscripten.h>
// #include <emscripten/bind.h>
// extern "C" {
// #include <libavformat/avformat.h>
// }

// class WebRTCReader {
// public:
//     WebRTCReader() {
//         avformat_network_init();
//     }

//     ~WebRTCReader() {
//         avformat_network_deinit();
//     }

//     bool openStream(const std::string& url) {
//         AVFormatContext* formatContext = avformat_alloc_context();
//         if (!formatContext) {
//             std::cerr << "Could not allocate format context" << std::endl;
//             return false;
//         }

//         if (avformat_open_input(&formatContext, url.c_str(), nullptr, nullptr) != 0) {
//             std::cerr << "Could not open input stream" << std::endl;
//             avformat_free_context(formatContext);
//             return false;
//         }

//         if (avformat_find_stream_info(formatContext, nullptr) < 0) {
//             std::cerr << "Could not find stream info" << std::endl;
//             avformat_close_input(&formatContext);
//             return false;
//         }

//         av_dump_format(formatContext, 0, url.c_str(), 0);
//         avformat_close_input(&formatContext);
//         return true;
//     }
// };

// EMSCRIPTEN_BINDINGS(WebRTCReader) {
//     emscripten::class_<WebRTCReader>("WebRTCReader")
//         .constructor<>()
//         .function("openStream", &WebRTCReader::openStream);
// }
