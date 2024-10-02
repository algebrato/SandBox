// Copyright © 2023 March Networks Corporation. All rights reserved.

#include "QADPVideoFrame.h"

#include "QADPFrameCommonHeader.h"

using marchnetworks::media::protocol::qadp::QADPFrame;
using marchnetworks::media::protocol::qadp::QADPFrameVideoDataHeader;
using marchnetworks::media::protocol::qadp::QADPFrameCommonHeader;
using marchnetworks::media::protocol::qadp::QADPVideoFrame;

QADPVideoFrame::QADPVideoFrame(std::unique_ptr<QADPFrameCommonHeader>&& networkByteOrderCommonHeader, 
                               std::unique_ptr<QADPFrameCommonHeader>&& hostByteOrderCommonHeader,
							   std::unique_ptr<QADPFrameVideoDataHeader>&& networkByteOrderDataHeader, 
							   std::unique_ptr<QADPFrameVideoDataHeader>&& hostByteOrderDataHeader,
							   std::unique_ptr<uint8_t[]> metadata, 
							   std::unique_ptr<uint8_t[]> data
							) : QADPFrame(std::move(networkByteOrderCommonHeader), 
							              std::move(hostByteOrderCommonHeader), 
										  std::move(metadata), std::move(data)),
	                            m_networkByteOrderDataHeader(std::move(networkByteOrderDataHeader)),
	                            m_hostByteOrderDataHeader(std::move(hostByteOrderDataHeader))
{
}

QADPVideoFrame::~QADPVideoFrame()
{
}

std::unique_ptr<QADPVideoFrame> QADPVideoFrame::Make(std::unique_ptr<QADPFrameCommonHeader>&& networkByteOrderCommonHeader, 
                                                     std::unique_ptr<QADPFrameCommonHeader>&& hostByteOrderCommonHeader,
													 std::unique_ptr<QADPFrameVideoDataHeader>&& networkByteOrderDataHeader, 
													 std::unique_ptr<QADPFrameVideoDataHeader>&& hostByteOrderDataHeader,
													 std::unique_ptr<uint8_t[]> metadata, 
													 std::unique_ptr<uint8_t[]> data
												)
{
	if (!networkByteOrderCommonHeader || !hostByteOrderCommonHeader)
		throw std::runtime_error("media packet framing header cannot be null");

	if (!networkByteOrderDataHeader && hostByteOrderDataHeader)
		throw std::runtime_error("video data header in network byte order cannot be null when header in host byte order is not");

	if (networkByteOrderDataHeader && !hostByteOrderDataHeader)
		throw std::runtime_error("video data header in host byte order cannot be null when header in network byte order is not");

	if (metadata && hostByteOrderCommonHeader->metadataSize <= 0)
		throw std::runtime_error("unexpected metadata with no size specified");

	if (!metadata && hostByteOrderCommonHeader->metadataSize > 0)
		throw std::runtime_error("unexpected null metadata with size greater than zero");

	if (data && hostByteOrderCommonHeader->dataSize <= 0)
		throw std::runtime_error("unexpected data with no size specified");

	if (!data && hostByteOrderCommonHeader->dataSize > 0)
		throw std::runtime_error("unexpected null data with size greater than zero");

	return std::unique_ptr<QADPVideoFrame>(new QADPVideoFrame(std::move(networkByteOrderCommonHeader), 
												              std::move(hostByteOrderCommonHeader),
															  std::move(networkByteOrderDataHeader), 
															  std::move(hostByteOrderDataHeader),
															  std::move(metadata), 
															  std::move(data)
														)
										);
}

std::string QADPVideoFrame::GetCodecName() const
{
	return m_hostByteOrderDataHeader ? m_hostByteOrderDataHeader->codec : "";
}
