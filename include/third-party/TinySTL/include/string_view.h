#ifndef _ZXY_STRING_VIEW_H
#define _ZXY_STRING_VIEW_H

#include <stdexcept>
#include <string>
#include <string.h>
#include "utility.h"

namespace TinySTL { 

class StringArg
{
public:
	StringArg(char const* str)
		: data_(str)
	{ }
	
	StringArg(std::string const& str)
		: data_(str.c_str())
	{ }

	operator char const* () const
	{ return data_; }

private:
	char const* data_;
};

// only support std::string
// because I just use it
//
// string_view just view which is read only
// so the life time of string depend on user
//
class string_view
{
public:	
	using size_type = unsigned;
	using value_type = char;
	using pointer = char*;
	using const_pointer = char const*;
	using reference = char&;
	using const_reference = char const&;
	using iterator = pointer;
	using const_iterator = const_pointer;
	
	// constructor
	string_view() = default;

	string_view(char const* str)
		: data_(str), len_(strlen(str))
	{ }
	
	constexpr string_view(char const* str, size_type len)
		: data_(str), len_(len) 
	{ }

	string_view(std::string const& str)
		: data_(str.c_str()), len_(str.size())
	{ }
	
	string_view(unsigned char const* str)
		: data_(reinterpret_cast<char const*>(str)), len_(strlen(data_))
	{ }

	string_view(unsigned char const* str, size_type len)
		: data_(reinterpret_cast<char const*>(str)), len_(len)
	{ }
	
	string_view(char const& c)
		: string_view(&c, 1)
	{ }
	
	template<uint64_t N>
	string_view(char const(&str)[N])
		: string_view(str, N)
	{ }

	// capacity
	constexpr bool empty() const noexcept
	{ return len_ == 0; }

	constexpr size_type size() const noexcept
	{ return len_; }
	
	// position
	constexpr char const* begin() const noexcept
	{ return data_; }

	constexpr char const* end() const noexcept
	{ return data_ + len_; }
	
	 // data access
	constexpr const_reference operator[](size_type n) const noexcept
	{ return data_[n]; }

	const_reference at(size_type n) const 
	{ 
		if(n >= len_) 
			throw std::length_error{"Given index over or equal length of string_view"};
		return data_[n];
	}
	
	constexpr const_reference front() const noexcept
	{ return data_[0]; }

	constexpr const_reference back() const noexcept
	{ return data_[len_ - 1]; }
	
	constexpr const_pointer data() const noexcept
	{ return data_; }
	
	void swap(string_view& rhs) noexcept
	{
		STL_SWAP(data_, rhs.data_);
		STL_SWAP(len_, rhs.len_);
	}
	
	// modify operation
	void remove_prefix(size_type n) noexcept
	{
		data_ += n;
		len_ -= n;
	}

	void remove_suffix(size_type n) noexcept
	{ len_ -= n; }

	size_type copy(char* dst, 
			size_type count, 
			size_type pos = 0) const
	{
		if(pos > len_)
			throw std::out_of_range{"Give position over length of string_view"};
		memcpy(dst, data_ + pos, count);
		return count;
	}

	string_view substr(size_type pos = 0,
			size_type count = npos) const noexcept
	{
		auto len = std::min(count, len_ - pos);
		return string_view(data_ + pos, len);
	}
	
	size_type find(string_view v, size_type pos = 0) const noexcept
	{
		// FIXME intead of kmp or other efficient algorithm
		char const* begin;
		if(v.size() > len_ || 
				!(begin = strstr(data_ + pos, v.data())))
			return npos;
		else
			return begin - data_;
	}
	
	size_type rfind(string_view v, size_type pos = npos) const noexcept
	{
		int len = std::min(len_ - 1, pos);	
		for(;len >= 0 ; --len){
			if(data_[len] == v.front() && len_ - len >= v.size() &&
					memcmp(data_ + len, v.data(), v.size()) == 0)
				return len;
		}

		return npos;
	}


