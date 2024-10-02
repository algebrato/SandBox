// Copyright © 2023 March Networks Corporation. All rights reserved.

#include "QADPFrameParser.h"

#include "../QADPFrameAudioDataHeader.h"
#include "../QADPFrameAuxiliaryDataHeader.h"
#include "../QADPFrameCommonHeader.h"
#include "../QADPFrameTextDataHeader.h"
#include "../QADPFrameType.h"
#include "../QADPFrameVideoDataHeader.h"
#include "../Metadata/QADPMetadataPacketHeader.h"
#include "../Metadata/QADPMetadataPackets.h"

#ifdef _WIN32
#include <WinSock2.h>
#include <Windows.h>
#else
#include <arpa/inet.h>
#include <cstring>
#endif // !_WIN32

#include <stdexcept>

#define UNKNOWN_MEDIA_PACKET_FRAMING_ID_FORMAT_STRING "unknown media packet framing ID 0x%x"
#define UNKNOWN_MEDIA_PACKET_FRAMING_TYPE_FORMAT_STRING "unknown media packet framing type %u"

using marchnetworks::media::protocol::qadp::QADPFrameAudioDataHeader;
using marchnetworks::media::protocol::qadp::QADPFrameAuxiliaryDataHeader;
using marchnetworks::media::protocol::qadp::QADPFrameCommonHeader;
using marchnetworks::media::protocol::qadp::QADPFrameParser;
using marchnetworks::media::protocol::qadp::QADPFrameTextDataHeader;
using marchnetworks::media::protocol::qadp::QADPFrameType;
using marchnetworks::media::protocol::qadp::QADPFrameVideoDataHeader;
using marchnetworks::media::protocol::qadp::QADPMetadataPacket;
using marchnetworks::media::protocol::qadp::QADPMetadataPacketHeader;
using marchnetworks::media::protocol::qadp::QADPMetadataPacketType;
using marchnetworks::media::protocol::qadp::QADPMetadataPackets;

std::unordered_map<int32_t, std::function<std::unique_ptr<QADPMetadataPacket>(uint8_t*, decltype(QADPMetadataPacketHeader::length))>> QADPFrameParser::s_parseMetadataPacketFunctions =
{
	{ static_cast<int32_t>(QADPMetadataPacketType::AudioConfiguration), std::bind(&QADPMetadataPackets<QADPMetadataPacketType::AudioConfiguration>::packetType::Parse, std::placeholders::_1, std::placeholders::_2) },
	{ static_cast<int32_t>(QADPMetadataPacketType::DriveSerialNumber), std::bind(&QADPMetadataPackets<QADPMetadataPacketType::DriveSerialNumber>::packetType::Parse, std::placeholders::_1, std::placeholders::_2) },
	{ static_cast<int32_t>(QADPMetadataPacketType::ImageAspectRatio), std::bind(&QADPMetadataPackets<QADPMetadataPacketType::ImageAspectRatio>::packetType::Parse, std::placeholders::_1, std::placeholders::_2) },
	{ static_cast<int32_t>(QADPMetadataPacketType::ImageEnhancements), std::bind(&QADPMetadataPackets<QADPMetadataPacketType::ImageEnhancements>::packetType::Parse, std::placeholders::_1, std::placeholders::_2) },
	{ static_cast<int32_t>(QADPMetadataPacketType::ImageTransforms), std::bind(&QADPMetadataPackets<QADPMetadataPacketType::ImageTransforms>::packetType::Parse, std::placeholders::_1, std::placeholders::_2) },
	{ static_cast<int32_t>(QADPMetadataPacketType::StreamChannelStatus), std::bind(&QADPMetadataPackets<QADPMetadataPacketType::StreamChannelStatus>::packetType::Parse, std::placeholders::_1, std::placeholders::_2) },
	{ static_cast<int32_t>(QADPMetadataPacketType::LensConfiguration), std::bind(&QADPMetadataPackets<QADPMetadataPacketType::LensConfiguration>::packetType::Parse, std::placeholders::_1, std::placeholders::_2) },
	{ static_cast<int32_t>(QADPMetadataPacketType::Seproban), std::bind(&QADPMetadataPackets<QADPMetadataPacketType::Seproban>::packetType::Parse, std::placeholders::_1, std::placeholders::_2) },
	{ static_cast<int32_t>(QADPMetadataPacketType::SmallMosaicPrivacyPatches), std::bind(&QADPMetadataPackets<QADPMetadataPacketType::SmallMosaicPrivacyPatches>::packetType::Parse, std::placeholders::_1, std::placeholders::_2) },
	{ static_cast<int32_t>(QADPMetadataPacketType::MosaicPrivacyPatches), std::bind(&QADPMetadataPackets<QADPMetadataPacketType::MosaicPrivacyPatches>::packetType::Parse, std::placeholders::_1, std::placeholders::_2) },
	{ static_cast<int32_t>(QADPMetadataPacketType::BlackPrivacyPatches), std::bind(&QADPMetadataPackets<QADPMetadataPacketType::BlackPrivacyPatches>::packetType::Parse, std::placeholders::_1, std::placeholders::_2) }
};

