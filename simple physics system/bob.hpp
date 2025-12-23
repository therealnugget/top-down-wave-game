#pragma once
#include "enemy.hpp"
#include "math.hpp"
#include "player.hpp"
#include "multicast delegates.hpp"
class SwordGuy final: public Enemy {
private:
	void Update(void) override;
	void LateUpdate(void);
	enum SGAnimation {
		idle,
		death,
		hurt,
		walk,
		run,
		jump,
		overhead,
		swing,
		dash,
		numSGAnims,
	};
	//-1 for yet to decide
	int attackDecision;
	bool decideAttack;
	bool animFinished;
	bool plrColOnFrm;
	bool turned;
	float speed;
	float plrDistSqr;
	float signAtDash;
	float damage;
	float selfDamage;
	FVector2 toPlr;
	Textures::TextureRect confusedTex;
	Node<std::function<void(void)>>* lateUpdateNode;
	static FVector2 closestEnemyPos;
	static SwordGuy *closestEnemyBehav;
	static constexpr float dashPositionAdd = 200.f;
	static constexpr float attackDistSqr = 10000.f;
	static constexpr float dashDistSqr = 17000.f;
	static constexpr const char* confusedPath = "question mark/question mark";
	static constexpr IntVec2 confusedImgOffset = { 0, 50 };
	static constexpr IntVec2 confusedSize = { 40, 40 };
	inline IntVec2 GetConfusedPos(void) {
		return entity->GetRectPosition() + confusedImgOffset;
	}
	void TakeDamage(float damageAmount);
	void SetPlayerDist(void);
	void EnactDamage(void);
	void CollisionCallback(Collision*);
	int curAnim;
	void ResetIfAttkFin(int animation);
public:
	SwordGuy();
	~SwordGuy() override {
		if (!lateUpdateNode) return;
		Main::LateUpdates -= lateUpdateNode;
	}
	inline FVector2 GetPosition() {
		return rb->newPosition;
	}
};