#pragma once
#include "main.hpp"
#include "physics.hpp"
#include "animations.hpp"
#include "multicast delegates.hpp"
class Crystal final : public Behaviour {
private:
	void Update(void);
	typedef enum {
		idle,
		collect,
	} CrytalAnimation;
	constexpr static IntVec2 crystal_size = IntVec2(15, 15);
	bool destroy = false;

public:
	Crystal(FVector2 pos) : Behaviour(SubRBData("main/crystal", Animations::MakeAnimStrs(2, idle, "crystal_spin", collect, "crystal_collect"), FVector2::GetOne() * 1.f * Physics::DefaultSquareVerticesVec, pos, crystal_size, std::initializer_list<FVector2>(), FVector2::Zero, IntVec2::Zero, Main::Tag::crystal, [this](Collision* collision) {
		if (!collision->CompareTag(Main::Tag::playerTrigCrystal)) return;
		destroy = true;
		}, std::unordered_map<std::string, std::variant<FVector2, FVector2*>>(), std::unordered_map<std::string, bool>(), FVector2::Zero, .0, 1.0f, false, true, { "" })) {
		SetUpdateNode(Main::Updates += [this]() {Update(); });
		rb->SetLayer(Main::Layer::playerLayer);
	}
};