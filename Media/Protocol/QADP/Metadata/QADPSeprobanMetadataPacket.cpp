// Copyright © 2023 March Networks Corporation. All rights reserved.

#include "QADPSeprobanMetadataPacket.h"

#ifndef _WIN32
#include <cstring>
#endif // !_WIN32

using marchnetworks::media::protocol::qadp::QADPMetadataPacket;
using marchnetworks::media::protocol::qadp::QADPMetadataPacketHeader;
using marchnetworks::media::protocol::qadp::QADPMetadataPacketType;
using marchnetworks::media::protocol::qadp::QADPSeprobanMetadataPacket;

QADPSeprobanMetadataPacket::QADPSeprobanMetadataPacket(const char* branchId, const char* eventType, uint8_t numberOfCameras, uint8_t cameraOrdinalNumber, const char* frameTimestamp, char timeFromAlarm, char fileType, const char* imageNumber, const uint8_t* occurrenceId) :
	QADPMetadataPacket(QADPMetadataPacketType::Seproban),
	m_numberOfCameras(numberOfCameras),
	m_cameraOrdinalNumber(cameraOrdinalNumber),
	m_timeFromAlarm(timeFromAlarm),
	m_fileType(fileType)
{
	memcpy(&m_branchId, &branchId, sizeof(m_branchId));
	memcpy(&m_eventType, &eventType, sizeof(m_eventType));
	memcpy(&m_frameTimestamp, &frameTimestamp, sizeof(m_frameTimestamp));
	memcpy(&m_imageNumber, &imageNumber, sizeof(m_imageNumber));
	memcpy(&m_occurrenceId, &occurrenceId, sizeof(m_occurrenceId));
}

QADPSeprobanMetadataPacket::~QADPSeprobanMetadataPacket()
{
}

std::unique_ptr<QADPSeprobanMetadataPacket> QADPSeprobanMetadataPacket::Parse(uint8_t* bytes, decltype(QADPMetadataPacketHeader::length) size)
{
	std::unique_ptr<QADPSeprobanMetadataPacket> packet;

	if (bytes && sizeof(m_branchId) + sizeof(m_eventType) + sizeof(m_numberOfCameras) + sizeof(m_cameraOrdinalNumber) + sizeof(m_frameTimestamp) + sizeof(m_timeFromAlarm) + sizeof(m_fileType) + sizeof(m_imageNumber) + sizeof(m_occurrenceId) <= size)
	{
		auto branchId = Read<std::remove_all_extents<decltype(m_branchId)>::type>(bytes, 0);
		auto eventType = Read<std::remove_all_extents<decltype(m_eventType)>::type>(bytes, sizeof(m_branchId));
		auto numberOfCameras = *Read<decltype(m_numberOfCameras)>(bytes, sizeof(m_eventType));
		auto cameraOrdinalNumber = *Read<decltype(m_cameraOrdinalNumber)>(bytes, sizeof(m_numberOfCameras));
		auto frameTimestamp = Read<std::remove_all_extents<decltype(m_frameTimestamp)>::type>(bytes, sizeof(m_cameraOrdinalNumber));
		auto timeFromAlarm = *Read<decltype(m_timeFromAlarm)>(bytes, sizeof(m_frameTimestamp));
		auto fileType = *Read<decltype(m_fileType)>(bytes, sizeof(m_timeFromAlarm));
		auto imageNumber = Read<std::remove_all_extents<decltype(m_imageNumber)>::type>(bytes, sizeof(m_fileType));
		auto occurrenceId = Read<std::remove_all_extents<decltype(m_occurrenceId)>::type>(bytes, sizeof(m_imageNumber));

		packet = std::move(std::unique_ptr<QADPSeprobanMetadataPacket>(new QADPSeprobanMetadataPacket(branchId, eventType, numberOfCameras, cameraOrdinalNumber, frameTimestamp, timeFromAlarm, fileType, imageNumber, occurrenceId)));
	}

	return packet;
}

std::string QADPSeprobanMetadataPacket::GetBranchID() const
{
	return std::string(m_branchId, sizeof(m_branchId));
}

std::string QADPSeprobanMetadataPacket::GetEventType() const
{
	return std::string(m_eventType, sizeof(m_eventType));
}

decltype(QADPSeprobanMetadataPacket::m_numberOfCameras) QADPSeprobanMetadataPacket::GetNumberOfCameras() const
{
	return m_numberOfCameras;
}

decltype(QADPSeprobanMetadataPacket::m_cameraOrdinalNumber) QADPSeprobanMetadataPacket::GetCameraOrdinalNumber() const
{
	return m_cameraOrdinalNumber;
}

std::string QADPSeprobanMetadataPacket::GetFrameTimestamp() const
{
	return std::string(m_frameTimestamp, sizeof(m_frameTimestamp));
}

bool QADPSeprobanMetadataPacket::IsImageRelatedToAlarm() const
{
	return m_timeFromAlarm != 'N';
}

bool QADPSeprobanMetadataPacket::IsImagePriorToAlarm() const
{
	return m_timeFromAlarm == 'A';
}

bool QADPSeprobanMetadataPacket::IsImageDuringAlarm() const
{
	return m_timeFromAlarm == 'D';
}

decltype(QADPSeprobanMetadataPacket::m_fileType) QADPSeprobanMetadataPacket::GetFileType() const
{
	return m_fileType;
}

std::string QADPSeprobanMetadataPacket::GetImageNumber() const
{
	return std::string(m_imageNumber, sizeof(m_imageNumber));
}

int64_t QADPSeprobanMetadataPacket::GetOccurrenceId() const
{
	return *reinterpret_cast<const int64_t*>(m_occurrenceId);
}
