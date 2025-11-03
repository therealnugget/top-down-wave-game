#include "main.hpp"
void MultiDelegate<void>::operator ()(void) {
	for (auto& del : delegates) {
		del();
	}
}