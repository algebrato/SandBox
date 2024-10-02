// Copyright © 2023 March Networks Corporation. All rights reserved.

#ifndef __MN_MEDIA_QADPAUDIOFRAME__H__
#define __MN_MEDIA_QADPAUDIOFRAME__H__

#include "QADPFrame.h"
#include "QADPFrameAudioDataHeader.h"

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

				class  QADPAudioFrame : public QADPFrame
				{
				private:
					QADPAudioFrame(std::unique_ptr<QADPFrameCommonHeader>&& networkByteOrderCommonHeader, std::unique_ptr<QADPFrameCommonHeader>&& hostByteOrderCommonHeader,
						std::unique_ptr<QADPFrameAudioDataHeader>&& networkByteOrderDataHeader, std::unique_ptr<QADPFrameAudioDataHeader>&& hostByteOrderDataHeader,
						std::unique_ptr<uint8_t[]> metadata, std::unique_ptr<uint8_t[]> data);
				public:
					virtual ~QADPAudioFrame();

					static std::unique_ptr<QADPAudioFrame> Make(std::unique_ptr<QADPFrameCommonHeader>&& networkByteOrderCommonHeader, std::unique_ptr<QADPFrameCommonHeader>&& hostByteOrderCommonHeader,
						std::unique_ptr<QADPFrameAudioDataHeader>&& networkByteOrderDataHeader, std::unique_ptr<QADPFrameAudioDataHeader>&& hostByteOrderDataHeader,
						std::unique_ptr<uint8_t[]> metadata, std::unique_ptr<uint8_t[]> data);

					std::string GetCodecName() const;
					decltype(QADPFrameAudioDataHeader::numberOfChannels) GetNumberOfChannels() const;
					decltype(QADPFrameAudioDataHeader::numberOfBitsPerSample) GetNumberOfBitsPerSample() const;
					decltype(QADPFrameAudioDataHeader::samplingRate) GetSamplingRate() const;

				private:
					std::unique_ptr<QADPFrameAudioDataHeader> m_networkByteOrderDataHeader;
					std::unique_ptr<QADPFrameAudioDataHeader> m_hostByteOrderDataHeader;
				};
			}
		}
	}
}

#endif
