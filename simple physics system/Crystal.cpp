#include "Crystal.hpp"
void Crystal::Update(void) {
	if (!home) return;
	auto dt = Main::DefCapDeltaTime();
	auto toPlr = FVector2::FromTo(rb->GetPosition(), Player::GetPosition());
	rb->AddForce(toPlr.Normalized() * crystalInForce * dt);
	crystalInForce += crystal_in_force_add * dt;
	if (toPlr.SqrMagnitude() < destroyDistance || homeTime->GetElapsedSeconds() > max_alive_seconds) entity->SetAnimation(collect);
	if (!entity->AnimFinished() || entity->GetCurAnim() != collect) return;
	Player::IncreaseProgress(progressAmount);
	delete homeTime;
	delete this;
}