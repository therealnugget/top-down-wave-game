#include "bob.hpp"
#include "animations.hpp"
#include "player.hpp"
#include "multicast delegates.hpp"
//named this enemy bob for character efficency
//static const FVector2 bobSize = FVector2(86.f * 2.f, 86.f);//minimum size for 500 entites on screen
static const FVector2 bobSize = FVector2(172.f * 2.f, 172.f);
constexpr static float border = .05f;
constexpr static float invBorder = 1.f - border;
SwordGuy::SwordGuy() : Enemy(SubRBData("sword guy", Animations::MakeAnimStrs(numSGAnims, idle, "idle", overhead, "overhead", swing, "swing", death, "death", dash, "dash", hurt, "hurt", run, "run", walk, "walk", jump, "jump"), FVector2(13.f / 128.f, 24.f / 64.f) * Physics::GetDefaultSquareVertVec(), /*Main::halfDisplaySize + FVector2::GetRight() * 350.f*/Main::GetRandFVec(static_cast<const FVector2>(static_cast<FVector2>(Main::DisplaySize)* border), Main::DisplaySize* invBorder), bobSize, std::initializer_list<FVector2>(), FVector2::Zero, IntVec2(-bobSize.x * .5f, -bobSize.y * .5f), Main::Tag::enemy, [this](Collision& collision) { CollisionCallback(collision); }, std::unordered_map<std::string, std::variant<FVector2, FVector2*>>(), std::unordered_map<std::string, bool>(), FVector2::Zero, .0, 1.f, true, false, { "right" }, true, 32.f / 128.f * bobSize.x, Main::Layer::enemyLayer)) {
    entity->SetAnimation(run);
    SetUpdateNode(Main::Updates += [this]() {Update();});
    speed = 150.f;
}
void SwordGuy::CollisionCallback(Collision& collision) {
    if (!collision.CompareTag(Main::Tag::playerAttack) || entity->GetCurAnim() == hurt) return;
    entity->SetAnimation(hurt);
    health--;
}
void SwordGuy::Update(void) {
    Enemy::Update();
    //important so that we don't try to run the rest of the update if the outer enemy has been deleted
    if (!enabled) return;
    auto pos = rb->position, playerPos = Player::GetPosition();
    rb->position += FVector2::FromTo(pos, playerPos).Normalized() * speed * Main::DefCapDeltaTime();
    SetFlipX(pos.x > playerPos.x);
    if (entity->GetCurAnim() == hurt && entity->AnimFinished()) entity->SetAnimation(run);
}