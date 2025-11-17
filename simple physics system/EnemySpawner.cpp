#include "EnemySpawner.hpp"
#include "bob.hpp"
#include "multicast delegates.hpp"
int EnemySpawner::frameIndex;
int EnemySpawner::numSpawnedEnemies = 0;
Node<SwordGuy*> *EnemySpawner::swordGuys = nullptr;
void EnemySpawner::Update(void) {
	if (frameIndex++ % spawnFrameInterval || numSpawnedEnemies > maxEnemies) return;
	SwordGuy *guy = new SwordGuy();
	numSpawnedEnemies++;
	guy->enemySpawnNode = Node<SwordGuy*>::AddAtHeadByVal(guy, &swordGuys);
}
void EnemySpawner::DestroySwordGuy(Node<SwordGuy *> *guy) {
 	delete guy->value;
	Node<SwordGuy*>::Remove(&swordGuys, guy);
}
void EnemySpawner::Init(void) {
	Main::Updates += EnemySpawner::Update;
}