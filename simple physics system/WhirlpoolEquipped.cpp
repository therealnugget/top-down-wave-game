#include "main.hpp"
#include "WhirlpoolEquipped.hpp"
#include "Item.hpp"
void WhirlPoolEquipped::Update(void) {
	rb->SetSize(effectSize + IntVec2(FVector2::GetOne() * (sinf(aliveSeconds * effectDilationSpeed) + 1.f) * effectSizeDilation), true);
	aliveSeconds += Main::DeltaTime();
}
#ifdef DEBUG_BUILD
bool WhirlPoolEquipped::instantiated = false;
#endif
WhirlPoolEquipped::WhirlPoolEquipped(void) : aliveSeconds(.0f) {
#ifdef DEBUG_BUILD
	if (instantiated) {
		ThrowError("there should not be more than one instance of the \"WhirlPoolEquipped\" class.");
	}
	instantiated = true;
#endif
	auto data = SubRBData(WhirlPool::startPath, { WhirlPool::endPath }, Physics::DefaultSquareVerticesVec, FVector2::Zero, effectSize, std::initializer_list<FVector2>(), FVector2::Zero, effectSize * -.5f, Main::Tag::whirlPool, false, nullptr, std::unordered_map<std::string, std::variant<FVector2, FVector2*>>(), std::unordered_map<std::string, bool>(), FVector2::Zero, .0f, 1.f, true, true, { Main::empty_cc });
	node = Physics::SubscribeEntity(&data);
	rb = node->value;
	updateNode = Main::Updates += [this]() { Update(); };
}
WhirlPoolEquipped::~WhirlPoolEquipped(void) {
	Physics::DeleteRB(node);
	node = nullptr;
	Main::Updates -= updateNode;
}