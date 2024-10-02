// Copyright © 2023 March Networks Corporation. All rights reserved.

#ifndef __MN_MEDIA_QADPFRAMEPARSER__H__
#define __MN_MEDIA_QADPFRAMEPARSER__H__

#include "../QADPFrameType.h"
#include "../Metadata/QADPMetadataPacketHeader.h"
#include "../Metadata/QADPMetadataPacketType.h"

#include <functional>
#include <memory>
#include <unordered_map>

namespace marchnetworks
{
	namespace media
	{
		namespace protocol
		{
			namespace qadp
			{
				class QADPMetadataPacket;
				struct QADPFrameAudioDataHeader;
				struct QADPFrameAuxiliaryDataHeader;
				struct QADPFrameCommonHeader;
				struct QADPFrameTextDataHeader;
				struct QADPFrameVideoDataHeader;

				class QADPFrameParser
				{
				public:
					QADPFrameParser() = delete;

					static std::unique_ptr<QADPFrameCommonHeader> ParseCommonHeader(const QADPFrameCommonHeader& networkByteOrderCommonHeader);
					static std::unique_ptr<QADPFrameAudioDataHeader> ParseDataHeader(const QADPFrameAudioDataHeader& networkByteOrderDataHeader);
					static std::unique_ptr<QADPFrameAuxiliaryDataHeader> ParseDataHeader(const QADPFrameAuxiliaryDataHeader& networkByteOrderDataHeader);
					static std::unique_ptr<QADPFrameTextDataHeader> ParseDataHeader(const QADPFrameTextDataHeader& networkByteOrderDataHeader);
					static std::unique_ptr<QADPFrameVideoDataHeader> ParseDataHeader(const QADPFrameVideoDataHeader& networkByteOrderDataHeader);
					static std::unordered_map<QADPMetadataPacketType, std::shared_ptr<QADPMetadataPacket>> ParseMetadata(const std::unique_ptr<uint8_t[]>& metadata, std::size_t size);

				private:
					static void LoadMediaPacketFramingID(const QADPFrameCommonHeader& networkByteOrderCommonHeader, QADPFrameCommonHeader& hostByteOrderCommonHeader);
					static void LoadMediaPacketFramingType(const QADPFrameCommonHeader& networkByteOrderCommonHeader, QADPFrameCommonHeader& hostByteOrderCommonHeader);

					static std::unordered_map<QADPMetadataPacketType, std::shared_ptr<QADPMetadataPacket>> ParseMetadataPackets(uint8_t* bytes, std::size_t size);
					static QADPMetadataPacketHeader ParseMetadataPacketHeader(uint8_t* bytes);
					static std::unique_ptr<QADPMetadataPacket> ParseMetadataPacket(uint8_t* bytes, decltype(QADPMetadataPacketHeader::length) size, decltype(QADPMetadataPacketHeader::type) packetType);

					static std::unordered_map<int32_t, std::function<std::unique_ptr<QADPMetadataPacket>(uint8_t*, decltype(QADPMetadataPacketHeader::length))>> s_parseMetadataPacketFunctions;
				};
			}
		}
	}
}

#endif
