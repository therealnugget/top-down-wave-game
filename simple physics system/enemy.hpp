#pragma once
#include "main.hpp"
#include "physics.hpp"
#include "timer.hpp"
class SwordGuy;
class Enemy : public Behaviour {
protected:
	int health;
	Enemy(SubRBData);
	~Enemy();
	virtual void Update(void);
	bool enabled;
	enum EnemyAnimation {
		idle,
		death,
		hurt,
		run,
		walk,
		jump,
		numEnemyAnims,
	};
public:
	Node<SwordGuy*>* enemySpawnNode;
	friend class SubRBData;
};