#include "bob.hpp"
#include "animations.hpp"
#include "player.hpp"
#include "multicast delegates.hpp"
//named this enemy bob for character efficency
static const FVector2 bobSize = FVector2(86.f * 2.f, 86.f);
constexpr static float border = .05f;
constexpr static float invBorder = 1.f - border;
SwordGuy::SwordGuy() : Enemy(SubRBData("sword guy", Animations::MakeAnimStrs(numSGAnims, idle, "idle", overhead, "overhead", swing, "swing", death, "death", dash, "dash", hurt, "hurt", run, "run", walk, "walk", jump, "jump"), FVector2(13.f / 128.f, 24.f / 64.f) * Physics::GetDefaultSquareVertVec(), /*Main::halfDisplaySize + FVector2::GetRight() * 350.f*/Main::GetRandFVec(static_cast<const FVector2>(static_cast<FVector2>(Main::DisplaySize)* border), Main::DisplaySize* invBorder), bobSize, std::initializer_list<FVector2>(), FVector2::Zero, IntVec2(-bobSize.x * .5f, -bobSize.y * .5f), Main::Tag::enemy, [this](Collision& collision) { CollisionCallback(collision); }, std::unordered_map<std::string, std::variant<FVector2, FVector2*>>(), std::unordered_map<std::string, bool>(), FVector2::Zero, .0, 1.f, true, false, { "right" }, true, 32.f / 128.f * bobSize.x, Main::Layer::enemyLayer)) {
    SetUpdateNode(Main::Updates += [this]() {Update();});
    PlayAnimation(run);
    speed = 150.f;
}
void SwordGuy::Update(void) {
    Enemy::Update();    
    if (!enabled) return;
    auto pos = GetPosition(), playerPos = Player::GetPosition();
    AddPosition(FVector2::FromTo(pos, playerPos).Normalized() * speed * Main::DefCapDeltaTime());
    SetFlipX(pos.x > playerPos.x);
}