#include "main.hpp"
#include "multicast delegates.hpp"
void MultiDelegate<void>::operator ()(void) {
	curNode = delegates;
	while (curNode) {
		curNode->value();
		if (removedOnUpdate) {
			removedOnUpdate = false;
			continue;
		}
		Node<std::function<void(void)>>::Advance(&curNode);
	}
}