// Copyright © 2023 March Networks Corporation. All rights reserved.

#ifndef __MN_MEDIA_QADPIMAGEASPECTRATIOMETADATAPACKET__H__
#define __MN_MEDIA_QADPIMAGEASPECTRATIOMETADATAPACKET__H__

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
				class  QADPImageAspectRatioMetadataPacket : public QADPMetadataPacket
				{
				private:
					QADPImageAspectRatioMetadataPacket(uint16_t aspectWidth, uint16_t aspectHeight);

				public:
					virtual ~QADPImageAspectRatioMetadataPacket();

					static std::unique_ptr<QADPImageAspectRatioMetadataPacket> Parse(uint8_t* bytes, decltype(QADPMetadataPacketHeader::length) size);

				private:
					uint16_t m_aspectWidth;
					uint16_t m_aspectHeight;

				public:
					decltype(m_aspectWidth) GetImageAspectWidth() const;
					decltype(m_aspectHeight) GetImageAspectHeight() const;
				};
			}
		}
	}
}

#endif
