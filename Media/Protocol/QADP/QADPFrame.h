// Copyright © 2023 March Networks Corporation. All rights reserved.

#ifndef __MN_MEDIA_QADPFRAME__H__
#define __MN_MEDIA_QADPFRAME__H__

#include "QADPFrameType.h"
#include "Metadata/QADPMetadataPackets.h"

#include <chrono>
#include <memory>
#include <unordered_map>

namespace marchnetworks
{
	namespace media
	{
		namespace chrono
		{
			class Timestamp;
		}

		namespace protocol
		{
			namespace qadp
			{
				class QADPMetadataPacket;
				struct QADPFrameCommonHeader;

				class  QADPFrame
				{
				protected:
					QADPFrame(std::unique_ptr<QADPFrameCommonHeader>&& networkByteOrderCommonHeader, std::unique_ptr<QADPFrameCommonHeader>&& hostByteOrderCommonHeader,
						std::unique_ptr<uint8_t[]>&& metadata, std::unique_ptr<uint8_t[]>&& data);

				public:
					virtual ~QADPFrame();

					std::size_t GetCommonHeaderSize() const;

					std::size_t GetDataHeaderSize() const;

					const uint8_t* GetData() const;
					std::size_t GetDataSize() const;

					const uint8_t* GetMetaData() const;
					std::size_t GetMetaDataSize() const;

					template <QADPMetadataPacketType _qadpMetadataPacketType>
					std::shared_ptr<typename QADPMetadataPackets<_qadpMetadataPacketType>::packetType> GetMetadataPacket() const;

					chrono::Timestamp GetTimestamp() const;

					QADPFrameType GetType() const;

					bool HasDataHeader() const;
					bool IsKeyFrame() const;

				private:
					static const std::unordered_map<QADPFrameType, std::size_t> s_qadpFrameTypeToHeaderSizeMap;

					std::unique_ptr<QADPFrameCommonHeader> m_networkByteOrderCommonHeader;
					std::unique_ptr<QADPFrameCommonHeader> m_hostByteOrderCommonHeader;

					std::unique_ptr<uint8_t[]> m_metadata;
					std::size_t m_metadataSize;
					std::unordered_map<QADPMetadataPacketType, std::shared_ptr<QADPMetadataPacket>> m_metadataPackets;

					std::unique_ptr<uint8_t[]> m_data;
					std::size_t m_dataSize;

					std::chrono::nanoseconds m_timestamp;
				};

				template <QADPMetadataPacketType _qadpMetadataPacketType>
				std::shared_ptr<typename QADPMetadataPackets<_qadpMetadataPacketType>::packetType> QADPFrame::GetMetadataPacket() const
				{
					const auto& it = m_metadataPackets.find(_qadpMetadataPacketType);
					return it != m_metadataPackets.end() ? std::static_pointer_cast<typename QADPMetadataPackets<_qadpMetadataPacketType>::packetType>(it->second) : nullptr;
				}
			}
		}
	}
}

#endif
