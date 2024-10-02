// Copyright © 2023 March Networks Corporation. All rights reserved.

#ifndef __MN_MEDIA_QADPMETADATAPACKET__H__
#define __MN_MEDIA_QADPMETADATAPACKET__H__

#include "QADPMetadataPacketType.h"

#include <cstddef>
#include <cstdint>

namespace marchnetworks
{
	namespace media
	{
		namespace protocol
		{
			namespace qadp
			{
				class QADPMetadataPacket
				{
				protected:
					QADPMetadataPacket(QADPMetadataPacketType qadpMetadataPacketType);

				public:
					virtual ~QADPMetadataPacket();

					QADPMetadataPacketType GetPacketType() const;

				protected:
					template <typename T>
					static T Read(uint8_t*& bytes);

					template <typename T>
					static T* Read(uint8_t*& bytes, std::size_t numberOfBytes);

				private:
					const QADPMetadataPacketType m_packetType;
				};

				template <typename T>
				T QADPMetadataPacket::Read(uint8_t*& bytes)
				{
					auto value = *reinterpret_cast<T*>(bytes);
					bytes += sizeof(T);
					return value;
				}

				template <typename T>
				T* QADPMetadataPacket::Read(uint8_t*& bytes, std::size_t byteOffset)
				{
					bytes += byteOffset;
					return reinterpret_cast<T*>(bytes);
				}
			}
		}
	}
}

#endif
