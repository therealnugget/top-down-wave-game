#include "bob.hpp"
#include "animations.hpp"
#include "player.hpp"
#include "EnemySpawner.hpp"
#include "math.hpp"
#include "camera.hpp"
//named this enemy bob for character efficency
//static const FVector2 bobSize = FVector2(86.f * 2.f, 86.f);//minimum size for 500 entites on screen
static constexpr FVector2 bobSize = FVector2(172.f * 2.f, 172.f);
constexpr static float border = .1f;
SwordGuy::SwordGuy(void) : attackDecide(false), attackDecision(Main::GetRandInt(overhead, swing, dash)), Enemy(SubRBData("sword guy", Animations::MakeAnimStrs(numSGAnims, idle, "idle", overhead, "overhead", swing, "swing", death, "death", dash, "dash", hurt, "hurt", run, "run", walk, "walk", jump, "jump"), FVector2(13.f / 128.f, 24.f / 64.f)* Physics::GetDefaultSquareVertVec(), /*Main::halfDisplaySize + FVector2::GetRight() * 350.f*/Main::GetRandFVec(static_cast<FVector2>(Camera::GetCamExtentWorld(1.f)) - Main::halfDisplaySize * border, static_cast<FVector2>(Camera::GetCamExtentWorld(-1.f)) + Main::halfDisplaySize * border), bobSize, std::initializer_list<FVector2>(), FVector2::Zero, IntVec2(-bobSize.x * .5f, -bobSize.y * .5f), Main::Tag::enemy, true, [this](Collision* collision) { CollisionCallback(collision); }, std::unordered_map<std::string, std::variant<FVector2, FVector2*>>(), std::unordered_map<std::string, bool>(), FVector2::Zero, .0, 1.f, true, false, { "right" }, true, 32.f / 128.f * bobSize.x, Main::Layer::enemyLayer)) {
    entity->SetAnimation(run);
    for (auto& anim : { hurt, overhead, swing, dash, death }) {
        entity->SetNotLoop(anim);
    }
    SetUpdateNode(Main::Updates += [this]() {Update();});
}
void SwordGuy::TakeDamage(float damageAmount) {
    Enemy::TakeDamage(damageAmount);
    if (curAnim != dash || entity->FirstFrame()) ResetIfAttkFin(hurt);
}
void SwordGuy::EnactDamage(void) {
    if (!turned) {
        Player::TakeDamage(damage);
        return;
    }
    auto closestEnemyBehav = EnemySpawner::closestEnemy;
    if (!closestEnemyBehav || closestEnemyBehav == this) return;
    closestEnemyBehav->TakeDamage(selfDamage);
}
void SwordGuy::ResetIfAttkFin(int animation) {
    if (animFinished) {
        if (curAnim == dash) {
            EnactDamage();
            rb->newPosition.x += dashPositionAdd * Math::Sign(toPlr.x);
        }
        if (curAnim == overhead || curAnim == swing) {
            EnactDamage();
            attackDecide = true;
        }
        entity->ResetAnim(animation);
        return;
    }
    entity->SetAnimation(animation);
}
static constexpr float yDiffDash = 30.f;
void SwordGuy::SetPlayerDist(void) {
    plrDistSqr = toPlr.SqrMagnitude();
}
void SwordGuy::CheckResetAttackDecision(void) {
    if (!attackDecide) return;
    attackDecide = false;
    register auto inc = (attackDecision + 1) % (dash + 1);
    attackDecision = inc + overhead * (inc == 0);
}
void SwordGuy::Update(void) {
    Enemy::Update();
    if (health <= .0f) {
        rb->SetVelocity(FVector2::Zero);
        entity->SetAnimation(death);
        enabled = false;
        if (entity->AnimFinished()) {
            EnemySpawner::DestroySwordGuy(enemySpawnNode);
        }
        return;
    }
    for (auto& col : colsOnFrame) col.second = false;
    curAnim = entity->GetCurAnim();
    animFinished = entity->AnimFinished();
    toPlr = rb->GetPosition().To(Player::GetPosition());
    SetPlayerDist();
    if (plrDistSqr < EnemySpawner::minPlrDist) {
        EnemySpawner::minPlrDist = plrDistSqr;
        EnemySpawner::closestEnemy = this;
    }
    if (isSingleEnemy && turned) {
        Main::LateUpdates -= lateUpdateNode;
        lateUpdateNode = nullptr;
        turned = false;
    }
    if (turned) {
        toPlr = GetPosition().To(EnemySpawner::closestEnemy->GetPosition());
        SetPlayerDist();
    }
    if (curAnim == hurt && !animFinished) return;
    if (curAnim == dash) {
        if (!animFinished) return;
        attackDecide = true;
    }
    if (plrDistSqr < dashDistSqr) {
        CheckResetAttackDecision();
        if (attackDecision == dash) {
            if (Math::abs(toPlr.y) < yDiffDash) {
                ResetIfAttkFin(attackDecision);
                goto ret;
            }
        }
        else if (plrDistSqr < attackDistSqr) {
        attack:
            ResetIfAttkFin(attackDecision);
            goto ret;
        }
    }
    else if (touchingEnemy) {
        CheckResetAttackDecision();
        goto attack;
    }
    ResetIfAttkFin(run);
    rb->AddForce(toPlr.Normalized() * speed * Main::DefCapDeltaTime());
ret:
    touchingEnemy = false;
    entity->SetFlip(toPlr.x < .0f);
}