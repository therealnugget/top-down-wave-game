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
	virtual void SetPlayerDist(void) = 0;
	virtual void EnactDamage(void) = 0;
	virtual void CollisionCallback(Collision*);
	Enemy(SubRBData, float = 1.f, float = .5f, float = 19000000.f, int = 2);
	~Enemy() override;
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
public:
	virtual void TakeDamage(float damageAmount);
	Node<SwordGuy*>* enemySpawnNode;
	inline FVector2 GetPosition() {
		return rb->newPosition;
	}
	friend class SubRBData;
};