#include "EnemySpawner.hpp"
#include "bob.hpp"
#include "multicast delegates.hpp"
int EnemySpawner::frameIndex;
Node<SwordGuy*> *EnemySpawner::swordGuys = nullptr;
void EnemySpawner::Update(void) {
	if (frameIndex++ % spawnFrameInterval) return;
	SwordGuy *guy = new SwordGuy();
	guy->enemySpawnNode = Node<SwordGuy*>::AddAtHeadByVal(guy, &swordGuys);
}
void EnemySpawner::DestroySwordGuy(Node<SwordGuy *> *guy) {
 	delete guy->value;
	Node<SwordGuy*>::Remove(&swordGuys, guy);
}
void EnemySpawner::Init(void) {
	Main::Updates += EnemySpawner::Update;
}