#include "Crystal.hpp"
void Crystal::Update(void) {
	Behaviour::Update();
	if (!destroy) return;
	delete this;
}