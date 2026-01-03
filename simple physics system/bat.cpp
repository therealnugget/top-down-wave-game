#include "bat.hpp"
#include "animations.hpp"
#include "player.hpp"
#include "EnemySpawner.hpp"
#include "math.hpp"
#include "camera.hpp"
//named this enemy bob for character efficency
//static const FVector2 bobSize = FVector2(86.f * 2.f, 86.f);//minimum size for 500 entites on screen
static constexpr FVector2 batSize = FVector2(16.f * 2.f, 16.f * 2.f);
constexpr static float border = .1f;
Bat::Bat(void) : Enemy(SubRBData("bat", Animations::MakeAnimStrs(numBatAnims, idle, "idle", death, "death", hurt, "hurt", run, "run"), FVector2(16.f / 12.f, 12.f / 16.f)* Physics::GetDefaultSquareVertVec(), /*Main::halfDisplaySize + FVector2::GetRight() * 350.f*/Main::GetRandFVec(static_cast<FVector2>(Camera::GetCamExtentWorld(1.f)) - Main::halfDisplaySize * border, static_cast<FVector2>(Camera::GetCamExtentWorld(-1.f)) + Main::halfDisplaySize * border), batSize, std::initializer_list<FVector2>(), FVector2::Zero, IntVec2(-batSize.x * .5f, -batSize.y * .5f), Main::Tag::enemy, true, [this](Collision* collision) { CollisionCallback(collision); }, std::unordered_map<std::string, std::variant<FVector2, FVector2*>>(), std::unordered_map<std::string, bool>(), FVector2::Zero, .0, 1.f, true, false, { "right" }, true, 32.f / 128.f * batSize.x, Main::Layer::enemyLayer), IntVec2(7, -40), 5) {
    entity->SetAnimation(run);
    for (auto& anim : { hurt, death }) {
        entity->SetNotLoop(anim);
    }
    SetUpdateNode(Main::Updates += [this]() {Update();});
    derivedTakeDamage = [this](float damageAmount) { TakeDamage(damageAmount); };
}
void Bat::TakeDamage(float damageAmount) {
    Enemy::TakeDamage(damageAmount);
    entity->SetAnimation(hurt);
}
void Bat::Update(void) {
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
    if (plrDistSqr < attackDistSqr) {
    attack:
        entity->SetAnimation(idle);
        goto ret;
    }
    else if (touchingEnemy) {
        goto attack;
    }
    entity->SetAnimation(run);
    rb->AddForce(toPlr.Normalized() * speed * Main::DefCapDeltaTime());
ret:
    touchingEnemy = false;
    entity->SetFlip(toPlr.x < .0f);
}