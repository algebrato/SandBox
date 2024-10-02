// Copyright © 2023 March Networks Corporation. All rights reserved.

#include "QADPFrame.h"

#include "../../Chrono/Timestamp.h"
#include "QADPFrameCommonHeader.h"
#include "QADPFrameFlags.h"
#include "QADPFrames.h"
#include "Services/QADPFrameParser.h"

using marchnetworks::media::chrono::Timestamp;
using marchnetworks::media::protocol::qadp::QADPFrame;
using marchnetworks::media::protocol::qadp::QADPFrameCommonHeader;
using marchnetworks::media::protocol::qadp::QADPFrameFlags;
using marchnetworks::media::protocol::qadp::QADPFrameParser;
using marchnetworks::media::protocol::qadp::QADPFrameType;
using marchnetworks::media::protocol::qadp::QADPFrames;

const std::unordered_map<QADPFrameType, std::size_t> QADPFrame::s_qadpFrameTypeToHeaderSizeMap =
{
	{ QADPFrameType::Audio, sizeof(QADPFrames<QADPFrameType::Audio>::headerType) },
	{ QADPFrameType::AuxiliaryData, sizeof(QADPFrames<QADPFrameType::AuxiliaryData>::headerType) },
	{ QADPFrameType::Text, sizeof(QADPFrames<QADPFrameType::Text>::headerType) },
	{ QADPFrameType::Video, sizeof(QADPFrames<QADPFrameType::Video>::headerType) }
};

QADPFrame::QADPFrame(std::unique_ptr<QADPFrameCommonHeader>&& networkByteOrderCommonHeader, std::unique_ptr<QADPFrameCommonHeader>&& hostByteOrderCommonHeader,
	std::unique_ptr<uint8_t[]>&& metadata, std::unique_ptr<uint8_t[]>&& data) :
	m_networkByteOrderCommonHeader(std::move(networkByteOrderCommonHeader)),
	m_hostByteOrderCommonHeader(std::move(hostByteOrderCommonHeader)),
	m_metadata(std::move(metadata)),
	m_metadataSize(m_hostByteOrderCommonHeader->metadataSize >= 0 ? static_cast<std::size_t>(m_hostByteOrderCommonHeader->metadataSize) : 0),
	m_data(std::move(data)),
	m_dataSize(m_hostByteOrderCommonHeader->dataSize >= 0 ? static_cast<std::size_t>(m_hostByteOrderCommonHeader->dataSize) : 0),
	m_timestamp(std::chrono::microseconds(((uint64_t)m_hostByteOrderCommonHeader->realTimestampHighOrderBits << 32) + m_hostByteOrderCommonHeader->realTimestampLowOrderBits))
{
	if (m_hostByteOrderCommonHeader->metadataSize > 0)
		m_metadataPackets = QADPFrameParser::ParseMetadata(m_metadata, m_hostByteOrderCommonHeader->metadataSize);
}

QADPFrame::~QADPFrame()
{
}

std::size_t QADPFrame::GetCommonHeaderSize() const
{
	return sizeof(QADPFrameCommonHeader);
}

std::size_t QADPFrame::GetDataHeaderSize() const
{
	return HasDataHeader() ? s_qadpFrameTypeToHeaderSizeMap.at(GetType()) : 0;
}

const uint8_t* QADPFrame::GetData() const
{
	return m_data.get();
}

std::size_t QADPFrame::GetDataSize() const
{
	return m_dataSize;
}

const uint8_t* QADPFrame::GetMetaData() const
{
	return m_metadata.get();
}

std::size_t QADPFrame::GetMetaDataSize() const
{
	return m_metadataSize;
}

Timestamp QADPFrame::GetTimestamp() const
{
	return Timestamp(m_timestamp);
}

QADPFrameType QADPFrame::GetType() const
{
	return static_cast<QADPFrameType>(m_hostByteOrderCommonHeader->mediaPacketFramingType);
}

bool QADPFrame::HasDataHeader() const
{
	return GetType() != QADPFrameType::Video || IsKeyFrame();
}

bool QADPFrame::IsKeyFrame() const
{
	return m_hostByteOrderCommonHeader->flags & QADPFrameFlags::MEDIA_KEY_FRAME;
}
