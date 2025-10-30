#include "enemy.hpp"
Enemy::Enemy(CollisionCallback callback): Behaviour(callback) {
	//auto temp = Physics::SubscribeEntity("sword guy", animStrs, FVector2(17.f / 64.f, 24.f / 64.f) * Physics::GetDefaultSquareVertVec(), Main::halfDisplaySize + FVector2::GetRight() * 350.f, FVector2::GetOne() * 300.f, std::initializer_list<FVector2>(), FVector2::Zero, FVector2(-150.f, -300.f * 39.f / 64.f), { "right" });

	/*animStrs[overhead] = "overhead";
	animStrs[swing] = "swing";
	animStrs[death] = "death";
	animStrs[dash] = "dash";
	animStrs[hurt] = "hurt";
	animStrs[idle] = "idle";
	animStrs[run] = "run";
	animStrs[walk] = "walk";
	animStrs[jump] = "jump";
	player = plrNode->value;*/
}
void Enemy::Update(void) {

}