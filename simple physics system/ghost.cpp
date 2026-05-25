#include "ghost.hpp"
#include "animations.hpp"
#include "player.hpp"
#include "EnemySpawner.hpp"
#include "math.hpp"
#include "camera.hpp"
//static const FVector2 bobSize = FVector2(86.f * 2.f, 86.f);//minimum size for 500 entites on screen
static constexpr FVector2 ghostSize = FVector2(16.f * 2.f, 16.f * 2.f);
constexpr static float border = .1f;
float Ghost::initSpeed;
Ghost::Ghost(void) : Enemy(SubRBData("ghost", Animations::MakeAnimStrs(numGhostAnims, idle, "idle", death, "death", hurt, "hurt", run, "run"), FVector2(16.f / 12.f, 12.f / 16.f)* Physics::GetDefaultSquareVertVec(), /*Main::halfDisplaySize + FVector2::GetRight() * 350.f*/Main::GetRandFVec(static_cast<FVector2>(Camera::GetCamExtentWorld(1.f)) - Main::halfDisplaySize * border, static_cast<FVector2>(Camera::GetCamExtentWorld(-1.f)) + Main::halfDisplaySize * border), ghostSize, std::initializer_list<FVector2>(), FVector2::Zero, IntVec2(-ghostSize.x * .5f, -ghostSize.y * .5f), Main::Tag::enemy, true, [this](Collision* collision) { CollisionCallback(collision); }, std::unordered_map<std::string, std::variant<FVector2, FVector2*>>(), std::unordered_map<std::string, bool>(), FVector2::Zero, .0, 1.f, true, false, { "right" }, true, 32.f / 128.f * ghostSize.x, Main::Layer::enemyLayer), IntVec2(7, -40), 5), avoidanceTimer(Timer(speed_mul_teleport_time + .05f, true)), lastFramePlrClose(false) {
    static bool temp2 = true;
    temp |= temp2;
    temp2 = false;
    initSpeed = speed;
    entity->SetAnimation(run);
    for (auto& anim : { hurt, death }) {
        entity->SetNotLoop(anim);
    }
    SetUpdateNode(Main::Updates += [this]() {Update(); });
    derivedTakeDamage = [this](float damageAmount) { TakeDamage(damageAmount); };
}
void Ghost::TakeDamage(float damageAmount) {
    Enemy::TakeDamage(damageAmount);
    entity->SetAnimation(hurt);
}
void Ghost::Update(void) {
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
    bool bPlrClose = plrDistSqr < avoidanceDistSqr;
    if (entity->GetCurAnim() == death) {
        if (entity->AnimFinished()) {
            entity->SetAnimation(run);
            entity->SetAnimSpd(1.f);
        }
        goto ret;
    }
    if (curAnim == hurt && !animFinished) return;
    if (plrDistSqr < attackDistSqr) {
    attack:
        entity->SetAnimation(idle);
        goto ret;
    }
    if (lastFramePlrClose && !bPlrClose) {
        avoidanceTimer.Reset();
    }
    if (avoidanceTimer.GetElapsedSeconds() < speed_mul_teleport_time) speed = initSpeed * speed_teleport_factor;
    if (avoidanceTimer.GetElapsedSeconds() > max_avoid_time_teleport) {
        if (!Main::GetRandInt(0, random_spawn_one_in + 1)) {
            entity->SetAnimation(death);
            rb->SetPosition(Player::GetPosition() + Main::GetRandFVec(FVector2::One, FVector2::GetOne() * -1.f) * teleport_out_factor);
            entity->SetAnimSpd(-1.f);
            goto ret;
        }
        else avoidanceTimer.Reset();
    }
    if (touchingEnemy) {
        goto attack;
    }
    entity->SetAnimation(run);
    rb->AddForce(toPlr.Normalized() * speed * Main::DefCapDeltaTime());
ret:
    lastFramePlrClose = bPlrClose;
    touchingEnemy = false;
    entity->SetFlip(toPlr.x < .0f);
}