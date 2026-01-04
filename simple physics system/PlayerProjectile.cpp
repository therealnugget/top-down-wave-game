#include "PlayerProjectile.hpp"
#include "player.hpp"
FVector2 PlayerProjectile::GetProjectilePos(int index) {
	if (makeNewSeeds[index]) randRots[index] = Main::GetRandFloat(-randRotationOffset, randRotationOffset);
	auto returnVal = Math::GetTurbulentPos(static_cast<FVector2>(Player::GetProjectilePos(projectileTime.GetElapsedSeconds() * baseSpeed, mouseDiff)), Player::GetPosition(), randRots[index]);
	makeNewSeeds[index] = false;
	return returnVal;
}
void PlayerProjectile::DestroyProjectile(void) {
	if (!projectileNodes[0]) return;
	for (auto& projectileNode : projectileNodes) {
		Physics::DeleteRB(projectileNode);
	}
	projectileNodes[0] = nullptr;
}
void PlayerProjectile::Update(void) {
	if (!Main::leftClick) {
		DestroyProjectile();
		for (int i = 0; i < makeNewSeeds.size(); i++) makeNewSeeds[i] = true;
		return;
	}
	currentProjectileFrame++;
	if (Main::leftClickOnFrame) {
		currentProjectileFrame = 0;
		mouseDiff = Player::GetMouseDiff();
	} 
	if ((currentProjectileFrame & (projectileFrameWait - 1)) != 0) {
		if (projectileRBs[0]) for (int i = 0; i < numProjectiles; i++) projectileRBs[i]->SetPosition(GetProjectilePos(i));
		return;
	}
	projectileTime.Reset();
	mouseDiff = Player::GetMouseDiff();
	if (!projectileNodes[0]) {
		for (int i = 0; i < numProjectiles; i++) {
			projectileNodes[i] = Player::CreatePlayerProjectile(basePath, endPath, static_cast<IntVec2>(Player::GetPosition()), projectileSize, &projectileRBs[i]);
			projectileRBs[i]->tag = tag;
			projectileRBs[i]->SetLayer(Main::Layer::playerProjLayer);
		}
	}
}