#pragma once
//static
template<typename T>
class Array {
public:
	static inline void CopyTo(T* ptr, T arr[], rsize_t size) {
		memcpy(arr, ptr, size);
	}
};