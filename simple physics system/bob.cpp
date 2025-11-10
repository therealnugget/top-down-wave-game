#include "bob.hpp"
#include "animations.hpp"
#include "player.hpp"
#include "multicast delegates.hpp"
//named this enemy bob for character efficency
static const FVector2 bobSize = FVector2(300.f * 2.f, 300.f);
constexpr static float border = .05f;
constexpr static float invBorder = 1.f - border;
SwordGuy::SwordGuy() : Enemy(SubRBData("sword guy", Animations::MakeAnimStrs(numSGAnims, idle, "idle", overhead, "overhead", swing, "swing", death, "death", dash, "dash", hurt, "hurt", run, "run", walk, "walk", jump, "jump"), FVector2(17.f / 64.f, 24.f / 64.f)* Physics::GetDefaultSquareVertVec(), /*Main::halfDisplaySize + FVector2::GetRight() * 350.f*/Main::GetRandFVec(static_cast<const FVector2>(static_cast<FVector2>(Main::DisplaySize)* border), Main::DisplaySize* invBorder), bobSize, std::initializer_list<FVector2>(), FVector2::Zero, FVector2(-bobSize.x * .5f, -300.f * 39.f / 64.f), Main::Tag::enemy, nullptr, std::unordered_map<std::string, std::variant<FVector2, FVector2*>>(), std::unordered_map<std::string, bool>(), FVector2::Zero, .0, 1.f, true, false, { "right" }, true, 32.f / 128.f * bobSize.x)) {
    SetUpdateNode(Main::Updates += [this]() {Update();});
    PlayAnimation(run);
    speed = 3000.f;
    auto healthBarSize = IntVec2(100.f, 50.f);
    healthBar = Physics::SubStandaloneEnt(Entity::MakeEntity("health bar", { "health_bar" }, Main::halfDisplaySize, healthBarSize, healthBarSize * (IntVec2::GetOne() + IntVec2::GetRight())));
    healthBarEnt = healthBar->value;
    healthBarEnt->DeActivate();
}
void SwordGuy::Update(void) {
    auto pos = GetPosition(), playerPos = Player::GetPosition();
    healthBarEnt->SetRectPosition(static_cast<IntVec2>(pos) + healthBarOffset);
    AddPosition(FVector2::FromTo(pos, playerPos).Normalized() * speed * Main::DeltaTime());
    SetFlipX(pos.x > playerPos.x);
}