// Copyright © 2023 March Networks Corporation. All rights reserved.

#include "QADPStreamChannelStatusMetadataPacket.h"

using marchnetworks::media::protocol::qadp::QADPMetadataPacket;
using marchnetworks::media::protocol::qadp::QADPMetadataPacketHeader;
using marchnetworks::media::protocol::qadp::QADPMetadataPacketType;
using marchnetworks::media::protocol::qadp::QADPStreamChannelBusyStatus;
using marchnetworks::media::protocol::qadp::QADPStreamChannelImageSlicingType;
using marchnetworks::media::protocol::qadp::QADPStreamChannelStatusMetadataPacket;

std::unordered_map<decltype(QADPStreamChannelStatusMetadataPacket::m_streamChannelBusyStatus), QADPStreamChannelBusyStatus> QADPStreamChannelStatusMetadataPacket::s_streamChannelBusyStates =
{
	{ static_cast<decltype(QADPStreamChannelStatusMetadataPacket::m_streamChannelBusyStatus)>(QADPStreamChannelBusyStatus::Streaming), QADPStreamChannelBusyStatus::Streaming },
	{ static_cast<decltype(QADPStreamChannelStatusMetadataPacket::m_streamChannelBusyStatus)>(QADPStreamChannelBusyStatus::NotStreamingIntentionally), QADPStreamChannelBusyStatus::NotStreamingIntentionally },
	{ static_cast<decltype(QADPStreamChannelStatusMetadataPacket::m_streamChannelBusyStatus)>(QADPStreamChannelBusyStatus::NotStreamingForConcomitantSeprobanAlarm), QADPStreamChannelBusyStatus::NotStreamingForConcomitantSeprobanAlarm }
};

std::unordered_map<decltype(QADPStreamChannelStatusMetadataPacket::m_imageSlicingType), QADPStreamChannelImageSlicingType> QADPStreamChannelStatusMetadataPacket::s_imageSlicingTypes =
{
	{ static_cast<decltype(QADPStreamChannelStatusMetadataPacket::m_imageSlicingType)>(QADPStreamChannelImageSlicingType::NoSlicing), QADPStreamChannelImageSlicingType::NoSlicing },
	{ static_cast<decltype(QADPStreamChannelStatusMetadataPacket::m_imageSlicingType)>(QADPStreamChannelImageSlicingType::Interlaced), QADPStreamChannelImageSlicingType::Interlaced },
	{ static_cast<decltype(QADPStreamChannelStatusMetadataPacket::m_imageSlicingType)>(QADPStreamChannelImageSlicingType::Sliced_2x2), QADPStreamChannelImageSlicingType::Sliced_2x2 },
	{ static_cast<decltype(QADPStreamChannelStatusMetadataPacket::m_imageSlicingType)>(QADPStreamChannelImageSlicingType::Sliced_3x3), QADPStreamChannelImageSlicingType::Sliced_3x3 },
	{ static_cast<decltype(QADPStreamChannelStatusMetadataPacket::m_imageSlicingType)>(QADPStreamChannelImageSlicingType::Sliced_4x4), QADPStreamChannelImageSlicingType::Sliced_4x4 }
};

QADPStreamChannelStatusMetadataPacket::QADPStreamChannelStatusMetadataPacket(int8_t isUserUnauthorized, int8_t isStreamChannelBeingRecorded, int8_t isStreamChannelDisconnected, int8_t isEncryptedStreamChannel, int8_t unused, int8_t streamChannelBusyStatus, int8_t imageSlicingType, int8_t isAudioOnlyStreamChannel) :
	QADPMetadataPacket(QADPMetadataPacketType::StreamChannelStatus),
	m_isUserUnauthorized(isUserUnauthorized),
	m_isStreamChannelBeingRecorded(isStreamChannelBeingRecorded),
	m_isStreamChannelDisconnected(isStreamChannelDisconnected),
	m_isEncryptedStreamChannel(isEncryptedStreamChannel),
	m_unused(unused),
	m_streamChannelBusyStatus(streamChannelBusyStatus),
	m_imageSlicingType(imageSlicingType),
	m_isAudioOnlyStreamChannel(isAudioOnlyStreamChannel)
{
}

