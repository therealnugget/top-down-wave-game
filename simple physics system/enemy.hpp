#pragma once
#include "linkedList.hpp"
class Enemy : public Behaviour {
private:
	virtual void Update(void);
protected:
	Enemy(CollisionCallback);
};