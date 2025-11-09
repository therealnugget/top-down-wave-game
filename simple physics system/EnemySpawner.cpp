#include "EnemySpawner.hpp"
#include "bob.hpp"
#include "multicast delegates.hpp"
#define TRY_LINKED_LIST
#define TEMP
#ifdef TEMP
#include "timer.hpp"
#endif
constexpr int spawnFrameInterval = 100;
static int frameIndex;
#ifdef TRY_LINKED_LIST
Node<SwordGuy*> *swordGuys = nullptr;
#else
rsize_t numSwordGuys;
uint_fast64_t curSwordGuyInd;
SwordGuy** swordGuys = nullptr;
#endif
float creationLatency, deletionLatency;
void EnemySpawner::Update(void) {
	if (Main::GetKey(SDL_SCANCODE_K) && swordGuys) {
		Timer temp2 = Timer();
#ifdef TRY_LINKED_LIST
		swordGuys->RemoveAllNodesWVal(&swordGuys, true);
#else
		for (uint_fast64_t i = 0; i < numSwordGuys; i++) {
			delete swordGuys[i];
		}
		delete[] swordGuys;
		swordGuys = nullptr;
#endif
		temp2.PrintElapsedSeconds("deleting latency:");
	}
	if (frameIndex++ % spawnFrameInterval) return;
	Timer timer = Timer();
#ifdef TRY_LINKED_LIST
	Node<SwordGuy*>::AddAtHeadByVal(new SwordGuy(), &swordGuys);
#else
	if (curSwordGuyInd == numSwordGuys - 1 || numSwordGuys == 0) swordGuys = static_cast<SwordGuy**>(realloc(swordGuys, sizeof(SwordGuy *) * (numSwordGuys = (numSwordGuys == 0 + numSwordGuys * 2))));
	swordGuys[curSwordGuyInd++] = new SwordGuy();
#endif
	timer.PrintElapsedSeconds("creation latency:");
}
void EnemySpawner::Init(void) {
	Main::Updates += EnemySpawner::Update;
}