std::unique_ptr<QADPFrameCommonHeader> QADPFrameParser::ParseCommonHeader(const QADPFrameCommonHeader& networkByteOrderCommonHeader)
{
	auto hostByteOrderCommonHeader = std::make_unique<QADPFrameCommonHeader>();

	LoadMediaPacketFramingID(networkByteOrderCommonHeader, *hostByteOrderCommonHeader);
	LoadMediaPacketFramingType(networkByteOrderCommonHeader, *hostByteOrderCommonHeader);
	hostByteOrderCommonHeader->frameSequenceNumber = ntohl(networkByteOrderCommonHeader.frameSequenceNumber);
	hostByteOrderCommonHeader->metadataSize = ntohl(networkByteOrderCommonHeader.metadataSize);
	hostByteOrderCommonHeader->dataSize = ntohl(networkByteOrderCommonHeader.dataSize);
	hostByteOrderCommonHeader->realTimestampHighOrderBits = ntohl(networkByteOrderCommonHeader.realTimestampHighOrderBits);
	hostByteOrderCommonHeader->realTimestampLowOrderBits = ntohl(networkByteOrderCommonHeader.realTimestampLowOrderBits);
	hostByteOrderCommonHeader->synchronizationTimestampHighOrderBits = ntohl(networkByteOrderCommonHeader.synchronizationTimestampHighOrderBits);
	hostByteOrderCommonHeader->synchronizationTimestampLowOrderBits = ntohl(networkByteOrderCommonHeader.synchronizationTimestampLowOrderBits);
	hostByteOrderCommonHeader->flags = ntohl(networkByteOrderCommonHeader.flags);

	return hostByteOrderCommonHeader;
}

std::unique_ptr<QADPFrameAudioDataHeader> QADPFrameParser::ParseDataHeader(const QADPFrameAudioDataHeader& networkByteOrderDataHeader)
{
	auto hostByteOrderDataHeader = std::make_unique<QADPFrameAudioDataHeader>();

	hostByteOrderDataHeader->requestID = ntohl(networkByteOrderDataHeader.requestID);
	hostByteOrderDataHeader->numberOfChannels = networkByteOrderDataHeader.numberOfChannels;
	hostByteOrderDataHeader->numberOfBitsPerSample = networkByteOrderDataHeader.numberOfBitsPerSample;
	hostByteOrderDataHeader->playbackSpeedPercentage = ntohs(networkByteOrderDataHeader.playbackSpeedPercentage);
	hostByteOrderDataHeader->samplingRate = ntohl(networkByteOrderDataHeader.samplingRate);
	memcpy(hostByteOrderDataHeader->codec, networkByteOrderDataHeader.codec, sizeof(hostByteOrderDataHeader->codec));
	hostByteOrderDataHeader->codec[sizeof(networkByteOrderDataHeader.codec) - 1] = 0; // ensure null termination
	memcpy(hostByteOrderDataHeader->encoderID, networkByteOrderDataHeader.encoderID, sizeof(hostByteOrderDataHeader->encoderID));
	hostByteOrderDataHeader->encoderID[sizeof(networkByteOrderDataHeader.encoderID) - 1] = 0; // ensure null termination

	return hostByteOrderDataHeader;
}

