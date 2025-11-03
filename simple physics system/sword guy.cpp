#include "sword guy.hpp"
#include "animations.hpp"
Orc::Orc() : Enemy(SubRBData("sword guy", Animations::MakeAnimStrs(numOrcAnims, idle, "idle", overhead, "overhead", swing, "swing", death, "death", dash, "dash", hurt, "hurt", run, "run", walk, "walk", jump, "jump"), FVector2(17.f / 64.f, 24.f / 64.f)* Physics::GetDefaultSquareVertVec(), Main::halfDisplaySize + FVector2::GetRight() * 350.f, FVector2::GetOne() * 300.f, std::initializer_list<FVector2>(), FVector2::Zero, FVector2(-150.f, -300.f * 39.f / 64.f), -1, nullptr, std::unordered_map<const char*, std::variant<FVector2, FVector2*>>(), std::unordered_map<const char *, bool>(), FVector2::Zero, .0, 1.f, true, false, {"right"})) {
    Main::Updates += [this]() { Update(); };
}
void Orc::Update(void) {

}