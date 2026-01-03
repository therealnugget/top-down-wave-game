#include "enemy.hpp"
#include "multicast delegates.hpp"
#include "player.hpp"
#include "EnemySpawner.hpp"
static constexpr float default_immune_time = .6f;
int Enemy::numEnemies = 0;
bool Enemy::isSingleEnemy = true;
float Enemy::knockBack = 1400.f;
const std::unordered_map<int, const char*> Enemy::debuffPaths = { {confused, "question mark/question mark"}, {poisoned, "poison/poision"} };
Enemy::Enemy(SubRBData data, IntVec2 debugOffset, int max_health, float _damage, float _selfDamage, float _speed, int _numColsOnFrame): _debuffActive(0), debugImgOffset(debugOffset), numColsOnFrame(_numColsOnFrame), speed(_speed), damage(_damage), selfDamage(_selfDamage), lateUpdateNode(nullptr), health(max_health), enabled(true), Behaviour(&data) {
	colsOnFrame.reserve(numColsOnFrame);
	colsOnFrame.emplace(Main::Tag::player, false);
	colsOnFrame.emplace(Main::Tag::enemy, false);
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
        auto& confusedTex = *texRect.second;
        Physics::SetRealPos(confusedTex.GetRectAddr(), entity, GetPosition(), false);
        confusedTex.GetRectAddr()->x += debugImgOffset.x;
        confusedTex.GetRectAddr()->y += debugImgOffset.y;
        Textures::RenderStandaloneTex(confusedTex);
    }
}
void Enemy::TakeDamage(float damageAmount) {
	health -= damageAmount;
}
void Enemy::AddDebuffTex(int debuff) {
    debuffIndex++;
    auto debuffPos = GetDebuffPos(static_cast<float>(debuffIndex) - static_cast<float>(debuffTexes.size() - 1) * .5f);
    debuffTexes.emplace(debuff, new Textures::TextureRect(Textures::InitAnim(debuffPaths.at(debuff)), SDL_Rect{ debuffPos.x, debuffPos.y, confusedSize.x, confusedSize.y }));
}
void Enemy::CollisionCallback(Collision* collision) {
    if (!enabled) return;
    register auto plrTag = Main::Tag::player;
    auto& curCol = colsOnFrame[plrTag];
    if (!curCol && collision->CompareTag(plrTag)) {
        EnactDamage();
        curCol = true;
    }
    register auto enemyTag = Main::Tag::enemy;
    curCol = colsOnFrame[enemyTag];
    if (GetDebuffActive(confused) && !curCol && collision->CompareTag(enemyTag)) {
        touchingEnemy = true;
        curCol = true;
    }
    if (!colOnFrame) {
        if (collision->CompareTag(Main::Tag::poison)) {
            colOnFrame = true;
            AddDebuffTex(poisoned);
        }
        if (!isSingleEnemy && !GetDebuffActive(confused) && collision->CompareTag(Main::Tag::enemyTurner)) {
            colOnFrame = true;
            AddDebuffTex(confused);
            lateUpdateNode = (Main::LateUpdates += [this]() {LateUpdate(); });
            SetDebuffActive(confused);
            rb->tag = Main::Tag::enemyTurned;
        }
        if (entity->GetCurAnim() == hurt && !entity->AnimFinished()) return;
        auto isPlr = collision->CompareTag(Main::Tag::playerAttack);
        if ((!isPlr && !collision->CompareTag(Main::Tag::enemyTurned))) return;
        colOnFrame = true;
#ifdef DEBUG_BUILD
        if (!derivedTakeDamage) ThrowError("derivedTakeDamage of enemy base class has not been assigned in the derived class. please assign it to the base function of TakeDamage(float).");
#endif
        derivedTakeDamage(isPlr * Player::GetDamage() + !isPlr * selfDamage);
        rb->AddVelocity(collision->GetNormal() * (isPlr * Player::GetKnockBack() + !isPlr * knockBack));
    }
}
//it's not as simple as deleting the enemy as soon as the collision callback is called. if you think about it, the collision callback is in the middle of the physics update. so we will be deleting a rigidbody in the middle of the narrow phase function, then trying to access it later on in that same function, and then again in the physics update. so the closest thing we can do is:
//1: if we need the enemy to appear as though he is deleted as soon as the collision occurs, delete the entity, and destroy the outer behaviour at the start of the next frame
// or 2: if we don't care about showing the enemy for the single frame in which he is, in this example, dead, then we can simply destroy the enemy at the start of the next frame as we've done here.
void Enemy::Update(void) {
    if (!enabled) return;
    Behaviour::Update();
    for (auto& col : colsOnFrame) col.second = false;
    curAnim = entity->GetCurAnim();
    animFinished = entity->AnimFinished();
    toPlr = rb->GetPosition().To(Player::GetPosition());
    SetPlayerDist();
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
}