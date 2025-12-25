#include "enemy.hpp"
#include "multicast delegates.hpp"
#include "player.hpp"
static constexpr int default_max_health = 3;
static constexpr float default_immune_time = .6f;
int Enemy::numEnemies = 0;
bool Enemy::isSingleEnemy = true;
float Enemy::knockBack = 1400.f;
Enemy::Enemy(SubRBData data, float _damage, float _selfDamage, float _speed, int _numColsOnFrame): numColsOnFrame(_numColsOnFrame), speed(_speed), damage(_damage), selfDamage(_selfDamage), lateUpdateNode(nullptr), health(default_max_health), enabled(true), Behaviour(&data) {
	colsOnFrame.reserve(numColsOnFrame);
	colsOnFrame.emplace(Main::Tag::player, false);
	colsOnFrame.emplace(Main::Tag::enemy, false);
	numEnemies++;
	isSingleEnemy = numEnemies == 1;
	confusedImgOffset = IntVec2(155, 20);
}
Enemy::~Enemy() {
	numEnemies--;
	if (!lateUpdateNode) return;
	Main::LateUpdates -= lateUpdateNode;
}
//it's not as simple as deleting the enemy as soon as the collision callback is called. if you think about it, the collision callback is in the middle of the physics update. so we will be deleting a rigidbody in the middle of the narrow phase function, then trying to access it later on in that same function, and then again in the physics update. so the closest thing we can do is:
//1: if we need the enemy to appear as though he is deleted as soon as the collision occurs, delete the entity, and destroy the outer behaviour at the start of the next frame
// or 2: if we don't care about showing the enemy for the single frame in which he is, in this example, dead, then we can simply destroy the enemy at the start of the next frame as we've done here.
void Enemy::Update(void) {
    Behaviour::Update();
}
void Enemy::LateUpdate(void) {
	Physics::SetRealPos(confusedTex.GetRectAddr(), entity, GetPosition(), false);
	confusedTex.GetRectAddr()->x += confusedImgOffset.x;
	confusedTex.GetRectAddr()->y += confusedImgOffset.y;
	Textures::RenderStandaloneTex(confusedTex);
}
void Enemy::TakeDamage(float damageAmount) {
	health -= damageAmount;
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
    if (turned && !curCol && collision->CompareTag(enemyTag)) {
        touchingEnemy = true;
        curCol = true;
    }
    if (!isSingleEnemy && !colOnFrame && collision->CompareTag(Main::Tag::enemyTurner)) {
        if (!turned) {
            auto confusedPos = GetConfusedPos();
            confusedTex = Textures::TextureRect(Textures::InitAnim(confusedPath), SDL_Rect{ confusedPos.x, confusedPos.y, confusedSize.x, confusedSize.y });
            lateUpdateNode = (Main::LateUpdates += [this]() {LateUpdate(); });
        }
        turned = true;
        rb->tag = Main::Tag::enemyTurned;
    }
    if (colOnFrame || entity->GetCurAnim() == hurt && !entity->AnimFinished()) return;
    auto isPlr = collision->CompareTag(Main::Tag::playerAttack);
    if ((!isPlr && !collision->CompareTag(Main::Tag::enemyTurned))) return;
    colOnFrame = true;
    TakeDamage(Player::GetDamage());
    rb->AddVelocity(collision->GetNormal() * (isPlr * Player::GetKnockBack() + !isPlr * knockBack));
}