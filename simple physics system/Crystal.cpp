#include "Crystal.hpp"
void Crystal::Update(void) {
	if (!home) {
		if (aliveTime.GetElapsedSeconds() > max_alive_seconds) goto destroy;
		goto ret;
	}
	{
		auto dt = Main::DefCapDeltaTime();
		auto toPlr = FVector2::FromTo(rb->GetPosition(), Player::GetPosition());
		rb->AddForce(toPlr.Normalized() * crystalInForce * dt);
		crystalInForce += crystal_in_force_add * dt;
		if (toPlr.SqrMagnitude() < destroyDistance || homeTime->GetElapsedSeconds() > max_home_seconds) entity->SetAnimation(collect);
	}
	if (!entity->AnimFinished() || entity->GetCurAnim() != collect) goto ret;
	Player::IncreaseProgress(progressAmount);
destroy:
	_freea(homeTime);
	delete this;
ret:
	pastHome = home;
}