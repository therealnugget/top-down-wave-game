#include "bob.hpp"
#include "animations.hpp"
#include "player.hpp"
#include "EnemySpawner.hpp"
#include "math.hpp"
#include "camera.hpp"
//named this enemy bob for character efficency
//static const FVector2 bobSize = FVector2(86.f * 2.f, 86.f);//minimum size for 500 entites on screen
static const FVector2 bobSize = FVector2(172.f * 2.f, 172.f);
constexpr static float border = .1f;
SwordGuy::SwordGuy(): lateUpdateNode(nullptr), selfDamage(.5f), damage(1.f), attackDecision(Main::GetRandInt(overhead, swing, dash)), decideAttack(true), Enemy(SubRBData("sword guy", Animations::MakeAnimStrs(numSGAnims, idle, "idle", overhead, "overhead", swing, "swing", death, "death", dash, "dash", hurt, "hurt", run, "run", walk, "walk", jump, "jump"), FVector2(13.f / 128.f, 24.f / 64.f)* Physics::GetDefaultSquareVertVec(), /*Main::halfDisplaySize + FVector2::GetRight() * 350.f*/Main::GetRandFVec(static_cast<FVector2>(Camera::GetCamExtentWorld(1.f)) - Main::halfDisplaySize * border, static_cast<FVector2>(Camera::GetCamExtentWorld(-1.f)) + Main::halfDisplaySize * border), bobSize, std::initializer_list<FVector2>(), FVector2::Zero, IntVec2(-bobSize.x * .5f, -bobSize.y * .5f), Main::Tag::enemy, true, [this](Collision* collision) { CollisionCallback(collision); }, std::unordered_map<std::string, std::variant<FVector2, FVector2*>>(), std::unordered_map<std::string, bool>(), FVector2::Zero, .0, 1.f, true, false, {"right"}, true, 32.f / 128.f * bobSize.x, Main::Layer::enemyLayer)) {
    entity->SetAnimation(run);
    for (auto& anim : { hurt, overhead, swing, dash, death }) {
        entity->SetNotLoop(anim);
    }
    SetUpdateNode(Main::Updates += [this]() {Update();});
    speed = 19000000.f;
}
void SwordGuy::TakeDamage(float damage) {
    if (curAnim != dash || entity->FirstFrame()) ResetIfAttkFin(hurt);
    health -= damage;
}
void SwordGuy::EnactDamage(void) {
    if (!turned) {
        Player::TakeDamage(damage);
        return;
    }
    if (!closestEnemyBehav) return;
    closestEnemyBehav->TakeDamage(selfDamage);
}
void SwordGuy::LateUpdate(void) {
    confusedTex.SetPosition(GetConfusedPos());
    Textures::RenderStandaloneTex(confusedTex);
}
void SwordGuy::CollisionCallback(Collision* collision) {
    if (!enabled) return;
    if (!plrColOnFrm && collision->CompareTag(Main::Tag::player)) {
        Player::TakeDamage(damage);
        plrColOnFrm = true;
    }
    if (!colOnFrame && collision->CompareTag(Main::Tag::enemyTurner)) {
        if (!turned) {
            auto confusedPos = GetConfusedPos();
            confusedTex = Textures::TextureRect(Textures::InitAnim(confusedPath), SDL_Rect{ confusedPos.x, confusedPos.y, confusedSize.x, confusedSize.y });
            lateUpdateNode = (Main::LateUpdates += [this]() {LateUpdate(); });
        }
        turned = true;
        rb->tag = Main::Tag::playerAttack;
    }
    if (colOnFrame || entity->GetCurAnim() == hurt && !entity->AnimFinished() || !collision->CompareTag(Main::Tag::playerAttack)) return;
    colOnFrame = true;
    TakeDamage(Player::GetDamage());
    rb->AddVelocity(collision->GetNormal() * Player::GetMouseDiff());
}
void SwordGuy::ResetIfAttkFin(int animation) {
    if (animFinished) {
        if (curAnim == dash) {
            Player::TakeDamage(damage);
            rb->newPosition.x += dashPositionAdd * Math::Sign(toPlr.x);
        }
        if (curAnim == overhead || curAnim == swing) {
            Player::TakeDamage(damage);
            decideAttack = true;
        }
        entity->ResetAnim(animation);
        return;
    }
    entity->SetAnimation(animation);
}
static constexpr float yDiffDash = 30.f;
FVector2 SwordGuy::closestEnemyPos = {};
SwordGuy *SwordGuy::closestEnemyBehav = nullptr;
void SwordGuy::SetPlayerDist(void) {
    plrDistSqr = toPlr.SqrMagnitude();
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
    plrColOnFrm = false;
    curAnim = entity->GetCurAnim();
    animFinished = entity->AnimFinished();
    toPlr = rb->GetPosition().To(Player::GetPosition());
    SetPlayerDist();
    if (plrDistSqr < EnemySpawner::minPlrDist) {
        EnemySpawner::minPlrDist = plrDistSqr;
        closestEnemyPos = GetPosition();
        closestEnemyBehav = this;
    }
    if (turned) {
        toPlr = GetPosition().To(closestEnemyPos);
        SetPlayerDist();
    }
    if (curAnim == hurt && !animFinished) return;
    if (curAnim == dash) {
        if (!animFinished) return;
        decideAttack = true;
    }
    if (plrDistSqr < dashDistSqr) {
        if (decideAttack) {
            auto inc = (attackDecision + 1) % (dash + 1);
            attackDecision = inc + overhead * (inc == 0);
            decideAttack = false;
        }
        if (attackDecision == dash) {
            if (Math::abs(toPlr.y) < yDiffDash) {
                ResetIfAttkFin(attackDecision);
                goto ret;
            }
        }
        else if (plrDistSqr < attackDistSqr) {
            ResetIfAttkFin(attackDecision);
            goto ret;
        }
    }
    ResetIfAttkFin(run);
    rb->AddForce(toPlr.Normalized() * speed * Main::DefCapDeltaTime());
ret:
    entity->SetFlip(toPlr.x < .0f);
}