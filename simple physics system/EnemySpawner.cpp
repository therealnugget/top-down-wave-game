#include "EnemySpawner.hpp"
#include "bob.hpp"
#include "multicast delegates.hpp"
constexpr int spawnFrameInterval = 100;
static int frameIndex;
Node<SwordGuy*> *swordGuys = nullptr;
float creationLatency, deletionLatency;
void EnemySpawner::Update(void) {
	if (Main::GetKey(SDL_SCANCODE_K) && swordGuys) {
		swordGuys->RemoveAllNodesWVal(&swordGuys, true);
	}
	if (frameIndex++ % spawnFrameInterval) return;
	Node<SwordGuy*>::AddAtHeadByVal(new SwordGuy(), &swordGuys);
}
void EnemySpawner::Init(void) {
	Main::Updates += EnemySpawner::Update;
}