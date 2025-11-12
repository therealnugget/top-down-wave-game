#pragma once
#include "main.hpp"
#include "physics.hpp"
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
protected:
	Enemy(SubRBData);
	virtual void Update(void);
	virtual void CollisionCallback(Collision&);
	//for collision callback
	friend class SubRBData;
};