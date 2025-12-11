#pragma once
#include "main.hpp"
#include "physics.hpp"
#include "animations.hpp"
#include "multicast delegates.hpp"
#include "player.hpp"
#include "timer.hpp"
class Crystal final : public Behaviour {
private:
	void Update(void);
	typedef enum {
		idle,
		collect,
	} CrytalAnimation;
	constexpr static IntVec2 crystal_size = IntVec2(15, 15);
	constexpr static float crystal_out_force = 100.f;
	constexpr static float destroyDistance = 23.f;
	constexpr static float crystal_in_force_add = 200000000.f;
	constexpr static float max_alive_seconds = 2.5f;
	constexpr static float start_in_speed = 40000000.f;
	bool home;
	float crystalInForce;
	Timer *homeTime;
public:
	Crystal(FVector2 pos): crystalInForce(start_in_speed), home(false), Behaviour(SubRBData("main/crystal", Animations::MakeAnimStrs(2, idle, "crystal_spin", collect, "crystal_collect"), FVector2::GetOne() * 1.f * Physics::DefaultSquareVerticesVec, pos, crystal_size, std::initializer_list<FVector2>(), FVector2::Zero, IntVec2::Zero, Main::Tag::crystal, [this](Collision* collision) {
		if (!collision->CompareTag(Main::Tag::playerTrigCrystal) || colOnFrame) return;
		home = true;
		homeTime = new Timer();
		rb->AddForce(FVector2::FromTo(Player::GetPosition(), rb->GetPosition()).Normalized() * crystal_out_force);
		}, std::unordered_map<std::string, std::variant<FVector2, FVector2*>>(), std::unordered_map<std::string, bool>(), FVector2::Zero, .0, 1.0f, false, true, Main::empty_cc_init)) {
		entity->SetNotLoop(collect);
		SetUpdateNode(Main::Updates += [this]() {Update(); });
		rb->SetLayer(Main::Layer::playerLayer);
	}
};