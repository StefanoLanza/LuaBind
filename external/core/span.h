#pragma once

namespace Typhoon {

// Temporary replacement for std::span in C++ 20
template <class T>
class span {
public:
	span(T* data, size_t size)
	    : data_(data)
	    , size_(size) {
	}
	T* data() const {
		return data_;
	}
	size_t size() const {
		return size_;
	}
	T& operator[](size_t i) noexcept {
		return data_[i];
	}
	T const& operator[](size_t i) const noexcept {
		return data_[i];
	}
	T* begin() const noexcept {
		return data_;
	}
	T* end() const noexcept {
		return data_ + size_;
	}

	bool empty() const {
		return size_ == 0;
	}

public:
	T*     data_;
	size_t size_;
};

} // namespace Typhoon
