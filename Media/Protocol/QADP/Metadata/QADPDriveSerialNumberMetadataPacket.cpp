// Copyright © 2023 March Networks Corporation. All rights reserved.

#include "QADPDriveSerialNumberMetadataPacket.h"

#ifndef _WIN32
#include <cstring>
#endif // !_WIN32

using marchnetworks::media::protocol::qadp::QADPDriveSerialNumberMetadataPacket;
using marchnetworks::media::protocol::qadp::QADPMetadataPacket;
using marchnetworks::media::protocol::qadp::QADPMetadataPacketHeader;
using marchnetworks::media::protocol::qadp::QADPMetadataPacketType;

QADPDriveSerialNumberMetadataPacket::QADPDriveSerialNumberMetadataPacket(std::unique_ptr<int8_t[]>&& serialNumber, std::size_t serialNumberLength) :
	QADPMetadataPacket(QADPMetadataPacketType::DriveSerialNumber),
	m_serialNumber(std::move(serialNumber)),
	m_serialNumberLength(serialNumberLength)
{
}

QADPDriveSerialNumberMetadataPacket::~QADPDriveSerialNumberMetadataPacket()
{
}

std::unique_ptr<QADPDriveSerialNumberMetadataPacket> QADPDriveSerialNumberMetadataPacket::Parse(uint8_t* bytes, decltype(QADPMetadataPacketHeader::length) size)
{
	std::unique_ptr<QADPDriveSerialNumberMetadataPacket> packet;

	auto serialNumberLength = size > 0 ? strnlen(reinterpret_cast<char*>(bytes), size) : 0;

	decltype(m_serialNumber) serialNumber;
	if (serialNumberLength > 0)
	{
		serialNumber = std::make_unique<decltype(m_serialNumber)::element_type[]>(serialNumberLength);
		memcpy(serialNumber.get(), bytes, serialNumberLength);
	}

	packet = std::move(std::unique_ptr<QADPDriveSerialNumberMetadataPacket>(new QADPDriveSerialNumberMetadataPacket(std::move(serialNumber), serialNumberLength)));

	return packet;
}

std::string QADPDriveSerialNumberMetadataPacket::GetSerialNumber() const
{
	return m_serialNumberLength > 0 ? std::string(reinterpret_cast<char*>(m_serialNumber.get()), m_serialNumberLength) : "";
}
