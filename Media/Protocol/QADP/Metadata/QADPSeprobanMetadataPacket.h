// Copyright © 2023 March Networks Corporation. All rights reserved.

#ifndef __MN_MEDIA_QADPSEPROBANMETADATAPACKET__H__
#define __MN_MEDIA_QADPSEPROBANMETADATAPACKET__H__

#include "QADPMetadataPacket.h"
#include "QADPMetadataPacketHeader.h"

#include <memory>
#include <string>

namespace marchnetworks
{
	namespace media
	{
		namespace protocol
		{
			namespace qadp
			{
				class  QADPSeprobanMetadataPacket : public QADPMetadataPacket
				{
				private:
					QADPSeprobanMetadataPacket(const char* branchId, const char* eventType, uint8_t numberOfCameras, uint8_t cameraOrdinalNumber, const char* frameTimestamp, char timeFromAlarm, char fileType, const char* imageNumber, const uint8_t* occurrenceId);

				public:
					virtual ~QADPSeprobanMetadataPacket();

					static std::unique_ptr<QADPSeprobanMetadataPacket> Parse(uint8_t* bytes, decltype(QADPMetadataPacketHeader::length) size);

				private:
					char m_branchId[5];
					char m_eventType[3];
					uint8_t m_numberOfCameras;
					uint8_t m_cameraOrdinalNumber;
					char m_frameTimestamp[6];
					char m_timeFromAlarm;
					char m_fileType;
					char m_imageNumber[6];
					uint8_t m_occurrenceId[8];

				public:
					std::string GetBranchID() const;
					std::string GetEventType() const;
					decltype(m_numberOfCameras) GetNumberOfCameras() const;
					decltype(m_cameraOrdinalNumber) GetCameraOrdinalNumber() const;
					std::string GetFrameTimestamp() const;
					bool IsImageRelatedToAlarm() const;
					bool IsImagePriorToAlarm() const;
					bool IsImageDuringAlarm() const;
					decltype(m_fileType) GetFileType() const;
					std::string GetImageNumber() const;
					int64_t GetOccurrenceId() const;
				};
			}
		}
	}
}

#endif
