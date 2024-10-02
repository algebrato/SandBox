// Copyright © 2023 March Networks Corporation. All rights reserved.

#include "QADPMetadataPacket.h"

using marchnetworks::media::protocol::qadp::QADPMetadataPacket;
using marchnetworks::media::protocol::qadp::QADPMetadataPacketType;

QADPMetadataPacket::QADPMetadataPacket(QADPMetadataPacketType qadpMetadataPacketType) :
	m_packetType(qadpMetadataPacketType)
{
}

QADPMetadataPacket::~QADPMetadataPacket()
{
}

QADPMetadataPacketType QADPMetadataPacket::GetPacketType() const
{
	return m_packetType;
}
