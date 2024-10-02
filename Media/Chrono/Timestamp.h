// Copyright © 2023 March Networks Corporation. All rights reserved.

#ifndef __MN_MEDIA_TIMESTAMP__H__
#define __MN_MEDIA_TIMESTAMP__H__

#include <chrono>

namespace marchnetworks
{
	namespace media
	{
		namespace chrono
		{
			class  Timestamp
			{
			public:
				Timestamp();

				template <typename T>
				Timestamp(T value);

				virtual ~Timestamp();

				static Timestamp Max() noexcept;
				static Timestamp Min() noexcept;

				bool operator==(const Timestamp& other) const;
				bool operator<(const Timestamp& other) const;
				bool operator<=(const Timestamp& other) const;
				bool operator>(const Timestamp& other) const;
				bool operator>=(const Timestamp& other) const;

				Timestamp operator+(const Timestamp& other) const;
				Timestamp operator-(const Timestamp& other) const;

				template <typename T>
				T Value() const;

			private:
				std::chrono::nanoseconds m_timestamp;
			};

			template <>
			Timestamp::Timestamp(std::chrono::nanoseconds value);

			template <typename T>
			Timestamp::Timestamp(T value) :
				m_timestamp(std::chrono::duration_cast<std::chrono::nanoseconds>(value))
			{
			}

			template <>
			std::chrono::nanoseconds Timestamp::Value() const;

			template <typename T>
			T Timestamp::Value() const
			{
				return std::chrono::duration_cast<T>(m_timestamp);
			}
		}
	}
}

#endif
