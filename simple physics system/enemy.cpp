#include "enemy.hpp"
#include "multicast delegates.hpp"
#include "player.hpp"
#include "EnemySpawner.hpp"
static constexpr float default_immune_time = .6f;
int Enemy::numEnemies = 0;
bool Enemy::isSingleEnemy = true;
float Enemy::knockBack = 1400.f;
std::vector<int> Enemy::insigniaTagList;
const std::unordered_map<int, const char*> Enemy::debuffPaths = { {confused, "question mark/question mark"}, {poisoned, "poison/poison debuff"} };
Enemy::Enemy(SubRBData data, IntVec2 debugOffset, int max_health, float _damage, float _selfDamage, float _speed, int _numColsOnFrame): frameIndex(0), _debuffActive(0), debugImgOffset(debugOffset), numColsOnFrame(_numColsOnFrame), speed(_speed), damage(_damage), selfDamage(_selfDamage), lateUpdateNode(nullptr), health(max_health), Behaviour(&data) {
	colsOnFrame.reserve(numColsOnFrame);
	colsOnFrame.emplace(Main::Tag::player, false);
	colsOnFrame.emplace(Main::Tag::enemy, false);
	colsOnFrame.emplace(Main::Tag::whirlPool, false);
	colsOnFrame.emplace(Main::Tag::wrath, false);
	isSingleEnemy = ++numEnemies == 1;
}
Enemy::~Enemy() {
    isSingleEnemy = --numEnemies == 1;
    if (!lateUpdateNode) return;
	Main::LateUpdates -= lateUpdateNode;
}
void Enemy::SetPlayerDist(void) {
    plrDistSqr = toPlr.SqrMagnitude();
}
void Enemy::EnactDamage(void) {
    if (!GetDebuffActive(confused)) {
        Player::TakeDamage(damage);
        return;
    }
    auto closestEnemyBehav = EnemySpawner::closestEnemy;
    if (!closestEnemyBehav || closestEnemyBehav == this) return;
    closestEnemyBehav->TakeDamage(selfDamage);
}
void Enemy::LateUpdate(void) {
    for (auto& texRect : debuffTexes) {
        auto& confusedTex = texRect.second.texture;
        Physics::SetRealPos(confusedTex.GetRectAddr(), entity, GetPosition() + static_cast<FVector2>(GetDebuffPos(texRect.second.index)), false);
        Textures::RenderStandaloneTex(confusedTex);
    }
}
//don't call this directly. call OnDamaged() instead
void Enemy::TakeDamage(float damageAmount) {
	health -= damageAmount;
}
IntVec2 Enemy::GetDebuffPos(int index) {
    return IntVec2(debugImgOffset.x + debuffSeparation * index * ((index & 1) * 2 - 1), debugImgOffset.y);
}
void Enemy::AddDebuffTex(int debuff) {
    if (!lateUpdateNode) lateUpdateNode = (Main::LateUpdates += [this]() {LateUpdate(); });
    SetDebuffActive(debuff);
    int debuffTexSize = debuffTexes.size();
    auto debuffPos = GetDebuffPos(debuffTexSize);
#ifdef DEBUG_BUILD
    if (!debuffPaths.contains(debuff)) ThrowError("please add the enumerator and file path to \"debuffPaths\" at the top of ", __FILE__, ".");
#endif
    debuffTexes.emplace(debuff, Debuff(Textures::TextureRect(Textures::InitAnim(debuffPaths.at(debuff)), SDL_Rect{ debuffPos.x, debuffPos.y, debuffSize.x, debuffSize.y }), debuffTexSize));
}
void Enemy::OnDamaged(float damageAmount, FVector2 velocityChange) {
    if (entity->GetCurAnim() == hurt && !entity->AnimFinished()) return;
#ifdef DEBUG_BUILD
    if (!derivedTakeDamage) ThrowError("derivedTakeDamage of enemy base class has not been assigned in the derived class. please assign it to the base function of TakeDamage(float).");
#endif
    derivedTakeDamage(damageAmount);
    rb->AddVelocity(velocityChange);
}
void Enemy::CollisionCallback(Collision* collision) {
    if (!enabled) return;
    auto plrTag = Main::Tag::player;
    auto& curCol = colsOnFrame[plrTag];
    if (!curCol && collision->CompareTag(plrTag)) {
        EnactDamage();
        curCol = true;
        return;
    }
    for (int tag : insigniaTagList) {
        curCol = colsOnFrame[tag];
        if (curCol) continue;
        OnDamaged(InsigniaEquipped::GetDamage(tag), FVector2::Zero);
        curCol = true;
        return;
    }
    auto enemyTag = Main::Tag::enemy;
    curCol = colsOnFrame[enemyTag];
    if (GetDebuffActive(confused) && !curCol && collision->CompareTag(enemyTag)) {
        touchingEnemy = true;
        curCol = true;
        return;
    }
    if (!colOnFrame) {
        if (collision->CompareTag(Main::Tag::poison) && !GetDebuffActive(poisoned)) {
            colOnFrame = true;
            AddDebuffTex(poisoned);
            return;
        }
        if (!isSingleEnemy && !GetDebuffActive(confused) && collision->CompareTag(Main::Tag::enemyTurner)) {
            colOnFrame = true;
            AddDebuffTex(confused);
            rb->tag = Main::Tag::enemyTurned;
            return;
        }
        auto isPlr = collision->CompareTag(Main::Tag::playerAttack);
        if (!isPlr && !collision->CompareTag(Main::Tag::enemyTurned)) return;
        colOnFrame = true;
        OnDamaged(isPlr * Player::GetDamage() + !isPlr * selfDamage, collision->GetNormal() * (isPlr * Player::GetKnockBack() + !isPlr * knockBack));
    }
}
//it's not as simple as deleting the enemy as soon as the collision callback is called. if you think about it, the collision callback is in the middle of the physics update. so we will be deleting a rigidbody in the middle of the narrow phase function, then trying to access it later on in that same function, and then again in the physics update. so the closest thing we can do is:
//1: if we need the enemy to appear as though he is deleted as soon as the collision occurs, delete the entity, and destroy the outer behaviour at the start of the next frame
// or 2: if we don't care about showing the enemy for the single frame in which he is, in this example, dead, then we can simply destroy the enemy at the start of the next frame as we've done here.
void Enemy::Update(void) {
    if (!enabled) return;
    for (auto& col : colsOnFrame) col.second = false;
    curAnim = entity->GetCurAnim();
    animFinished = entity->AnimFinished();
    toPlr = rb->GetPosition().To(Player::GetPosition());
    SetPlayerDist();
    if (InsigniaEquipped::GetWhirlPoolActive() && plrDistSqr < InsigniaEquipped::GetPullDstSqr()) rb->AddForce(toPlr.Normalized() * InsigniaEquipped::GetPullForce());
    if (plrDistSqr < EnemySpawner::minPlrDist) {
        EnemySpawner::minPlrDist = plrDistSqr;
        EnemySpawner::closestEnemy = this;
    }
    if (isSingleEnemy && GetDebuffActive(confused)) {
        Main::LateUpdates -= lateUpdateNode;
        lateUpdateNode = nullptr;
        SetDebuffActive(confused, false);
    }
    if (GetDebuffActive(confused)) {
        toPlr = GetPosition().To(EnemySpawner::closestEnemy->GetPosition());
        SetPlayerDist();
    }
    if (GetDebuffActive(poisoned) && (frameIndex++ % Poison::damageFrameWait) == 0) {
        derivedTakeDamage(Poison::damageAmount);
    }
    Behaviour::Update();
}