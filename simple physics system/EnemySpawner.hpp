#pragma once
#include "main.hpp"
#include "timer.hpp"
#include "Item.hpp"
#include "enemy.hpp"
class SwordGuy;
//static
class EnemySpawner final {
private:
	static constexpr int spawnFrameInterval = 15;
	static constexpr float wave_num_enemy_mult = 1.2f;
	static constexpr float waveTextDisappearTime = 2.f;
	static int maxEnemies;
	static int waveIndex;
	static int numSpawnedEnemies;
	static int frameIndex;
	static bool lastFrameEndWave;
	//there can be a maximum of 64 enemies, but this can be increased down the line if need be
	enum EnemyType {
		bob = 1,
		bat = 2,
		numEnemyTypes = 2,
	};
	inline static Enemy* SpawnEnemy(int type);
	static const std::vector<int> enemyTypeProgression;
	static int progressionIndex;
	static Node<Enemy*>* enemies;
	static Node<Text*> *waveText;
	static Timer *waveTextTimer;
	static constexpr int waveTextSize = 140;
	static constexpr IntVec2 waveTextSizeVec = IntVec2(waveTextSize * 2, waveTextSize);
	static constexpr IntVec2 waveTextOffset = IntVec2(0, 0);
	static Text::TextData textData;
public:
	static float minPlrDist;
	static Enemy* closestEnemy;
	static void Init(void);
	static void DestroyWaveText(void);
	static void Update(void);
	static void DestroyEnemy(Node<Enemy*>* guy);
};