// Copyright © 2023 March Networks Corporation. All rights reserved.

#ifndef __MN_MEDIA_ISTREAM__H__
#define __MN_MEDIA_ISTREAM__H__

namespace marchnetworks
{
	namespace media
	{
		namespace io
		{
			class IStream
			{
			public:
				IStream() {};
				virtual ~IStream() {};

				virtual void Open() = 0;
				virtual void Close() = 0;

				virtual bool IsEndOfStream() const = 0;
				virtual bool IsOpened() const = 0;
			};
		}
	}
}

#endif
