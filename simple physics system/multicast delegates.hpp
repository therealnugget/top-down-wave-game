#pragma once
#include <functional>
#include "usefulTypedefs.hpp"
template<typename T>
class MultiDelegate;
template<typename T>
class MultiDelegate {
private:
	std::vector<std::function<void(T)>> delegates;
public:
	inline void operator +=(std::function<void(T)> del) {
		delegates.push_back(del);
	}
	void operator ()(T param);
};
template<>
class MultiDelegate<void>;
template<>
class MultiDelegate<void> {
private:
	std::vector<std::function<void(void)>> delegates;
public:
	inline void operator +=(std::function<void(void)> del) {
		delegates.push_back(del);
	}
	void operator ()();
};