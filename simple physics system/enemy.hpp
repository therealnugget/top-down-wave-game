#pragma once
#include "main.hpp"
#include "physics.hpp"
#include "timer.hpp"
class SwordGuy;
class Enemy : public Behaviour {
private:
	static constexpr IntVec2 healthBarOffset = { -13, -30 };
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
	inline IntVec2 GetDesiredHBPos() {
		return static_cast<IntVec2>(GetPosition()) + healthBarOffset;
	}
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