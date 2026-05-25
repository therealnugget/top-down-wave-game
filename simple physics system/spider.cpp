#include "Spider.hpp"
#include "animations.hpp"
#include "player.hpp"
#include "EnemySpawner.hpp"
#include "math.hpp"
#include "camera.hpp"
static constexpr FVector2 spiderSize = FVector2(16.f * 2.f, 16.f * 2.f);
constexpr static float border = .1f;
float Spider::initSpeed;
Spider::Spider(void) : Enemy(SubRBData("spider", Animations::MakeAnimStrs(numBatAnims, idle, "idle", death, "death", hurt, "hurt", run, "run"), FVector2(16.f / 12.f, 12.f / 16.f)* Physics::GetDefaultSquareVertVec(), /*Main::halfDisplaySize + FVector2::GetRight() * 350.f*/Main::GetRandFVec(static_cast<FVector2>(Camera::GetCamExtentWorld(1.f)) - Main::halfDisplaySize * border, static_cast<FVector2>(Camera::GetCamExtentWorld(-1.f)) + Main::halfDisplaySize * border), spiderSize, std::initializer_list<FVector2>(), FVector2::Zero, IntVec2(-spiderSize.x * .5f, -spiderSize.y * .5f), Main::Tag::enemy, true, [this](Collision* collision) { CollisionCallback(collision); }, std::unordered_map<std::string, std::variant<FVector2, FVector2*>>(), std::unordered_map<std::string, bool>(), FVector2::Zero, .0, 1.f, true, false, { "right" }, true, 32.f / 128.f * spiderSize.x, Main::Layer::enemyLayer), IntVec2(7, -40), 5), avoidanceTimer(Timer(true)) {
    initSpeed = speed;
    entity->SetAnimation(run);
    for (auto& anim : { hurt, death }) {
        entity->SetNotLoop(anim);
    }
    SetUpdateNode(Main::Updates += [this]() {Update();});
    derivedTakeDamage = [this](float damageAmount) { TakeDamage(damageAmount); };
}
void Spider::TakeDamage(float damageAmount) {
    Enemy::TakeDamage(damageAmount);
    entity->SetAnimation(hurt);
}
void Spider::Update(void) {
    Enemy::Update();
    if (health <= .0f) {
        rb->SetVelocity(FVector2::Zero);
        entity->SetAnimation(death);
        enabled = false;
        if (entity->AnimFinished()) {
            EnemySpawner::DestroyEnemy(enemySpawnNode);
        }
        return;
    }
    if (curAnim == hurt && !animFinished) return;
    bool avoiding = plrDistSqr > avoid_dist_sqr;
    if (plrDistSqr < attack_dist_sqr) {
    attack:
        entity->SetAnimation(idle);
        goto ret;
    }
    if (avoiding) {
        if (avoidanceTimer.GetElapsedSeconds() > max_avoid_time) {
            speed = initSpeed * avoid_spd_multiplier;
        }
    }
    else if (lastFramePlrAvoid) {
        avoidanceTimer.Reset();
        speed = initSpeed;
    }
    if (touchingEnemy) {
        goto attack;
    }
    entity->SetAnimation(run);
    rb->AddForce(toPlr.Normalized() * speed * Main::DefCapDeltaTime());
ret:
    lastFramePlrAvoid = avoiding;
    touchingEnemy = false;
    entity->SetFlip(toPlr.x < .0f);
}