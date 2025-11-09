#pragma once
#include <functional>
#include <tuple>
#include "usefulTypedefs.hpp"
#include "main.hpp"
template<typename T>
class MultiDelegate;
template<typename T>
class MultiDelegate {
private:
	Node<std::function<void(T)>>* delegates = nullptr;
public:
	inline Node<std::function<void(T)>> * operator +=(std::function<void(T)> del) {
		return Node<std::function<void(T)>>::AddAtHead(del, &delegates);
	}
	void operator ()(T param);
};
template<typename T>
void MultiDelegate<T>::operator ()(T param) {
	auto head = delegates;
	while (head) {
		head->value(param);
		Node<std::function<void(T)>>::Advance(&head);
	}
}
template<>
class MultiDelegate<void>;
template<>
class MultiDelegate<void> {
private:
	Node<std::function<void(void)>> *delegates = nullptr;
public:
	inline Node<std::function<void(void)>>* operator +=(std::function<void(void)> del) {
		return Node<std::function<void(void)>>::AddAtHead(del, &delegates);
	}
	inline void operator -=(Node<std::function<void(void)>>* node) {
		Node<std::function<void(void)>>::Remove(&delegates, node);
	}
	void operator ()();
};