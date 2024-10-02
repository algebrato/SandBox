// Copyright © 2023 March Networks Corporation. All rights reserved.

#include "QADPAudioConfigurationMetadataPacket.h"

#ifndef _WIN32
#include <cstring>
#endif // !_WIN32

using marchnetworks::media::protocol::qadp::QADPAudioConfigurationMetadataPacket;
using marchnetworks::media::protocol::qadp::QADPMetadataPacket;
using marchnetworks::media::protocol::qadp::QADPMetadataPacketHeader;
using marchnetworks::media::protocol::qadp::QADPMetadataPacketType;

QADPAudioConfigurationMetadataPacket::QADPAudioConfigurationMetadataPacket(uint8_t version, int8_t volume, std::unique_ptr<uint8_t[]>&& configurationBuffer, uint16_t configurationBufferSize) :
	QADPMetadataPacket(QADPMetadataPacketType::AudioConfiguration),
	m_version(version),
	m_volume(volume),
	m_configurationBuffer(std::move(configurationBuffer)),
	m_configurationBufferSize(configurationBufferSize)
{
}

QADPAudioConfigurationMetadataPacket::~QADPAudioConfigurationMetadataPacket()
{
}

std::unique_ptr<QADPAudioConfigurationMetadataPacket> QADPAudioConfigurationMetadataPacket::Parse(uint8_t* bytes, decltype(QADPMetadataPacketHeader::length) size)
{
	std::unique_ptr<QADPAudioConfigurationMetadataPacket> packet;

	if (bytes && sizeof(m_version) + sizeof(m_volume) + sizeof(m_configurationBufferSize) <= size)
	{
		auto version = Read<decltype(m_version)>(bytes);
		auto volume = Read<decltype(m_volume)>(bytes);
		auto configurationBufferSize = Read<decltype(m_configurationBufferSize)>(bytes);
		if (configurationBufferSize > 0)
		{
			if (configurationBufferSize + sizeof(m_version) + sizeof(m_volume) + sizeof(m_configurationBufferSize) <= size)
			{
				std::unique_ptr<decltype(m_configurationBuffer)::element_type[]> configurationBuffer(new decltype(m_configurationBuffer)::element_type[configurationBufferSize]);
				memcpy(configurationBuffer.get(), bytes, configurationBufferSize);
				packet = std::move(std::unique_ptr<QADPAudioConfigurationMetadataPacket>(new QADPAudioConfigurationMetadataPacket(version, volume, std::move(configurationBuffer), configurationBufferSize)));
			}
		}
		else
			packet = std::move(std::unique_ptr<QADPAudioConfigurationMetadataPacket>(new QADPAudioConfigurationMetadataPacket(version, volume, nullptr, 0)));
	}

	return packet;
}

decltype(QADPAudioConfigurationMetadataPacket::m_version) QADPAudioConfigurationMetadataPacket::GetConfigurationPacketVersion() const
{
	return m_version;
}

decltype(QADPAudioConfigurationMetadataPacket::m_volume) QADPAudioConfigurationMetadataPacket::GetVolumePercentageLevel() const
{
	return m_volume;
}
