#include "enemy.hpp"
#include "EnemySpawner.hpp"
static constexpr int default_max_health = 3;
static constexpr float default_immune_time = .6f;
Enemy::Enemy(SubRBData data): health(default_max_health), enabled(true), Behaviour(data) {
}
Enemy::~Enemy() {
}
//it's not as simple as deleting the enemy as soon as the collision callback is called. if you think about it, the collision callback is in the middle of the physics update. so we will be deleting a rigidbody in the middle of the narrow phase function, then trying to access it later on in that same function, and then again in the physics update. so the closest thing we can do is:
//1: if we need the enemy to appear as though he is deleted as soon as the collision occurs, delete the entity, and destroy the outer behaviour at the start of the next frame
// or 2: if we don't care about showing the enemy for the single frame in which he is, in this example, dead, then we can simply destroy the enemy at the start of the next frame as we've done here.
void Enemy::Update(void) {
    if (!health) {
        EnemySpawner::DestroySwordGuy(enemySpawnNode);
        enabled = false;
        return;
    }
}