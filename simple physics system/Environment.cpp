#include "Environment.hpp"
#include "multicast delegates.hpp"
Node<std::function<void(void)>>* Environment::updateNode = nullptr;
Node<Entity*>* Environment::treeEntNodes[Environment::numTrees];
void Environment::Init(void) {
	updateNode = (Main::Updates += []() {Update(); });
	for (auto &tree: treeEntNodes) {
		tree = Physics::SubStandaloneEnt(Entity::MakeEntity("environment", { "green tree" }, FVector2::Zero, treeSize, true, Main::GetRandFVec(-Main::halfDisplaySize, Main::halfDisplaySize)));
	}
}
void Environment::Update(void) {
	//pass for now
}
Environment::~Environment(void) {
	for (auto tree : treeEntNodes) {
		Physics::UnsubStandaloneEnt(tree);
	}
#ifdef DEBUG_BUILD
	if (!updateNode) ThrowError("update node should not be nullptr, because it should be assigned on main(int, char*[]).");
#endif
	Main::Updates -= updateNode;
}