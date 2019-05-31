
#pragma once

#include <cstddef>
#include <type_traits>

namespace reborn
{
	class mapper_internal
	{
	public:
		template <class T, class PTR>
		static bool read(const PTR addr, T& out)
		{
			out = *reinterpret_cast<const T*>(addr);
			return true;
		}
		template <class T, class PTR>
		static bool write(const PTR addr, const T& in)
		{
			*reinterpret_cast<T*>(addr) = in;
			return true;
		}
		template <class RET, class T, class... ARGS, class PTR>
		static RET call(PTR addr, RET(T::*member)(ARGS...), ARGS... args)
		{
			return (reinterpret_cast<T*>(addr)->*member)(std::forward<ARGS>(args)...);
		}
	};
	template <class INTERNAL_PTR, class EXTERNAL_PTR, class MAPPER = mapper_internal>
	class offset_ptr
	{
	public:
		using ptr_t = INTERNAL_PTR;
		using ex_ptr_t = EXTERNAL_PTR;
		using mapper_t = MAPPER;
		using offset_ptr_t = offset_ptr<ptr_t, ex_ptr_t, mapper_t>;

		constexpr offset_ptr()
			: addr_{0}
		{}
		template <class T>
		constexpr offset_ptr(T addr)
			: addr_{ptr_t(addr)}
		{}
		template <class T>
		constexpr offset_ptr(const T* addr)
			: addr_{reinterpret_cast<const ptr_t>(addr)}
		{}
		constexpr offset_ptr(const std::nullptr_t addr)
			: addr_{0}
		{}
		constexpr ptr_t addr()
		{
			return addr_;
		}
		constexpr ptr_t addr() const
		{
			return addr_;
		}
		template <class T>
		operator T*()
		{
			return reinterpret_cast<T*>(addr());
		}
		template <class T>
		operator const T*() const
		{
			return reinterpret_cast<T*>(addr());
		}
		constexpr offset_ptr_t operator+(offset_ptr_t other)
		{
			return addr_ + other.addr();
		}
		constexpr const offset_ptr_t operator+(offset_ptr_t other) const
		{
			return addr_ + other.addr();
		}
		constexpr offset_ptr_t operator-(offset_ptr_t other)
		{
			return addr_ - other.addr();
		}
		constexpr const offset_ptr_t operator-(offset_ptr_t other) const
		{
			return addr_ - other.addr();
		}
		constexpr offset_ptr_t operator*(offset_ptr_t other)
		{
			return addr_ * other.addr();
		}
		constexpr const offset_ptr_t operator*(offset_ptr_t other) const
		{
			return addr_ * other.addr();
		}
		constexpr offset_ptr_t operator/(offset_ptr_t other)
		{
			return addr_ / other.addr();
		}
		constexpr const offset_ptr_t operator/(offset_ptr_t other) const
		{
			return addr_ / other.addr();
		}
		constexpr offset_ptr_t& operator+=(offset_ptr_t other)
		{
			addr_ += other.addr();
			return *this;
		}
		constexpr const offset_ptr_t& operator+=(offset_ptr_t other) const
		{
			addr_ += other.addr();
			return *this;
		}
		constexpr offset_ptr_t& operator-=(offset_ptr_t other)
		{
			addr_ -= other.addr();
			return *this;
		}
		constexpr const offset_ptr_t& operator-=(offset_ptr_t other) const
		{
			addr_ -= other.addr();
			return *this;
		}
		constexpr offset_ptr_t& operator*=(offset_ptr_t other)
		{
			addr_ *= other.addr();
			return *this;
		}
		constexpr const offset_ptr_t& operator*=(offset_ptr_t other) const
		{
			addr_ *= other.addr();
			return *this;
		}
		constexpr offset_ptr_t& operator/=(offset_ptr_t other)
		{
			addr_ /= other.addr();
			return *this;
		}
		constexpr const offset_ptr_t& operator/=(offset_ptr_t other) const
		{
			addr_ /= other.addr();
			return *this;
		}
		constexpr bool operator==(offset_ptr_t other)
		{
			return addr_ == other.addr();
		}
		constexpr bool operator==(offset_ptr_t other) const
		{
			return addr_ == other.addr();
		}
		constexpr bool operator!=(offset_ptr_t other)
		{
			return !(*(this) == other);
		}
		constexpr bool operator!=(offset_ptr_t other) const
		{
			return !(*(this) == other);
		}
		constexpr bool operator<=(offset_ptr_t other)
		{
			return addr_ <= other.addr();
		}
		constexpr bool operator<=(offset_ptr_t other) const
		{
			return addr_ <= other.addr();
		}
		constexpr bool operator>=(offset_ptr_t other)
		{
			return addr_ >= other.addr();
		}
		constexpr bool operator>=(offset_ptr_t other) const
		{
			return addr_ >= other.addr();
		}
		constexpr bool operator<(offset_ptr_t other)
		{
			return !((*this) >= other);
		}
		constexpr bool operator<(offset_ptr_t other) const
		{
			return !((*this) >= other);
		}
		constexpr bool operator>(offset_ptr_t other)
		{
			return !((*this) <= other);
		}
		constexpr bool operator>( offset_ptr_t other) const
		{
			return !((*this) <= other);
		}
		constexpr offset_ptr_t operator*()
		{
			ex_ptr_t out;
			if (!mapper_t::read(addr(), out))
			{
				throw std::runtime_error("mapper_t::read failed!");
			}
			return out;
		}
		constexpr const offset_ptr_t operator*() const
		{
			ex_ptr_t out;
			if (!mapper_t::read(addr(), out))
			{
				throw std::runtime_error("mapper_t::read failed!");
			}
			return out;
		}
		template <class T>
		offset_ptr operator<<=(const T& value)
		{
			if (!mapper_t::write(addr(), value))
			{
				throw std::runtime_error("mapper_t::write failed!");
			}
			return *this;
		}
		template <class T>
		const offset_ptr& operator>>(T& buffer) const
		{
			if (!mapper_t::read(addr(), buffer))
			{
				throw std::runtime_error("mapper_t::read failed!");
			}
			return *this;
		}
		template <class T>
		T v() const
		{
			T val;
			*this >> val;
			return val;
		}
		template <class T, ex_ptr_t OFFSET>
		class variable
		{
		public:
			template <class T>
			variable(const T& ptr)
				: ptr_{ptr}
			{}
			template <class T>
			class variable_proxy
			{
			public:
				variable_proxy(const T& obj)
					: obj_{obj}
				{}
				T* operator->()
				{
					return &obj_;
				}
			private:
				T obj_;
			};
			auto operator->()
			{
				return variable_proxy{T{ptr_ + OFFSET}};
			}
			template <class T>
			auto& operator<<=(const T& other)
			{
				(ptr_ + OFFSET) <<= other;
				return *this;
			}
			template <class TT>
			auto& operator>>(TT& other)
			{
				(ptr_ + OFFSET) >> other;
				return *this;
			}
			T operator*() const
			{
				return (ptr_ + OFFSET).v<T>();
			}
			T* ptr()
			{
				return reinterpret_cast<T*>(ptr_.addr());
			}
			const T* ptr() const
			{
				return reinterpret_cast<const T*>(ptr_.addr());
			}
		private:
			offset_ptr_t ptr_;
			friend class variable<T*, OFFSET>;
		};
		template <class T, ex_ptr_t OFFSET>
		class variable<T*, OFFSET>
			: public variable<T, OFFSET>
		{
		public:
			auto operator->()
			{
				return variable<T, OFFSET>::template variable_proxy<T>{T{*(this->ptr_ + OFFSET)}};
			}
		};
		template <class T, ex_ptr_t OFFSET, std::size_t S>
		class variable_arr
		{
		public:
			template <class T>
			variable_arr(const T& ptr)
				: ptr_{ptr}
			{}
			auto operator[](std::size_t i) const
			{
				return variable<T, 0x00>{offset_ptr_t{ptr_ + OFFSET + i * S}};
			}
		private:
			offset_ptr_t ptr_;
		};
		template <ex_ptr_t OFFSET>
		class variable_cstr
			: public variable_arr<char, OFFSET, 0x01>
		{
		public:
			std::string str(std::size_t size) const
			{
				std::string result(size, ' ');
				for (std::size_t i = 0; i < size; i++)
				{
					result[i] = *((*this)[i]);
				}
				return result;
			}
			std::string operator*() const
			{
				std::string result;
				std::size_t i = 0;
				char c;
				while ((c = *((*this)[i])) != 0)
				{
					result += c;
					i++;
				}
				return result;
			}

		private:
		};

