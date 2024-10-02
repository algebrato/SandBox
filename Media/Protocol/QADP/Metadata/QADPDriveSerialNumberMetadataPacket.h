// Copyright © 2023 March Networks Corporation. All rights reserved.

#ifndef __MN_MEDIA_QADPDRIVESERIALNUMBERMETADATAPACKET__H__
#define __MN_MEDIA_QADPDRIVESERIALNUMBERMETADATAPACKET__H__

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
				class  QADPDriveSerialNumberMetadataPacket : public QADPMetadataPacket
				{
				private:
					QADPDriveSerialNumberMetadataPacket(std::unique_ptr<int8_t[]>&& serialNumber, std::size_t serialNumberLength);

				public:
					virtual ~QADPDriveSerialNumberMetadataPacket();

					static std::unique_ptr<QADPDriveSerialNumberMetadataPacket> Parse(uint8_t* bytes, decltype(QADPMetadataPacketHeader::length) size);

				private:
					std::unique_ptr<int8_t[]> m_serialNumber;
					std::size_t m_serialNumberLength;

				public:
					std::string GetSerialNumber() const;
				};
			}
		}
	}
}

#endif
