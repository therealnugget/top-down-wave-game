#pragma once
#include <functional>
#include "usefulTypedefs.hpp"
template<typename T>
class MultiDelegate;
template<typename T>
class MultiDelegate {
private:
	std::vector<void (*)(T)> delegates;
public:
	inline void operator +=(void (*del)(T)) {
		delegates.push_back(del);
	}
	void operator ()(T param);
};
template<>
class MultiDelegate<void>;
template<>
class MultiDelegate<void> {
private:
	std::vector<void (*)()> delegates;
public:
	inline void operator +=(void (*del)()) {
		delegates.push_back(del);
	}
	inline void operator +=(std::function<void(void)> del) {
		*this += del;
	}
	void operator ()();
};