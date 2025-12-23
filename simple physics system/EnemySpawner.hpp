#pragma once
#include "main.hpp"
#include "timer.hpp"
#include "Item.hpp"
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
	static int curNumEnemies;
	static int frameIndex;
	static bool lastFrameEndWave;
	static Node<SwordGuy*>* swordGuys;
	static Node<Text*> *waveText;
	static Timer *waveTextTimer;
	static constexpr int waveTextSize = 140;
	static constexpr IntVec2 waveTextSizeVec = IntVec2(waveTextSize * 2, waveTextSize);
	static Text::TextData textData;
public:
	static float minPlrDist;
	static void Init(void);
	static void DestroyWaveText(void);
	static void Update(void);
	static void DestroySwordGuy(Node<SwordGuy*>* guy);
};