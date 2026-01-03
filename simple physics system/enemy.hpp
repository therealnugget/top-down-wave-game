#pragma once
#include "main.hpp"
#include "physics.hpp"
#include "timer.hpp"
#include <unordered_map>
class SwordGuy;
class Enemy : public Behaviour {
private:
	static constexpr int default_max_health = 3;
	static const std::unordered_map<int, const char*> debuffPaths;
	int debuffIndex;
	std::unordered_map<int, Textures::TextureRect*> debuffTexes;
	void AddDebuffTex(int debuff);
	inline IntVec2 GetDebuffPos(float index) {
		return entity->GetRectPosition() + static_cast<FVector2>(debugImgOffset) * index;
	}
	IntVec2 debugImgOffset;
	//don't access this directly. use the "GetDebuffActive(int)" and "SetDebuffActive(int)" functions.
	int _debuffActive;
protected:
	bool animFinished;
	static constexpr IntVec2 confusedSize = { 40, 40 };
	static float knockBack;
	float health;
	enum debuffType {
		confused = 1,
		poisoned = 2,
	};
	inline bool GetDebuffActive(int debuff) {
		return _debuffActive & debuff;
	}
	inline void SetDebuffActive(int bitIndex, bool value = true) {
		_debuffActive = (_debuffActive | bitIndex) * value + (_debuffActive & ~bitIndex) * !value;
	}
	bool touchingEnemy;
	int numColsOnFrame;
	std::unordered_map<int, bool> colsOnFrame;
	float speed;
	float damage;
	float plrDistSqr;
	float selfDamage;
	FVector2 toPlr;
	int curAnim;
	static int numEnemies;
	static bool isSingleEnemy;
	void SetPlayerDist(void);
	void EnactDamage(void);
	virtual void CollisionCallback(Collision*);
	Enemy(SubRBData, IntVec2 = IntVec2(155, 20), int max_health = default_max_health, float damage = 1.f, float selfDamage = .5f, float speed = 19000000.f, int numColsOnFrame = 2);
	virtual void Update(void);
	virtual void LateUpdate(void);
	Node<std::function<void(void)>>* lateUpdateNode;
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
	//strange, isn't it? a function that calls from the most derived to the base class. eù_e
	std::function<void(float)> derivedTakeDamage;
public:
	~Enemy() override;
	virtual void TakeDamage(float damageAmount);
	Node<Enemy*>* enemySpawnNode;
	inline FVector2 GetPosition() {
		return rb->newPosition;
	}
	friend class SubRBData;
};