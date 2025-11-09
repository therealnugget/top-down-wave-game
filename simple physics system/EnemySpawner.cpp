#include "EnemySpawner.hpp"
#include "bob.hpp"
#include "multicast delegates.hpp"
void EnemySpawner::Update(void) {
	//temp code lol
	static int i = 0;
	if (i++ % 100) return;
	SwordGuy* guy = new SwordGuy();
	//delete guy;
}
void EnemySpawner::Init(void) {
	Main::Updates += EnemySpawner::Update;
}