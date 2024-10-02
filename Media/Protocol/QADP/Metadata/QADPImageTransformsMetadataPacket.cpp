// Copyright © 2023 March Networks Corporation. All rights reserved.

#include "QADPImageTransformsMetadataPacket.h"

using marchnetworks::media::protocol::qadp::QADPImageTransformsMetadataPacket;
using marchnetworks::media::protocol::qadp::QADPMetadataPacket;
using marchnetworks::media::protocol::qadp::QADPMetadataPacketHeader;
using marchnetworks::media::protocol::qadp::QADPMetadataPacketType;

QADPImageTransformsMetadataPacket::QADPImageTransformsMetadataPacket(uint16_t flipBitField, uint16_t rotationDegrees) :
	QADPMetadataPacket(QADPMetadataPacketType::ImageTransforms),
	m_flipBitField(flipBitField),
	m_rotationDegrees(rotationDegrees)
{
}

QADPImageTransformsMetadataPacket::~QADPImageTransformsMetadataPacket()
{
}

std::unique_ptr<QADPImageTransformsMetadataPacket> QADPImageTransformsMetadataPacket::Parse(uint8_t* bytes, decltype(QADPMetadataPacketHeader::length) size)
{
	std::unique_ptr<QADPImageTransformsMetadataPacket> packet;

	if (bytes && sizeof(m_flipBitField) + sizeof(m_rotationDegrees) <= size)
	{
		auto flipBitField = Read<decltype(m_flipBitField)>(bytes);
		auto rotationDegrees = Read<decltype(m_rotationDegrees)>(bytes);
		if (rotationDegrees >= -360 && rotationDegrees <= 360)
			rotationDegrees = rotationDegrees % 360;

		packet = std::move(std::unique_ptr<QADPImageTransformsMetadataPacket>(new QADPImageTransformsMetadataPacket(flipBitField, rotationDegrees)));
	}

	return packet;
}

decltype(QADPImageTransformsMetadataPacket::m_rotationDegrees) QADPImageTransformsMetadataPacket::GetRotationDegrees() const
{
	return m_rotationDegrees;
}

bool QADPImageTransformsMetadataPacket::HasHorizontalFlipToBeApplied() const
{
	return (m_flipBitField & 0x0001) != 0;
}

bool QADPImageTransformsMetadataPacket::HasVerticalFlipToBeApplied() const
{
	return (m_flipBitField & 0x0002) != 0;
}
