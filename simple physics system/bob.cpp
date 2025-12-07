#include "bob.hpp"
#include "animations.hpp"
#include "player.hpp"
#include "multicast delegates.hpp"
#include "math.hpp"
//named this enemy bob for character efficency
//static const FVector2 bobSize = FVector2(86.f * 2.f, 86.f);//minimum size for 500 entites on screen
static const FVector2 bobSize = FVector2(172.f * 2.f, 172.f);
constexpr static float border = .05f;
constexpr static float invBorder = 1.f - border;
SwordGuy::SwordGuy(): attackDecision(Main::GetRandInt(overhead, swing, dash)), decideAttack(true), Enemy(SubRBData("sword guy", Animations::MakeAnimStrs(numSGAnims, idle, "idle", overhead, "overhead", swing, "swing", death, "death", dash, "dash", hurt, "hurt", run, "run", walk, "walk", jump, "jump"), FVector2(13.f / 128.f, 24.f / 64.f)* Physics::GetDefaultSquareVertVec(), /*Main::halfDisplaySize + FVector2::GetRight() * 350.f*/Main::GetRandFVec(static_cast<const FVector2>(static_cast<FVector2>(Main::DisplaySize)* border), Main::DisplaySize* invBorder), bobSize, std::initializer_list<FVector2>(), FVector2::Zero, IntVec2(-bobSize.x * .5f, -bobSize.y * .5f), Main::Tag::enemy, [this](Collision& collision) { CollisionCallback(collision); }, std::unordered_map<std::string, std::variant<FVector2, FVector2*>>(), std::unordered_map<std::string, bool>(), FVector2::Zero, .0, 1.f, true, false, {"right"}, true, 32.f / 128.f * bobSize.x, Main::Layer::enemyLayer)) {
    entity->SetAnimation(run);
    for (auto& anim : { hurt, overhead, swing, dash }) {
        entity->SetNotLoop(anim);
    }
    SetUpdateNode(Main::Updates += [this]() {Update();});
    speed = 1900000.f;
}
void SwordGuy::CollisionCallback(Collision& collision) {
    if (entity->GetCurAnim() == hurt && !entity->AnimFinished() || !collision.CompareTag(Main::Tag::playerAttack) || colOnFrame) return;
    colOnFrame = true;
    if (curAnim != dash || entity->FirstFrame()) ResetIfAttkFin(hurt);
    rb->AddVelocity(collision.GetNormal() * Player::GetMouseDiff());
    health--;
}
void SwordGuy::ResetIfAttkFin(int animation) {
    if (animFinished) {
        if (curAnim == dash) {
            Player::TakeDamage();
            rb->newPosition.x += 100.f * Math::Sign(GetToPlr().x);
        }
        if (curAnim == overhead || curAnim == swing) {
            Player::TakeDamage();
            decideAttack = true;
        }
        entity->ResetAnim(animation);
        return;
    }
    entity->SetAnimation(animation);
}
static constexpr float yDiffDash = 30.f;
void SwordGuy::Update(void) {
    Enemy::Update();
    //important so that we don't try to run the rest of the update if the outer enemy has been deleted
    if (!enabled) return;
    toPlr = GetToPlr();
    curAnim  = entity->GetCurAnim();
    animFinished = entity->AnimFinished();
    if (curAnim == hurt && !animFinished) return;
    if (curAnim == dash) {
        if (!animFinished) return;
        decideAttack = true;
    }
    plrDistSqr = toPlr.SqrMagnitude();
    if (plrDistSqr < dashDistSqr) {
        if (decideAttack) {
            auto inc = (attackDecision + 1) % (dash + 1);
            attackDecision = inc + overhead * (inc == 0);
            decideAttack = false;
        }
        if (attackDecision == dash) {
            if (Math::abs(toPlr.y) < yDiffDash) {
                ResetIfAttkFin(dash);
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
    entity->SetFlip(GetToPlr().x < .0f);
}