#include "enemy.hpp"
#include "EnemySpawner.hpp"
static constexpr float default_max_health = 100.f;
static constexpr float default_immune_time = .1f;
Enemy::Enemy(SubRBData data): maxHealth(default_max_health), health(maxHealth), enabled(true), immuneTime(default_immune_time), Behaviour(data) {
    auto healthBarSize = IntVec2(100.f, 50.f);
    healthBar = Physics::SubStandaloneEnt(Entity::MakeEntity("health bar", { "health_bar" }, Main::halfDisplaySize, healthBarSize, healthBarSize * (IntVec2::GetOne() + IntVec2::GetRight())));
    healthBarEnt = healthBar->value;
    healthBarEnt->DeActivate();
}
Enemy::~Enemy() {
    Physics::UnsubStandaloneEnt(healthBar);
    if (immuneTimer) _freea(immuneTimer);
}
//it's not as simple as deleting the enemy as soon as the collision callback is called. if you think about it, the collision callback is in the middle of the physics update. so we will be deleting a rigidbody in the middle of the narrow phase function, then trying to access it later on in that same function, and then again in the physics update. so the closest thing we can do is:
//1: if we need the enemy to appear as though he is deleted as soon as the collision occurs, delete the entity, and destroy the outer behaviour at the start of the next frame
// or 2: if we don't care about showing the enemy for the single frame in which he is, in this example, dead, then we can simply destroy the enemy at the start of the next frame as we've done here.
void Enemy::CollisionCallback(Collision &collision){
    if (!collision.CompareTag(Main::Tag::playerAttack)) return;
    if (!immuneTimer) {
        immuneTimer = static_cast<Timer*>(_malloca(sizeof(Timer)));
        immuneTimer->Initialize();
        health -= 10.f;
        return;
    }
    if (immuneTimer->GetElapsedSeconds() < immuneTime) return;
    _freea(static_cast<void*>(immuneTimer));
    immuneTimer = nullptr;
}
void Enemy::Update(void) {
    if (!health) {
        EnemySpawner::DestroySwordGuy(enemySpawnNode);
        enabled = false;
        return;
    }
    healthBarEnt->SetRectPosition(static_cast<IntVec2>(GetPosition()) + healthBarOffset);
    healthBarEnt->SetAnimFrame(static_cast<int>(ceilf((healthBarEnt->GetNumAnimFrames(healthbar_animation) - 1) * GetHealthProportion())), healthbar_animation);
}