#include "enemy.hpp"
static constexpr float default_max_health = 100.f;
Enemy::Enemy(SubRBData data): maxHealth(default_max_health), health(maxHealth), Behaviour(data) {

    auto healthBarSize = IntVec2(100.f, 50.f);
    healthBar = Physics::SubStandaloneEnt(Entity::MakeEntity("health bar", { "health_bar" }, Main::halfDisplaySize, healthBarSize, healthBarSize * (IntVec2::GetOne() + IntVec2::GetRight())));
    healthBarEnt = healthBar->value;
    healthBarEnt->DeActivate();
}
void Enemy::CollisionCallback(Collision &collision){
    if (!collision.CompareTag(Main::Tag::player)) return;
    //std::cout << "player is colliding with enemy\n";
}
void Enemy::Update(void) {
    healthBarEnt->SetRectPosition(static_cast<IntVec2>(GetPosition()) + healthBarOffset);
    healthBarEnt->SetAnimFrame(static_cast<int>(ceilf((healthBarEnt->GetNumAnimFrames(healthbar_animation) - 1) * GetHealthProportion())), healthbar_animation);
}