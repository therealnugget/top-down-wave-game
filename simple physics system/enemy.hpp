#pragma once
#include "main.hpp"
#include "physics.hpp"
#include "timer.hpp"
class SwordGuy;
class Enemy : public Behaviour {
protected:
	float health;
	Enemy(SubRBData);
	~Enemy() override;
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