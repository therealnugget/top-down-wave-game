#include "main.hpp"
#include "multicast delegates.hpp"
void MultiDelegate<void>::operator ()(void) {
	auto head = delegates;
	while (head) {
		head->value();
		Node<std::function<void(void)>>::Advance(&head);
	}
}