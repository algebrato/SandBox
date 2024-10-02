// Copyright © 2023 March Networks Corporation. All rights reserved.

#ifndef __MN_MEDIA_QADPAUDIOCONFIGURATIONMETADATAPACKET__H__
#define __MN_MEDIA_QADPAUDIOCONFIGURATIONMETADATAPACKET__H__

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
				class  QADPAudioConfigurationMetadataPacket : public QADPMetadataPacket
				{
				private:
					QADPAudioConfigurationMetadataPacket(uint8_t version, int8_t volume, std::unique_ptr<uint8_t[]>&& configurationBuffer, uint16_t configurationBufferSize);

				public:
					virtual ~QADPAudioConfigurationMetadataPacket();

					static std::unique_ptr<QADPAudioConfigurationMetadataPacket> Parse(uint8_t* bytes, decltype(QADPMetadataPacketHeader::length) size);

				private:
					uint8_t m_version;
					int8_t m_volume;
					const uint16_t m_configurationBufferSize;
					std::unique_ptr<uint8_t[]> m_configurationBuffer;

				public:
					decltype(m_version) GetConfigurationPacketVersion() const;
					decltype(m_volume) GetVolumePercentageLevel() const;
				};
			}
		}
	}
}

#endif
