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
public:
	Node<SwordGuy*>* enemySpawnNode;
	friend class SubRBData;
};