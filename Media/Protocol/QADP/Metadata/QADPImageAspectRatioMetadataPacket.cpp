// Copyright © 2023 March Networks Corporation. All rights reserved.

#include "QADPImageAspectRatioMetadataPacket.h"

using marchnetworks::media::protocol::qadp::QADPImageAspectRatioMetadataPacket;
using marchnetworks::media::protocol::qadp::QADPMetadataPacket;
using marchnetworks::media::protocol::qadp::QADPMetadataPacketHeader;
using marchnetworks::media::protocol::qadp::QADPMetadataPacketType;

QADPImageAspectRatioMetadataPacket::QADPImageAspectRatioMetadataPacket(uint16_t aspectWidth, uint16_t aspectHeight) :
	QADPMetadataPacket(QADPMetadataPacketType::ImageAspectRatio),
	m_aspectWidth(aspectWidth),
	m_aspectHeight(aspectHeight)
{
}

QADPImageAspectRatioMetadataPacket::~QADPImageAspectRatioMetadataPacket()
{
}

std::unique_ptr<QADPImageAspectRatioMetadataPacket> QADPImageAspectRatioMetadataPacket::Parse(uint8_t* bytes, decltype(QADPMetadataPacketHeader::length) size)
{
	std::unique_ptr<QADPImageAspectRatioMetadataPacket> packet;

	if (bytes && sizeof(m_aspectWidth) + sizeof(m_aspectHeight) <= size)
	{
		auto aspectWidth = Read<decltype(m_aspectWidth)>(bytes);
		auto aspectHeight = Read<decltype(m_aspectHeight)>(bytes);
		packet = std::move(std::unique_ptr<QADPImageAspectRatioMetadataPacket>(new QADPImageAspectRatioMetadataPacket(aspectWidth, aspectHeight)));
	}

	return packet;
}

decltype(QADPImageAspectRatioMetadataPacket::m_aspectWidth) QADPImageAspectRatioMetadataPacket::GetImageAspectWidth() const
{
	return m_aspectWidth;
}

decltype(QADPImageAspectRatioMetadataPacket::m_aspectHeight) QADPImageAspectRatioMetadataPacket::GetImageAspectHeight() const
{
	return m_aspectHeight;
}
