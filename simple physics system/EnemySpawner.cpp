#include "EnemySpawner.hpp"
#include "bob.hpp"
#include "multicast delegates.hpp"
#include "Item.hpp"
#include "Crystal.hpp"
int EnemySpawner::frameIndex;
int EnemySpawner::numSpawnedEnemies = 0;
int EnemySpawner::curNumEnemies = 0;
bool EnemySpawner::lastFrameEndWave = false;
Node<Text*> *EnemySpawner::waveText = nullptr;
Timer* EnemySpawner::waveTextTimer = nullptr;
Node<SwordGuy*> *EnemySpawner::swordGuys = nullptr;
int EnemySpawner::waveIndex = 0;
int EnemySpawner::maxEnemies = 30;
Text::TextData EnemySpawner::textData;
void EnemySpawner::DestroyWaveText(void) {
	delete waveTextTimer;
	waveTextTimer = nullptr;
	delete waveText->value;
	Physics::UnSubText(waveText);
}
void EnemySpawner::Update(void) {	
	if (waveTextTimer && waveTextTimer->GetElapsedSeconds() > waveTextDisappearTime) DestroyWaveText();
	auto bIsEndWave = curNumEnemies == 0;
	if (bIsEndWave && !lastFrameEndWave) {
		maxEnemies *= wave_num_enemy_mult;
		numSpawnedEnemies = 0;
		if (waveTextTimer) DestroyWaveText();
		textData.SetText(("Wave " + std::to_string(++waveIndex)).c_str());
		waveText = Physics::SubText(new Text(&textData));
		waveTextTimer = new Timer();
	}
	lastFrameEndWave = bIsEndWave;
	if (frameIndex++ % spawnFrameInterval || numSpawnedEnemies == maxEnemies) return;
	SwordGuy *guy = new SwordGuy();
	numSpawnedEnemies++;
	curNumEnemies++;
	guy->enemySpawnNode = Node<SwordGuy*>::AddAtHeadByVal(guy, &swordGuys);
}
void EnemySpawner::DestroySwordGuy(Node<SwordGuy *> *guy) {
	auto c = new Crystal(guy->value->GetPosition());
	curNumEnemies--;
 	delete guy->value;
	Node<SwordGuy*>::Remove(&swordGuys, guy);
}
void EnemySpawner::Init(void) {
	textData = Text::TextData(waveTextSizeVec, static_cast<IntVec2>(Main::halfDisplaySize) - waveTextSizeVec * .1f - IntVec2::Up * waveTextSizeVec * .3f, "broken");
	Main::Updates += EnemySpawner::Update;
}