#include "physics.hpp"
#include "linkedList.hpp"
void Main::AssignIfMore(Vector2<float>& check, Vector2<float> &assignConditionally) {
	AssignIfMore(check.x, assignConditionally.x);
	AssignIfMore(check.y, assignConditionally.y);
}
void Main::AssignIfLess(Vector2<float>& check, Vector2<float> &assignConditionally) {
	AssignIfLess(check.x, assignConditionally.x);
	AssignIfLess(check.y, assignConditionally.y);
}