std::unique_ptr<QADPFrameAuxiliaryDataHeader> QADPFrameParser::ParseDataHeader(const QADPFrameAuxiliaryDataHeader& networkByteOrderDataHeader)
{
	auto hostByteOrderDataHeader = std::make_unique<QADPFrameAuxiliaryDataHeader>();

	hostByteOrderDataHeader->requestID = ntohl(networkByteOrderDataHeader.requestID);
	memcpy(hostByteOrderDataHeader->codec, networkByteOrderDataHeader.codec, sizeof(hostByteOrderDataHeader->codec));
	hostByteOrderDataHeader->codec[sizeof(networkByteOrderDataHeader.codec) - 1] = 0; // ensure null termination
	memcpy(hostByteOrderDataHeader->encoderID, networkByteOrderDataHeader.encoderID, sizeof(hostByteOrderDataHeader->encoderID));
	hostByteOrderDataHeader->encoderID[sizeof(networkByteOrderDataHeader.encoderID) - 1] = 0; // ensure null termination

	return hostByteOrderDataHeader;
}

std::unique_ptr<QADPFrameTextDataHeader> QADPFrameParser::ParseDataHeader(const QADPFrameTextDataHeader& networkByteOrderDataHeader)
{
	auto hostByteOrderDataHeader = std::make_unique<QADPFrameTextDataHeader>();

	hostByteOrderDataHeader->sourceIdSize = ntohl(networkByteOrderDataHeader.sourceIdSize);
	hostByteOrderDataHeader->encodingSize = ntohl(networkByteOrderDataHeader.encodingSize);
	hostByteOrderDataHeader->textSampleTitleLength = ntohl(networkByteOrderDataHeader.textSampleTitleLength);
	hostByteOrderDataHeader->textSamplePrefixLength = ntohl(networkByteOrderDataHeader.textSamplePrefixLength);
	hostByteOrderDataHeader->textSampleLength = ntohl(networkByteOrderDataHeader.textSampleLength);

	return hostByteOrderDataHeader;
}

std::unique_ptr<QADPFrameVideoDataHeader> QADPFrameParser::ParseDataHeader(const QADPFrameVideoDataHeader& networkByteOrderDataHeader)
{
	auto hostByteOrderDataHeader = std::make_unique<QADPFrameVideoDataHeader>();

	hostByteOrderDataHeader->requestID = ntohl(networkByteOrderDataHeader.requestID);
	hostByteOrderDataHeader->width = ntohl(networkByteOrderDataHeader.width);
	hostByteOrderDataHeader->height = ntohl(networkByteOrderDataHeader.height);
	memcpy(hostByteOrderDataHeader->codec, networkByteOrderDataHeader.codec, sizeof(hostByteOrderDataHeader->codec));
	hostByteOrderDataHeader->codec[sizeof(networkByteOrderDataHeader.codec) - 1] = 0; // ensure null termination

	return hostByteOrderDataHeader;
}

std::unordered_map<QADPMetadataPacketType, std::shared_ptr<QADPMetadataPacket>> QADPFrameParser::ParseMetadata(const std::unique_ptr<uint8_t[]>& metadata, std::size_t size)
{
	using ProtocolVersionType = uint32_t;

	std::unordered_map<QADPMetadataPacketType, std::shared_ptr<QADPMetadataPacket>> qadpMetadataPackets;

	if (metadata && size >= sizeof(ProtocolVersionType))
	{
		auto protocolVersion = *(ProtocolVersionType*)metadata.get();

		qadpMetadataPackets = ParseMetadataPackets(metadata.get() + sizeof(ProtocolVersionType), size - sizeof(ProtocolVersionType));
	}

	return qadpMetadataPackets;
}

