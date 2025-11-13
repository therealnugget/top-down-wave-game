#pragma once
#include "main.hpp"
class SwordGuy;
//static
class EnemySpawner final {
private:
	static constexpr int spawnFrameInterval = 500;
	static int frameIndex;
	static Node<SwordGuy*>* swordGuys;
public:
	static void Init(void);
	static void Update(void);
	static void DestroySwordGuy(Node<SwordGuy*>* guy);
};