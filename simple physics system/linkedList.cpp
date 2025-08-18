#include "linkedList.hpp"
#include "physics.hpp"
void Main::AssignIfMore(FVector2& check, FVector2 &assignConditionally) {
	AssignIfMore(check.x, assignConditionally.x);
	AssignIfMore(check.y, assignConditionally.y);
}
void Main::AssignIfLess(FVector2& check, FVector2 &assignConditionally) {
	AssignIfLess(check.x, assignConditionally.x);
	AssignIfLess(check.y, assignConditionally.y);
}