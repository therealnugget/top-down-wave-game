#include "EnemyTurnProjectile.hpp"
#include "player.hpp"
FVector2 EnemyTurnProjectile::GetProjectilePos(int index) {
	if (makeNewSeeds[index]) randRots[index] = Main::GetRandFloat(-randRotationOffset, randRotationOffset);
	auto returnVal = Math::GetTurbulentPos(static_cast<FVector2>(Player::GetProjectilePos(projectileTime.GetElapsedSeconds() * baseSpeed, mouseDiff)), Player::GetPosition(), randRots[index]);
	makeNewSeeds[index] = false;
	return returnVal;
}
void EnemyTurnProjectile::DestroyProjectile(void) {
	if (!projectileNodes[0]) return;
	for (auto& projectileNode : projectileNodes) {
		Physics::DeleteRB(projectileNode);
	}
	projectileNodes[0] = nullptr;
}
void EnemyTurnProjectile::Update(void) {
	if (!Main::leftClick) {
		DestroyProjectile();
		for (bool &makeNewSeed: makeNewSeeds) makeNewSeed = true;
		return;
	}
	currentProjectileFrame++;
	if (Main::leftClickOnFrame) {
		currentProjectileFrame = 0;
		mouseDiff = Player::GetMouseDiff();
	} 
	if ((currentProjectileFrame & (projectile_frame_wait - 1)) != 0) {
		if (projectileRBs[0]) for (int i = 0; i < num_projectiles; i++) projectileRBs[i]->SetPosition(GetProjectilePos(i));
		return;
	}
	projectileTime.Reset();
	mouseDiff = Player::GetMouseDiff();
	if (!projectileNodes[0]) {
		for (int i = 0; i < num_projectiles; i++) {
			projectileNodes[i] = Player::CreatePlayerProjectile("question mark"s, "question mark", Player::GetPosition(), projectileSize, &projectileRBs[i]);
			projectileRBs[i]->tag = Main::Tag::enemyTurner;
			projectileRBs[i]->SetLayer(Main::Layer::playerProjLayer);
		}
	}
}