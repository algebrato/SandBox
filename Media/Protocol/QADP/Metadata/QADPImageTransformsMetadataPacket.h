// Copyright © 2023 March Networks Corporation. All rights reserved.

#ifndef __MN_MEDIA_QADPIMAGETRANSFORMSMETADATAPACKET__H__
#define __MN_MEDIA_QADPIMAGETRANSFORMSMETADATAPACKET__H__

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
				class  QADPImageTransformsMetadataPacket : public QADPMetadataPacket
				{
				private:
					QADPImageTransformsMetadataPacket(uint16_t flipBitField, uint16_t rotationDegrees);

				public:
					virtual ~QADPImageTransformsMetadataPacket();

					static std::unique_ptr<QADPImageTransformsMetadataPacket> Parse(uint8_t* bytes, decltype(QADPMetadataPacketHeader::length) size);

				private:
					uint16_t m_flipBitField;
					int16_t m_rotationDegrees;

				public:
					decltype(m_rotationDegrees) GetRotationDegrees() const;
					bool HasHorizontalFlipToBeApplied() const;
					bool HasVerticalFlipToBeApplied() const;
				};
			}
		}
	}
}

#endif