void QADPFrameParser::LoadMediaPacketFramingID(const QADPFrameCommonHeader& networkByteOrderCommonHeader, QADPFrameCommonHeader& hostByteOrderCommonHeader)
{
	auto mediaPacketFramingID = ntohl(networkByteOrderCommonHeader.mediaPacketFramingID);

	if (mediaPacketFramingID != DefaultMediaPacketFramingID)
	{
		char errorMessage[sizeof(UNKNOWN_MEDIA_PACKET_FRAMING_ID_FORMAT_STRING) + sizeof(mediaPacketFramingID) * 2];
		snprintf(errorMessage, sizeof(errorMessage), UNKNOWN_MEDIA_PACKET_FRAMING_ID_FORMAT_STRING, mediaPacketFramingID);
		throw std::runtime_error(errorMessage);
	}

	hostByteOrderCommonHeader.mediaPacketFramingID = mediaPacketFramingID;
}

void QADPFrameParser::LoadMediaPacketFramingType(const QADPFrameCommonHeader& networkByteOrderCommonHeader, QADPFrameCommonHeader& hostByteOrderCommonHeader)
{
	auto mediaPacketFramingType = ntohl(networkByteOrderCommonHeader.mediaPacketFramingType);

	if (mediaPacketFramingType < static_cast<decltype(mediaPacketFramingType)>(QADPFrameType::Unknown) ||
		(mediaPacketFramingType > static_cast<decltype(mediaPacketFramingType)>(QADPFrameType::AuxiliaryData) &&
			mediaPacketFramingType != static_cast<decltype(mediaPacketFramingType)>(QADPFrameType::KeepAlive) &&
			mediaPacketFramingType != static_cast<decltype(mediaPacketFramingType)>(QADPFrameType::Deepath)))
	{
		char errorMessage[sizeof(UNKNOWN_MEDIA_PACKET_FRAMING_TYPE_FORMAT_STRING) + sizeof(mediaPacketFramingType) * 4];
		snprintf(errorMessage, sizeof(errorMessage), UNKNOWN_MEDIA_PACKET_FRAMING_TYPE_FORMAT_STRING, mediaPacketFramingType);
		throw std::runtime_error(errorMessage);
	}

	hostByteOrderCommonHeader.mediaPacketFramingType = mediaPacketFramingType;
}

std::unordered_map<QADPMetadataPacketType, std::shared_ptr<QADPMetadataPacket>> QADPFrameParser::ParseMetadataPackets(uint8_t* bytes, std::size_t size)
{
	std::unordered_map<QADPMetadataPacketType, std::shared_ptr<QADPMetadataPacket>> qadpMetadataPackets;

	std::size_t parsedBytes = 0;

	while (parsedBytes + sizeof(QADPMetadataPacketHeader) <= size)
	{
		auto metadataPacketHeader = ParseMetadataPacketHeader(bytes + parsedBytes);
		parsedBytes += sizeof(QADPMetadataPacketHeader);

		if (parsedBytes + metadataPacketHeader.length <= size)
		{
			auto qadpMetadataPacket = std::move(ParseMetadataPacket(bytes + parsedBytes, metadataPacketHeader.length, metadataPacketHeader.type));
			if (qadpMetadataPacket)
				qadpMetadataPackets[qadpMetadataPacket->GetPacketType()] = std::move(qadpMetadataPacket);
			parsedBytes += metadataPacketHeader.length;
		}
		else
			parsedBytes = size;
	}

	return qadpMetadataPackets;
}

QADPMetadataPacketHeader QADPFrameParser::ParseMetadataPacketHeader(uint8_t* bytes)
{
	QADPMetadataPacketHeader qadpMetadataPacketHeader;
	memcpy(&qadpMetadataPacketHeader, bytes, sizeof(QADPMetadataPacketHeader));
	return qadpMetadataPacketHeader;
}

std::unique_ptr<QADPMetadataPacket> QADPFrameParser::ParseMetadataPacket(uint8_t* bytes, decltype(QADPMetadataPacketHeader::length) size, decltype(QADPMetadataPacketHeader::type) packetType)
{
	const auto& it = s_parseMetadataPacketFunctions.find(packetType);
	return it != s_parseMetadataPacketFunctions.end() ? it->second(bytes, size) : nullptr;
}
