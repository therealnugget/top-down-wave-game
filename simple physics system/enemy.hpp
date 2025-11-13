#pragma once
#include "main.hpp"
#include "physics.hpp"
#include "timer.hpp"
class SwordGuy;
class Enemy : public Behaviour {
private:
	static constexpr IntVec2 healthBarOffset = { -46, -106 };
	Node<Entity*>* healthBar;
	Entity* healthBarEnt;
	float maxHealth;
	float health;
	inline float GetHealthProportion() {
		return health / maxHealth;
	}
	static constexpr int healthbar_animation = 0;
	float immuneTime;
	Timer *immuneTimer;
protected:
	Enemy(SubRBData);
	~Enemy();
	virtual void Update(void);
	virtual void CollisionCallback(Collision&);
	bool enabled;
public:
	Node<SwordGuy*>* enemySpawnNode;
	friend class SubRBData;
};