QADPStreamChannelStatusMetadataPacket::~QADPStreamChannelStatusMetadataPacket()
{
}

std::unique_ptr<QADPStreamChannelStatusMetadataPacket> QADPStreamChannelStatusMetadataPacket::Parse(uint8_t* bytes, decltype(QADPMetadataPacketHeader::length) size)
{
	std::unique_ptr<QADPStreamChannelStatusMetadataPacket> packet;

	constexpr auto MinMetadataPacketSize = sizeof(m_isUserUnauthorized) + sizeof(m_isStreamChannelBeingRecorded) + sizeof(m_isStreamChannelDisconnected) + sizeof(m_isEncryptedStreamChannel);

	if (bytes && size >= MinMetadataPacketSize)
	{
		auto isUserUnauthorized = Read<decltype(m_isUserUnauthorized)>(bytes);
		auto isStreamChannelBeingRecorded = Read<decltype(m_isStreamChannelBeingRecorded)>(bytes);
		auto isStreamChannelDisconnected = Read<decltype(m_isStreamChannelDisconnected)>(bytes);
		auto isEncryptedStreamChannel = Read<decltype(m_isEncryptedStreamChannel)>(bytes);
		auto unused = 0;
		auto streamChannelBusyStatus =0;
		auto imageSlicingType = 0;
		auto isAudioOnlyStreamChannel = 0;

		if (size >= MinMetadataPacketSize + sizeof(m_unused) + sizeof(m_streamChannelBusyStatus) + sizeof(m_imageSlicingType) + sizeof(m_isAudioOnlyStreamChannel))
		{
			unused = Read<decltype(m_unused)>(bytes);
			streamChannelBusyStatus = Read<decltype(m_streamChannelBusyStatus)>(bytes);
			imageSlicingType = Read<decltype(m_imageSlicingType)>(bytes);
			isAudioOnlyStreamChannel = Read<decltype(m_isAudioOnlyStreamChannel)>(bytes);
		}

		packet = std::move(std::unique_ptr<QADPStreamChannelStatusMetadataPacket>(new QADPStreamChannelStatusMetadataPacket(isUserUnauthorized, isStreamChannelBeingRecorded, isStreamChannelDisconnected, isEncryptedStreamChannel, unused, streamChannelBusyStatus, imageSlicingType, isAudioOnlyStreamChannel)));
	}

	return packet;
}

QADPStreamChannelBusyStatus QADPStreamChannelStatusMetadataPacket::GetStreamChannelBusyStatus() const
{
	const auto& it = s_streamChannelBusyStates.find(m_streamChannelBusyStatus);
	return it != s_streamChannelBusyStates.end() ? it->second : QADPStreamChannelBusyStatus::Streaming;
}

QADPStreamChannelImageSlicingType QADPStreamChannelStatusMetadataPacket::GetStreamChannelImageSlicingType() const
{
	const auto& it = s_imageSlicingTypes.find(m_imageSlicingType);
	return it != s_imageSlicingTypes.end() ? it->second : QADPStreamChannelImageSlicingType::NoSlicing;
}

bool QADPStreamChannelStatusMetadataPacket::IsAudioOnlyStreamChannel() const
{
	return m_isAudioOnlyStreamChannel != 0;
}

bool QADPStreamChannelStatusMetadataPacket::IsEncryptedStreamChannel() const
{
	return m_isEncryptedStreamChannel != 0;
}

bool QADPStreamChannelStatusMetadataPacket::IsUserUnauthorized() const
{
	return m_isUserUnauthorized != 0;
}

bool QADPStreamChannelStatusMetadataPacket::IsStreamChannelBeingRecorded() const
{
	return m_isStreamChannelBeingRecorded != 0;
}

bool QADPStreamChannelStatusMetadataPacket::IsStreamChannelDisconnected() const
{
	return m_isStreamChannelDisconnected != 0;
}
