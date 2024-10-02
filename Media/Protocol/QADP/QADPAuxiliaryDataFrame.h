// Copyright © 2023 March Networks Corporation. All rights reserved.

#ifndef __MN_MEDIA_QADPAUXILIARYDATAFRAME__H__
#define __MN_MEDIA_QADPAUXILIARYDATAFRAME__H__

#include "QADPFrame.h"
#include "QADPFrameAuxiliaryDataHeader.h"

#include <string>

namespace marchnetworks
{
	namespace media
	{
		namespace protocol
		{
			namespace qadp
			{
				struct QADPFrameCommonHeader;

				class  QADPAuxiliaryDataFrame : public QADPFrame
				{
				private:
					QADPAuxiliaryDataFrame(std::unique_ptr<QADPFrameCommonHeader>&& networkByteOrderCommonHeader, std::unique_ptr<QADPFrameCommonHeader>&& hostByteOrderCommonHeader,
						std::unique_ptr<QADPFrameAuxiliaryDataHeader>&& networkByteOrderDataHeader, std::unique_ptr<QADPFrameAuxiliaryDataHeader>&& hostByteOrderDataHeader,
						std::unique_ptr<uint8_t[]> metadata, std::unique_ptr<uint8_t[]> data);
				public:
					virtual ~QADPAuxiliaryDataFrame();

					static std::unique_ptr<QADPAuxiliaryDataFrame> Make(std::unique_ptr<QADPFrameCommonHeader>&& networkByteOrderCommonHeader, std::unique_ptr<QADPFrameCommonHeader>&& hostByteOrderCommonHeader,
						std::unique_ptr<QADPFrameAuxiliaryDataHeader>&& networkByteOrderDataHeader, std::unique_ptr<QADPFrameAuxiliaryDataHeader>&& hostByteOrderDataHeader,
						std::unique_ptr<uint8_t[]> metadata, std::unique_ptr<uint8_t[]> data);

					std::string GetCodecName() const;

				private:
					std::unique_ptr<QADPFrameAuxiliaryDataHeader> m_networkByteOrderDataHeader;
					std::unique_ptr<QADPFrameAuxiliaryDataHeader> m_hostByteOrderDataHeader;
				};
			}
		}
	}
}

#endif