	bool contains(string_view v) const noexcept
	{ return find(v) != npos; }

	bool starts_with(string_view v) const noexcept
	{
		if(len_ >= v.size() && memcmp(data_, v.data(), v.size()) == 0)
			return true;
		else
			return false;
	}
	
	bool ends_with(string_view v) const noexcept
	{
		if(len_ >= v.size() && memcmp(data_ + len_ - v.size(), v.data(), v.size()) == 0)
			return true;
		else 
			return false;
	}
	
	/**
	 * @brief finds the first character equal to one of characters in the given character sequence
	 * @return the index of the first occurrence of any character of the sequence, if not found, return npos
	 */
	size_type find_first_of(string_view v, size_type pos = 0) const noexcept
	{ 
		for(; pos < len_; ++pos){
			if(charInRange(data_[pos], v))
				return pos;
		}

		return npos;
	}
	
	/**
	 * @brief finds the last character equal to one of characters in the given character sequence(i.e. character range)
	 * @param pos the end position of the search range
	 * @return the index of the last occurrence of any character of the sequence, if not found, return npos
	 */
	size_type find_last_of(string_view v, size_type pos = npos) const noexcept
	{ 
		int i = static_cast<int>(std::min(len_ - 1, pos));

		for(; i >= 0; --i){
			if(charInRange(data_[i], v))
				return i;
		}

		return npos;
	}

	size_type find_first_not_of(string_view v, size_type pos = 0)
	{
		for(; pos < len_; ++pos){
			if(!charInRange(data_[pos], v))
				return pos;
		}
		return npos;
	}
	
	size_type find_last_not_of(string_view v, size_type pos = npos)
	{
		int i = static_cast<int>(std::min(len_ - 1, pos));

		for(; i >= 0; --i){
			if(!charInRange(data_[i], v))
				return i;
		}

		return npos;
	}

	// lexicographic compare
	int compare(string_view v) const noexcept
	{
		int r = memcmp(data_, v.data(), len_ < v.size() ?
				len_ : v.size());

		if(r == 0){
			if(len_ < v.size()) r = -1;
			else r = 1;
		}

		return r;
	}

	
private:
	// helper
	// complexity O(sv.size())
	bool charInRange(char c, string_view const& sv) const noexcept
	{
		for(auto x : sv)
			if(c == x) return true;
	
		return false;
	}

private:
	char const* data_;
	size_type len_;

	// static
	// end indicator or error indicator
public:
	static constexpr size_type npos = -1;
};

inline void swap(string_view& lhs, string_view& rhs) noexcept(noexcept(lhs.swap(rhs)))
{ lhs.swap(rhs); }

inline bool operator==(string_view const& lhs, string_view const& rhs) noexcept
{ return (lhs.size() == rhs.size()) && memcmp(lhs.data(), rhs.data(), lhs.size()) == 0; }

inline bool operator!=(string_view const& lhs, string_view const& rhs) noexcept
{ return !(lhs == rhs); }

inline bool operator<(string_view const& lhs, string_view const& rhs) noexcept
{ return lhs.compare(rhs) < 0; }

inline bool operator>(string_view const& lhs, string_view const& rhs) noexcept
{ return rhs < lhs; }

inline bool operator>=(string_view const& lhs, string_view const& rhs) noexcept
{ return !(lhs < rhs); }

inline bool operator<=(string_view const& lhs, string_view const& rhs) noexcept
{ return !(lhs > rhs); }

template<typename Ostream>
Ostream& operator<<(Ostream& os, string_view const& v) noexcept
{ return os << v.data(); }

namespace literal{
	constexpr string_view operator""_sv(char const* str, std::size_t len) noexcept
	{ return string_view(str, len); }

} // namespace literal

} // namespace TinySTL

#endif // _ZXY_STRING_VIEW_H
