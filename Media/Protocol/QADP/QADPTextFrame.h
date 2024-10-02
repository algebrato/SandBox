// Copyright © 2023 March Networks Corporation. All rights reserved.

#ifndef __MN_MEDIA_QADPTEXTFRAME__H__
#define __MN_MEDIA_QADPTEXTFRAME__H__

#include "QADPFrame.h"
#include "QADPFrameTextDataHeader.h"

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

				class  QADPTextFrame : public QADPFrame
				{
				private:
					QADPTextFrame(std::unique_ptr<QADPFrameCommonHeader>&& networkByteOrderCommonHeader, std::unique_ptr<QADPFrameCommonHeader>&& hostByteOrderCommonHeader,
						std::unique_ptr<QADPFrameTextDataHeader>&& networkByteOrderDataHeader, std::unique_ptr<QADPFrameTextDataHeader>&& hostByteOrderDataHeader,
						std::unique_ptr<char[]> textSourceId, std::unique_ptr<uint8_t[]> data);
				public:
					virtual ~QADPTextFrame();

					static std::unique_ptr<QADPTextFrame> Make(std::unique_ptr<QADPFrameCommonHeader>&& networkByteOrderCommonHeader, std::unique_ptr<QADPFrameCommonHeader>&& hostByteOrderCommonHeader,
						std::unique_ptr<QADPFrameTextDataHeader>&& networkByteOrderDataHeader, std::unique_ptr<QADPFrameTextDataHeader>&& hostByteOrderDataHeader,
						std::unique_ptr<char[]> textSourceId, std::unique_ptr<uint8_t[]> data);

					std::string GetEncoding() const;
					std::string GetPrefix() const;
					std::string GetSourceId() const;
					std::string GetText() const;
					std::string GetTitle() const;

				private:
					std::unique_ptr<QADPFrameTextDataHeader> m_networkByteOrderDataHeader;
					std::unique_ptr<QADPFrameTextDataHeader> m_hostByteOrderDataHeader;

					std::unique_ptr<char[]> m_textSourceId;
				};
			}
		}
	}
}

#endif
