#include "bob.hpp"
#include "animations.hpp"
#include "player.hpp"
//named this enemy bob for character efficency
static const FVector2 bobSize = FVector2(300.f * 2.f, 300.f);
SwordGuy::SwordGuy() : Enemy(SubRBData("sword guy", Animations::MakeAnimStrs(numSGAnims, idle, "idle", overhead, "overhead", swing, "swing", death, "death", dash, "dash", hurt, "hurt", run, "run", walk, "walk", jump, "jump"), FVector2(17.f / 64.f, 24.f / 64.f)* Physics::GetDefaultSquareVertVec(), Main::halfDisplaySize + FVector2::GetRight() * 350.f, bobSize, std::initializer_list<FVector2>(), FVector2::Zero, FVector2(-bobSize.x * .5f, -300.f * 39.f / 64.f), -1, nullptr, std::unordered_map<const char*, std::variant<FVector2, FVector2*>>(), std::unordered_map<const char*, bool>(), FVector2::Zero, .0, 1.f, true, false, { "right" }, true, 32.f / 128.f * bobSize.x)) {
    Main::Updates += [this]() { Update(); };
    PlayAnimation(run);
    speed = 3000.f;
}
void SwordGuy::Update(void) {
    auto pos = GetPosition(), playerPos = Player::GetPosition();
    AddPosition(FVector2::FromTo(pos, playerPos).Normalized() * speed/* * Main::DeltaTime()*/ * 1/600.f);
    SetFlipX(pos.x > playerPos.x);
}