	private:
		ptr_t addr_;
	};
}
template <class T, class PTR, class EX_PTR, class MAPPER>
constexpr bool operator==(const T& lhs, const reborn::offset_ptr<PTR, EX_PTR, MAPPER> & rhs)
{
	return rhs == lhs;
}
template <class T, class PTR, class EX_PTR, class MAPPER>
constexpr bool operator!=(const T& lhs, const reborn::offset_ptr<PTR, EX_PTR, MAPPER>& rhs)
{
	return rhs != lhs;
}
template <class T, class PTR, class EX_PTR, class MAPPER>
constexpr bool operator<(const T& lhs, const reborn::offset_ptr<PTR, EX_PTR, MAPPER>& rhs)
{
	return rhs >= lhs;
}
template <class T, class PTR, class EX_PTR, class MAPPER>
constexpr bool operator>(const T& lhs, const reborn::offset_ptr<PTR, EX_PTR, MAPPER>& rhs)
{
	return rhs <= lhs;
}
template <class T, class PTR, class EX_PTR, class MAPPER>
constexpr bool operator<=(const T& lhs, const reborn::offset_ptr<PTR, EX_PTR, MAPPER>& rhs)
{
	return rhs > lhs;
}
template <class T, class PTR, class EX_PTR, class MAPPER>
constexpr bool operator>=(const T& lhs, const reborn::offset_ptr<PTR, EX_PTR, MAPPER>& rhs)
{
	return rhs < lhs;
}
