#include "EnemySpawner.hpp"
#include "bob.hpp"
#include "bat.hpp"
#include "multicast delegates.hpp"
#include "Item.hpp"
#include "Crystal.hpp"
#include "types.hpp"
int EnemySpawner::frameIndex;
int EnemySpawner::numSpawnedEnemies = 0;
int EnemySpawner::curNumEnemies = 0;
bool EnemySpawner::lastFrameEndWave = false;
Node<Text*> *EnemySpawner::waveText = nullptr;
Timer* EnemySpawner::waveTextTimer = nullptr;
Node<Enemy*> *EnemySpawner::enemies = nullptr;
int EnemySpawner::waveIndex = 0;
int EnemySpawner::maxEnemies = 30;
int EnemySpawner::progressionIndex = -1;
Text::TextData EnemySpawner::textData;
float EnemySpawner::minPlrDist = FLT_MAX;
Enemy* EnemySpawner::closestEnemy;
const std::vector<int> EnemySpawner::enemyTypeProgression = { bob, bob | bat, bat };
void EnemySpawner::DestroyWaveText(void) {
	delete waveTextTimer;
	waveTextTimer = nullptr;
	delete waveText->value;
	Physics::UnSubText(waveText);
}
Enemy *EnemySpawner::SpawnEnemy(int type) {
	switch (type) {
	case bob:
		return new SwordGuy();
	case bat:
		return new Bat();
	}
}
void EnemySpawner::Update(void) {
	minPlrDist = FLT_MAX;
	closestEnemy = nullptr;
	if (waveTextTimer && waveTextTimer->GetElapsedSeconds() > waveTextDisappearTime) DestroyWaveText();
	auto bIsEndWave = curNumEnemies == 0;
	if (bIsEndWave && !lastFrameEndWave) {
		maxEnemies *= wave_num_enemy_mult;
		progressionIndex = (progressionIndex + 1) % enemyTypeProgression.size();
		numSpawnedEnemies = 0;
		if (waveTextTimer) DestroyWaveText();
		textData.SetText(("Wave " + std::to_string(++waveIndex)).c_str());
		waveText = Physics::SubText(new Text(&textData));
		waveTextTimer = new Timer();
	}
	lastFrameEndWave = bIsEndWave;
	if (frameIndex++ % spawnFrameInterval || numSpawnedEnemies == maxEnemies) return;
	int bitIndex = 1;
	while (bitIndex != 0 && numSpawnedEnemies != maxEnemies) {
		if (enemyTypeProgression[progressionIndex] & bitIndex) {
			auto guy = SpawnEnemy(bitIndex);
			guy->enemySpawnNode = Node<Enemy*>::AddAtHeadByVal(guy, &enemies);
			numSpawnedEnemies++;
			curNumEnemies++;
		}
		bitIndex <<= 1;
	}
}
void EnemySpawner::DestroyEnemy(Node<Enemy*> *guy) {
	auto c = new Crystal(guy->value->GetPosition());
	curNumEnemies--;
 	delete guy->value;
	Node<Enemy*>::Remove(&enemies, guy);
}
void EnemySpawner::Init(void) {
	textData = Text::TextData(waveTextSizeVec, -waveTextSizeVec * .1f - IntVec2::Up * waveTextSizeVec * .3f, "broken");
	Main::Updates += EnemySpawner::Update;
}