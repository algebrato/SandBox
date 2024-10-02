// Copyright © 2023 March Networks Corporation. All rights reserved.

#ifndef __MN_MEDIA_QADPIMAGEENHANCEMENTSMETADATAPACKET__H__
#define __MN_MEDIA_QADPIMAGEENHANCEMENTSMETADATAPACKET__H__

#include "QADPMetadataPacket.h"
#include "QADPMetadataPacketHeader.h"

#include <memory>

namespace marchnetworks
{
	namespace media
	{
		namespace protocol
		{
			namespace qadp
			{
				class  QADPImageEnhancementsMetadataPacket : public QADPMetadataPacket
				{
				private:
					QADPImageEnhancementsMetadataPacket(int8_t brightness, uint8_t sharpness, int8_t contrast, int8_t isInterlacedVideoStream);

				public:
					virtual ~QADPImageEnhancementsMetadataPacket();

					static std::unique_ptr<QADPImageEnhancementsMetadataPacket> Parse(uint8_t* bytes, decltype(QADPMetadataPacketHeader::length) size);

				private:
					int8_t m_brightness;
					uint8_t m_sharpness;
					int8_t m_contrast;
					int8_t m_isInterlacedVideoStream;

				public:
					decltype(m_brightness) GetBrightness() const;
					decltype(m_sharpness) GetSharpness() const;
					decltype(m_contrast) GetContrast() const;
					bool IsInterlacedVideoStream() const;
				};
			}
		}
	}
}

#endif
