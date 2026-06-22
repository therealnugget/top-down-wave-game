#include "Environment.hpp"
#include "multicast delegates.hpp"
#include "camera.hpp"
Node<std::function<void(void)>>* Environment::lateUpdateNode = nullptr;
Node<Entity*>* Environment::treeEntNodes[Environment::numTrees];
FVector2 Environment::lastInpVec = FVector2::Zero;
IntVec2 Environment::RandTreePos(void) {
	return Main::GetRandIVec(Camera::GetCamExtentWorld(-1.f), Camera::GetCamExtentWorld(1.f)) + GetRandExtentInput();
}
IntVec2 Environment::GetRandExtentInput() {
	return Main::halfDisplaySizeI * lastInpVec * 2;
}
void Environment::Init(void) {
	lateUpdateNode = (Main::LateUpdates += []() {LateUpdate(); });
	for (auto &tree: treeEntNodes) {
		tree = Physics::SubStandaloneEnt(Entity::MakeEntity("environment", { "green tree" }, FVector2::Zero, treeSize, true, RandTreePos()));
	}
}
void Environment::LateUpdate(void) {
	auto randExtentInpPos = GetRandExtentInput();
	auto isPosX = lastInpVec.x > .0f;
	auto isPosY = lastInpVec.y > .0f;
	auto addNeg = IntVec2(randExtentInpPos.x * !isPosX, randExtentInpPos.y * !isPosY);
	auto addPos = IntVec2(randExtentInpPos.x * isPosX, randExtentInpPos.y * isPosY);
	for (auto& tree : treeEntNodes) {
		if (tree->value->GetRenderOffset().InRange(Camera::GetCamExtentWorld(-1.f) + addNeg, Camera::GetCamExtentWorld(1.f) + addPos)) continue;
		tree->value->SetRenderOfffset(RandTreePos());
	}
	if (Main::moving) lastInpVec = Main::fInputVec;
}
Environment::~Environment(void) {
	for (auto tree : treeEntNodes) {
		Physics::UnsubStandaloneEnt(tree);
	}
#ifdef DEBUG_BUILD
	if (!lateUpdateNode) ThrowError("update node should not be nullptr, because it should be assigned on main(int, char*[]).");
#endif
	Main::Updates -= lateUpdateNode;
}