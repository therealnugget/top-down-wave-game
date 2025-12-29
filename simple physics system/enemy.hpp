#pragma once
#include "main.hpp"
#include "physics.hpp"
#include "timer.hpp"
class SwordGuy;
class Enemy : public Behaviour {
protected:
	static constexpr const char* confusedPath = "question mark/question mark";
	static constexpr IntVec2 confusedSize = { 40, 40 };
	static float knockBack;
	float health;
	IntVec2 confusedImgOffset;
	Textures::TextureRect confusedTex;
	inline IntVec2 GetConfusedPos(void) {
		return entity->GetRectPosition() + confusedImgOffset;
	}
	bool turned;
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
	Enemy(SubRBData, IntVec2 = IntVec2(155, 20), float = 1.f, float = .5f, float = 19000000.f, int = 2);
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