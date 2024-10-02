// Copyright © 2023 March Networks Corporation. All rights reserved.

#include "QADPImageEnhancementsMetadataPacket.h"

using marchnetworks::media::protocol::qadp::QADPImageEnhancementsMetadataPacket;
using marchnetworks::media::protocol::qadp::QADPMetadataPacket;
using marchnetworks::media::protocol::qadp::QADPMetadataPacketHeader;
using marchnetworks::media::protocol::qadp::QADPMetadataPacketType;

QADPImageEnhancementsMetadataPacket::QADPImageEnhancementsMetadataPacket(int8_t brightness, uint8_t sharpness, int8_t contrast, int8_t isInterlacedVideoStream) :
	QADPMetadataPacket(QADPMetadataPacketType::ImageEnhancements),
	m_brightness(brightness),
	m_sharpness(sharpness),
	m_contrast(contrast),
	m_isInterlacedVideoStream(isInterlacedVideoStream)
{
}

QADPImageEnhancementsMetadataPacket::~QADPImageEnhancementsMetadataPacket()
{
}

std::unique_ptr<QADPImageEnhancementsMetadataPacket> QADPImageEnhancementsMetadataPacket::Parse(uint8_t* bytes, decltype(QADPMetadataPacketHeader::length) size)
{
	std::unique_ptr<QADPImageEnhancementsMetadataPacket> packet;

	if (bytes && sizeof(m_brightness) + sizeof(m_sharpness) + sizeof(m_contrast) + sizeof(m_isInterlacedVideoStream) <= size)
	{
		auto brightness = Read<decltype(m_brightness)>(bytes);
		auto sharpness = Read<decltype(m_sharpness)>(bytes);
		auto contrast = Read<decltype(m_contrast)>(bytes);
		auto isInterlacedVideoStream = Read<decltype(m_isInterlacedVideoStream)>(bytes);
		packet = std::move(std::unique_ptr<QADPImageEnhancementsMetadataPacket>(new QADPImageEnhancementsMetadataPacket(brightness, sharpness, contrast, isInterlacedVideoStream)));
	}

	return packet;
}

decltype(QADPImageEnhancementsMetadataPacket::m_brightness) QADPImageEnhancementsMetadataPacket::GetBrightness() const
{
	return m_brightness;
}

decltype(QADPImageEnhancementsMetadataPacket::m_sharpness) QADPImageEnhancementsMetadataPacket::GetSharpness() const
{
	return m_sharpness;
}

decltype(QADPImageEnhancementsMetadataPacket::m_contrast) QADPImageEnhancementsMetadataPacket::GetContrast() const
{
	return m_contrast;
}

bool QADPImageEnhancementsMetadataPacket::IsInterlacedVideoStream() const
{
	return m_isInterlacedVideoStream != 0;
}
