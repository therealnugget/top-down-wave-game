#include "chest.hpp"
#include "multicast delegates.hpp"
#include "animations.hpp"
#include "camera.hpp"
Node<std::function<void(void)>>* ChestSpawner::updateNode;
Timer ChestSpawner::chestSpawnTimer;
void ChestSpawner::Init(void) {
	updateNode = (Main::Updates += Update);
	chestSpawnTimer.Reset(Main::GetRandFloat(.0f, chest_rand_time_min));
}
void ChestSpawner::Update(void) {
	//if (chestSpawnTimer.GetElapsedSeconds() < chest_rand_time_max) return;
	static bool temp = false;
	if (!temp) {
		temp = true;
	}
	else return;
	new Chest(SubRBData("chest"s, Animations::MakeAnimStrs(numAnims, idle, "idle", opening, "opening"), static_cast<FVector2>(chest_collider_size) * Physics::DefaultSquareVerticesVec, Main::GetRandFVec(Camera::GetCamExtentWorld(-1.f), Camera::GetCamExtentWorld(1.f)), chest_size, std::initializer_list<FVector2>(), FVector2::Zero, -chest_size * .5f, Main::Tag::chest, true, nullptr, std::unordered_map<std::string, std::variant<FVector2, FVector2*>>(), std::unordered_map<std::string, bool>(), FVector2::Zero, .0f, 1.f, false, false, {"right"}, true, .0f, Main::Layer::all));
}
Chest::Chest(SubRBData data) : Behaviour(&data, true) {
	rb->SetCollisionCallback([this](Collision* col) {OnCollision(col); });
}
void Chest::OnCollision(Collision *collision) {
	if (!collision->CompareTag(Main::Tag::player)) return;
	colOnFrame = true;
	entity->SetAnimation(ChestSpawner::opening);
}