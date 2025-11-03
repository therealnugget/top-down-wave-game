#pragma once
#include "main.hpp"
#include "physics.hpp"
class Enemy : public Behaviour {
private:
	virtual void Update(void);
protected:
	Enemy(SubRBData);
};