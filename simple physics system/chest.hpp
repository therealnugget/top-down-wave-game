#pragma once
#include "main.hpp"
#include "timer.hpp"
#include "physics.hpp"
//static
class ChestSpawner final {
private:
	static Node<std::function<void(void)>>* updateNode;
	static void Update(void);
	static void RandomizeTimer(void);
	static Timer chestSpawnTimer;
	enum chestAnims {
		idle = 0,
		opening = 1,
		numAnims = 2,
	};
	constexpr static FVector2 chest_collider_size = FVector2(1.f, 13.f / 16.f);
	constexpr static FVector2 chest_size = FVector2(30.f, 30.f * 13.f / 16.f);
	constexpr static float chest_rand_time_min = 10.f;
	constexpr static float chest_rand_time_max = 14.f;
public:
	static void Init(void);
	friend class Chest;
};
class Chest final : public Behaviour {
private:
	void OnCollision(Collision*);
	void Update(void);
public:
	Chest(SubRBData data);
	friend class ChestSpawner;
};
class HealthPotion final : public Behaviour {
	//TODO: make this class
};