// Copyright © 2023 March Networks Corporation. All rights reserved.

#ifndef __MN_MEDIA_QADPVIDEOFRAME__H__
#define __MN_MEDIA_QADPVIDEOFRAME__H__

#include "QADPFrame.h"
#include "QADPFrameVideoDataHeader.h"

#include <string>

namespace marchnetworks
{
	namespace media
	{
		namespace protocol
		{
			namespace qadp
			{
				struct QADPFrameCommonHeader;

				class  QADPVideoFrame : public QADPFrame
				{
				private:
					QADPVideoFrame(std::unique_ptr<QADPFrameCommonHeader>&& networkByteOrderCommonHeader, 
					               std::unique_ptr<QADPFrameCommonHeader>&& hostByteOrderCommonHeader,
						           std::unique_ptr<QADPFrameVideoDataHeader>&& networkByteOrderDataHeader, 
								   std::unique_ptr<QADPFrameVideoDataHeader>&& hostByteOrderDataHeader,
						           std::unique_ptr<uint8_t[]> metadata, 
								   std::unique_ptr<uint8_t[]> data
								);
				public:
					virtual ~QADPVideoFrame();

					static std::unique_ptr<QADPVideoFrame> Make(std::unique_ptr<QADPFrameCommonHeader>&& networkByteOrderCommonHeader, 
					                                            std::unique_ptr<QADPFrameCommonHeader>&& hostByteOrderCommonHeader,
						                                        std::unique_ptr<QADPFrameVideoDataHeader>&& networkByteOrderDataHeader, 
																std::unique_ptr<QADPFrameVideoDataHeader>&& hostByteOrderDataHeader,
																std::unique_ptr<uint8_t[]> metadata, 
																std::unique_ptr<uint8_t[]> data
															);

					std::string GetCodecName() const;

				private:
					std::unique_ptr<QADPFrameVideoDataHeader> m_networkByteOrderDataHeader;
					std::unique_ptr<QADPFrameVideoDataHeader> m_hostByteOrderDataHeader;
				};
			}
		}
	}
}

#endif
