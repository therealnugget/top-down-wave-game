#include "orc.hpp"
Orc::Orc(CollisionCallback callback): Enemy(callback) {
	//auto temp = Physics::SubscribeEntity("sword guy", animStrs, FVector2(17.f / 64.f, 24.f / 64.f) * Physics::GetDefaultSquareVertVec(), Main::halfDisplaySize + FVector2::GetRight() * 350.f, FVector2::GetOne() * 300.f, std::initializer_list<FVector2>(), FVector2::Zero, FVector2(-150.f, -300.f * 39.f / 64.f), { "right" });

}
void Orc::Update(void) {

}