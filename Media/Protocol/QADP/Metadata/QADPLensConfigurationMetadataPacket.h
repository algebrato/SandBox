// Copyright © 2023 March Networks Corporation. All rights reserved.

#ifndef __MN_MEDIA_QADPLENSCONFIGURATIONMETADATAPACKET__H__
#define __MN_MEDIA_QADPLENSCONFIGURATIONMETADATAPACKET__H__

#include "QADPMetadataPacket.h"
#include "QADPMetadataPacketHeader.h"

#include <memory>
#include <string>

namespace marchnetworks
{
	namespace media
	{
		namespace protocol
		{
			namespace qadp
			{
				class  QADPLensConfigurationMetadataPacket : public QADPMetadataPacket
				{
				private:
					QADPLensConfigurationMetadataPacket(int8_t unused[3], uint8_t moutingType, std::unique_ptr<int8_t[]>&& model, std::size_t modelLength);

				public:
					virtual ~QADPLensConfigurationMetadataPacket();

					static std::unique_ptr<QADPLensConfigurationMetadataPacket> Parse(uint8_t* bytes, decltype(QADPMetadataPacketHeader::length) size);

				private:
					int8_t m_unused[3];
					uint8_t m_mountingType;
					std::unique_ptr<int8_t[]> m_model;
					std::size_t m_modelLength;

				public:
					decltype(m_mountingType) GetMountingType() const;
					std::string GetModel() const;
				};
			}
		}
	}
}

#endif
