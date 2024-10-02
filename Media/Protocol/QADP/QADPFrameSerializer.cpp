// Copyright © 2024 March Networks Corporation. All rights reserved.

#include "QADPFrameSerializer.h"

#include "QADPFrameFlags.h"
#include "QADPFrames.h"
#include "../../IO/IInputStream.h"

#ifndef _WIN32
#include <arpa/inet.h>
#endif // !_WIN32

#include <istream>

using marchnetworks::media::io::IInputStream;
using marchnetworks::media::protocol::qadp::QADPFrame;
using marchnetworks::media::protocol::qadp::QADPFrameCommonHeader;
using marchnetworks::media::protocol::qadp::QADPFrameFlags;
using marchnetworks::media::protocol::qadp::QADPFrameParser;
using marchnetworks::media::protocol::qadp::QADPFrameSerializer;
using marchnetworks::media::protocol::qadp::QADPFrameType;
using marchnetworks::media::protocol::qadp::QADPFrames;

std::unordered_map<QADPFrameType, std::function<std::unique_ptr<QADPFrame>(IInputStream&, std::unique_ptr<QADPFrameCommonHeader>&&, std::unique_ptr<QADPFrameCommonHeader>&&)>> QADPFrameSerializer::s_deserializationFunctions =
{
	{ QADPFrameType::Audio, std::bind(&QADPFrameSerializer::Deserialize<QADPFrameType::Audio>, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3) },
	{ QADPFrameType::AuxiliaryData, std::bind(&QADPFrameSerializer::Deserialize<QADPFrameType::AuxiliaryData>, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3) },
	{ QADPFrameType::EndOfStream, std::bind(&QADPFrameSerializer::Deserialize<QADPFrameType::EndOfStream>, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3) },
	{ QADPFrameType::Text, std::bind(&QADPFrameSerializer::Deserialize<QADPFrameType::Text>, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3) },
	{ QADPFrameType::Video, std::bind(&QADPFrameSerializer::Deserialize<QADPFrameType::Video>, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3) }
};

std::unique_ptr<QADPFrame> QADPFrameSerializer::Deserialize(IInputStream& stream)
{
	std::unique_ptr<QADPFrame> qadpFrame;

	auto networkByteOrderCommonHeader = std::move(ReadBytes<QADPFrameCommonHeader>(stream));
	if (networkByteOrderCommonHeader)
	{
		auto hostByteOrderCommonHeader = std::move(QADPFrameParser::ParseCommonHeader(*networkByteOrderCommonHeader));
		auto qadpFrameType = static_cast<QADPFrameType>(hostByteOrderCommonHeader->mediaPacketFramingType);

		if (s_deserializationFunctions.find(qadpFrameType) != s_deserializationFunctions.end())
			qadpFrame = s_deserializationFunctions[qadpFrameType](stream, std::move(networkByteOrderCommonHeader), std::move(hostByteOrderCommonHeader));
	}

	return qadpFrame;
}

template <>
std::unique_ptr<QADPFrame> QADPFrameSerializer::Deserialize<QADPFrameType::EndOfStream>(IInputStream& stream, std::unique_ptr<QADPFrameCommonHeader>&& networkByteOrderCommonHeader, std::unique_ptr<QADPFrameCommonHeader>&& hostByteOrderCommonHeader)
{
	return nullptr;
}

template <>
std::unique_ptr<QADPFrame> QADPFrameSerializer::Deserialize<QADPFrameType::Text>(IInputStream& stream, std::unique_ptr<QADPFrameCommonHeader>&& networkByteOrderCommonHeader, std::unique_ptr<QADPFrameCommonHeader>&& hostByteOrderCommonHeader)
{
	std::unique_ptr<QADPFrame> qadpFrame;

	auto networkByteOrderDataHeader = std::move(ReadBytes<QADPFrames<QADPFrameType::Text>::headerType>(stream));
	if (networkByteOrderDataHeader)
	{
		auto hostByteOrderDataHeader = std::move(QADPFrameParser::ParseDataHeader(*networkByteOrderDataHeader));

		std::unique_ptr<uint8_t[]> textSourceId;
		if (hostByteOrderDataHeader->sourceIdSize <= 0 || (textSourceId = ReadBytes(stream, hostByteOrderDataHeader->sourceIdSize)))
		{
			std::unique_ptr<uint8_t[]> data;
			if (hostByteOrderCommonHeader->dataSize <= 0 || (data = ReadBytes(stream, hostByteOrderCommonHeader->dataSize)))
				qadpFrame = std::move(QADPFrames<QADPFrameType::Text>::frameType::Make(std::move(networkByteOrderCommonHeader), std::move(hostByteOrderCommonHeader),
					std::move(networkByteOrderDataHeader), std::move(hostByteOrderDataHeader),
					std::unique_ptr<char[]>(reinterpret_cast<char*>(textSourceId.release())), std::move(data)));
		}
	}

	return qadpFrame;
}

template <>
std::unique_ptr<QADPFrame> QADPFrameSerializer::Deserialize<QADPFrameType::Video>(IInputStream& stream, std::unique_ptr<QADPFrameCommonHeader>&& networkByteOrderCommonHeader, std::unique_ptr<QADPFrameCommonHeader>&& hostByteOrderCommonHeader)
{
	std::unique_ptr<QADPFrame> qadpFrame;

	auto isKeyFrame = hostByteOrderCommonHeader->flags & QADPFrameFlags::MEDIA_KEY_FRAME;

	std::unique_ptr<QADPFrames<QADPFrameType::Video>::headerType> networkByteOrderDataHeader;
	std::unique_ptr<QADPFrames<QADPFrameType::Video>::headerType> hostByteOrderDataHeader;

	if (isKeyFrame && (networkByteOrderDataHeader = std::move(ReadBytes<QADPFrames<QADPFrameType::Video>::headerType>(stream))))
		hostByteOrderDataHeader = std::move(QADPFrameParser::ParseDataHeader(*networkByteOrderDataHeader));

	if (!isKeyFrame || (networkByteOrderDataHeader && hostByteOrderDataHeader))
	{
		std::unique_ptr<uint8_t[]> metadata;
		if (hostByteOrderCommonHeader->metadataSize <= 0 || (metadata = ReadBytes(stream, hostByteOrderCommonHeader->metadataSize)))
		{
			std::unique_ptr<uint8_t[]> data;
			if (hostByteOrderCommonHeader->dataSize <= 0 || (data = ReadBytes(stream, hostByteOrderCommonHeader->dataSize)))
				qadpFrame = std::move(QADPFrames<QADPFrameType::Video>::frameType::Make(std::move(networkByteOrderCommonHeader), std::move(hostByteOrderCommonHeader),
					std::move(networkByteOrderDataHeader), std::move(hostByteOrderDataHeader),
					std::move(metadata), std::move(data)));
		}
	}

	return qadpFrame;
}

std::unique_ptr<uint8_t[]> QADPFrameSerializer::ReadBytes(IInputStream& stream, std::size_t numberOfBytesToRead)
{
	auto buffer = std::move(std::unique_ptr<uint8_t[]>(new uint8_t[numberOfBytesToRead]));
	auto bytesRead = stream.Read(buffer.get(), numberOfBytesToRead);
	if (bytesRead == 0)
		buffer.reset(nullptr);
	else if (bytesRead < numberOfBytesToRead)
	{
		buffer.reset(nullptr);
		throw std::istream::failure("end of stream reached while reading the media packet framing ID");
	}

	return buffer;
}
