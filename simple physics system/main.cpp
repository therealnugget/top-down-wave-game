#include "main.hpp"
#include "winMgr.hpp"
#include "textures.hpp"
#include "player.hpp"
#include "math.hpp"
#include <sstream>
#include <iomanip>
#include <SDL_image.h>
#define SHIFT_INDEX 0
#define ALT_INDEX 1
#define CONTROL_INDEX 2
#define SHIFT_OFFSET NUM_SIG_SCANKEYS + SHIFT_INDEX
#define ALT_OFFSET NUM_SIG_SCANKEYS + ALT_INDEX
#define CTRL_OFFSET NUM_SIG_SCANKEYS + CONTROL_INDEX
static SDL_Event e;
const std::string Main::empty_string = "";
const char* const Main::empty_cc = empty_string.c_str();
bool Main::pressingKey[NUM_SIG_SCANKEYS];
bool Main::pressedKey[NUM_SIG_SCANKEYS];
bool Main::pressingModKey[NUM_MOD_KEYS];
bool Main::pressedModKey[NUM_MOD_KEYS];
static int currentKeyPressed;
static bool quit;
Node<int> *Main::setPressed;
Behaviour::Behaviour() {

}
void Main::SetPastKey(int *i) {
    if (*i >= NUM_SIG_SCANKEYS) {
        int ind = *i - NUM_SIG_SCANKEYS;
        pressedModKey[ind] = pressingModKey[ind];
        return;
    }
    pressedKey[*i] = pressingKey[*i];
}
static int currentMod;
bool Main::fullScreen;
FVector2 Main::fInputVec;
FVector2 Main::fInputVec2;
IntVec2 Main::iInputVec;
bool Main::dirKeyPress[num_inp_dirs];
bool Main::getDirKey[num_inp_dirs];
bool Main::dirKeyUp[num_inp_dirs];
bool Main::processedKey[num_inp_dirs];
bool Main::cancelKey[num_inp_dirs];
int Main::cancelOpKey[num_inp_dirs];
bool Main::keyDownHorizon, Main::keyDownVert;
bool Main::cancelH, Main::cancelV;
void Main::RegisterInput() {
    while (SDL_PollEvent(&e) > 0) {
        switch (e.type) {
        case SDL_WINDOWEVENT:
            switch (e.window.event) {
            case SDL_WINDOWEVENT_SIZE_CHANGED:
            case SDL_WINDOWEVENT_EXPOSED:
                SDL_RenderPresent(renderer);
                return;
            }
            return;
        default:
            currentMod = e.key.keysym.mod;
            if ((currentKeyPressed = e.key.keysym.scancode) < NUM_SIG_SCANKEYS && currentKeyPressed >= 0) {
                pressingKey[currentKeyPressed] = e.type == SDL_KEYDOWN;
                Node<int>::AddAtHead(currentKeyPressed, &setPressed);
            }
            if (pressingModKey[SHIFT_INDEX] = currentMod & KMOD_SHIFT) AddKeyPress(SHIFT_OFFSET);
            if (pressingModKey[CONTROL_INDEX] = currentMod & KMOD_CTRL) AddKeyPress(CTRL_OFFSET);
            if (pressingModKey[ALT_INDEX] = currentMod & KMOD_ALT) AddKeyPress(ALT_OFFSET);
            if (Main::ModKeyHeld(ALT_INDEX) && KeyPressed(SDL_SCANCODE_RETURN)) {
                bool curFullScreenState = !fullScreen;
                SDL_SetWindowFullscreen(WindowManager::window, SDL_WINDOW_FULLSCREEN * (fullScreen = curFullScreenState) + SDL_WINDOW_MAXIMIZED * curFullScreenState);
            }
            quit |= e.type == SDL_QUIT || (Main::ModKeyHeld(CONTROL_INDEX) && KeyPressed(SDL_SCANCODE_Q));
            return;
        }
    }
}
void Main::AssignDirKeyFromInfo(bool *assign, int dir, int key1, int key2, bool (*keyInfo)(int)) {
    assign[dir] = keyInfo(key1) || keyInfo(key2);
}
bool Main::GetOpAxisVal(bool h, bool v, bool dirIsHorizon) {
    //get value according to the opposing axis (i.e. if dir inputted is a horizontal dir, the output is the value of vertical, and vica versa.)
    return h * !dirIsHorizon + v * dirIsHorizon;
}
void Main::SetKeyState(int dir) {
    processedKey[dir] = getDirKey[dir] && !GetOpAxisVal(cancelH, cancelV, dir);
}
void Main::SetCancelState(int dir) {
    const bool dirIsHorizon = dir & 2;
    const bool opposingInput = GetOpAxisVal(keyDownHorizon, keyDownVert, dirIsHorizon) && dirKeyPress[dir];
    cancelKey[dir] = (cancelKey[dir] || opposingInput) && !dirKeyUp[dir] && !dirKeyUp[cancelOpKey[dir]];
    if (!opposingInput) return;
    int opKeyOffset = !dirIsHorizon * 2;
    int dir1 = input_down + opKeyOffset;
    if (getDirKey[dir1]) {
        cancelOpKey[dir] = dir1;
        return;
    }
    cancelOpKey[dir] = input_up + opKeyOffset;
}
void Main::SetAxisBool(bool& horizonB, bool& vertB, bool dirBools[num_inp_dirs]) {
    horizonB = dirBools[input_left] || dirBools[input_right];
    vertB = dirBools[input_down] || dirBools[input_up];
}
static int currentDir;
//this contains the behaviour for assigning which key was pressed in the frame, thus it should be called before any other behaviours.
void Main::EarlyUpdate() {
    RegisterInput();
    SDL_RenderClear(renderer);
    AssignDirKey(input_right, SDL_SCANCODE_RIGHT, SDL_SCANCODE_D);
    AssignDirKey(input_up, SDL_SCANCODE_UP, SDL_SCANCODE_W);
    AssignDirKey(input_left, SDL_SCANCODE_LEFT, SDL_SCANCODE_A);
    AssignDirKey(input_down, SDL_SCANCODE_DOWN, SDL_SCANCODE_S);
    SetAxisBool(keyDownHorizon, keyDownVert, getDirKey);
    for (currentDir = input_first; currentDir < input_last + 1; currentDir++) {
        SetCancelState(currentDir);
    }
    SetAxisBool(cancelH, cancelV, cancelKey);
    for (currentDir = input_first; currentDir < input_last + 1; currentDir++) {
        SetKeyState(currentDir);
    }
    //inverted y
    iInputVec = IntVec2(processedKey[input_right] - processedKey[input_left], processedKey[input_down] - processedKey[input_up]);
    //uncomment if needed
    //fInputVec2 = FVector2(GetKey(SDL_SCANCODE_L) - GetKey(SDL_SCANCODE_J), GetKey(SDL_SCANCODE_K) - GetKey(SDL_SCANCODE_I)).Normalized();
    fInputVec = FVector2(iInputVec);
}
bool Main::CheckPauseState() {
    if (!Main::KeyPressed(SDL_SCANCODE_ESCAPE)) return false;
    Main::timeScale = (Main::timeScale == 1.f) ? .0f : 1.f;
    return true;
}
void Main::ClearInput() {
    Node<int>::RemoveAllNodes(&setPressed, SetPastKey);
}
//called after all behaviour scripts because it calls "sdl_renderpresent" and adds more key functionality
void Main::LateUpdate() {
    SDL_RenderPresent(renderer);
    ClearInput();
}
//can't have a proper finalizer for a static class
void Main::Finalize() {
    SDL_DestroyRenderer(renderer);
    renderer = nullptr;
}
//per-component randomness.
FVector2 Main::GetRandFVec(FVector2 min, FVector2 max) {
    return { GetRandFloat(min.x, max.x), GetRandFloat(min.y, max.y) };
}
static float frequency;
//this is quite slow; not because of the float cast creating a new float obj, but rather the getperformancecounter(). for this reason, if you are on an absolute potato and are calling this function a lot of times in a frame without multithreading for behaviours, store the result at the start of the frame and reuse it (obviously unless you want it to be precise, but that's impractical for, for insance, movement code.)
float Main::DeltaTime() {
    return static_cast<float>(SDL_GetPerformanceCounter() - pastTime) / frequency * timeScale;
}
static void Close() {
    Physics::Finalize();
    Main::Finalize();
    SDL_DestroyWindow(WindowManager::window);
    WindowManager::window = nullptr;
    SDL_Quit();
}
Uint64 Main::pastTime;
SDL_Renderer* Main::renderer = nullptr;
//display mode
SDL_DisplayMode Main::DM;
FVector2 Main::DisplaySize;
FVector2 Main::halfDisplaySize;
FVector3 Main::renderDrawColor = FVector3(163.f, 215.f, 245.f);
std::vector<const char*> Main::dirPaths;
void Main::Start() {
    SDL_GetCurrentDisplayMode(0, &DM);//if we start using multiple windows, this needs to be changed
    DisplaySize = FVector2(DM);
    halfDisplaySize = DisplaySize / 2.f;
    WindowManager::window = SDL_CreateWindow("Hello. My name is Kevin, I have changnesia.", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, Main::DM.w, Main::DM.h, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED);
    if (!WindowManager::window) {
        ThrowError("window could not initialize: ", SDL_GetError());
        return;
    }
    renderer = SDL_CreateRenderer(WindowManager::window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    //SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    //auto GetCol = [](int ind) -> float {return std::get<ind>(static_cast<std::tuple<float, float, float>>(renderDrawColor)); };
    SDL_SetRenderDrawColor(renderer, std::get<0>(renderDrawColor), std::get<1>(renderDrawColor), std::get<2>(renderDrawColor), 255);//above code doesn't work because compiler is stupid
    if (!renderer) {
        ThrowError("renderer couldn't be created");
    }
    if ((IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) != IMG_INIT_PNG) {
        ThrowError("couldn't init img: ", IMG_GetError());
    }
    dirPaths.resize(static_cast<const size_t>(num_directions));
    dirPaths[left] = "left";
    dirPaths[up] = "up";
    dirPaths[down] = "down";
    dirPaths[right] = "right";
}
#ifndef NULL_PTR_REMOVE_FUNC
#define NULL_PTR_REMOVE_FUNC
template <>
Node<void*>* Node<void*>::Remove(Node<void*>** head, Node<void*>* remove, bool freeNode) {
    Node<void*>* next = remove->next, * prev = remove->prev;
    if (next) next->prev = prev;
    if (prev) prev->next = next;
    else {
        *head = next;
    }
    if (!freeNode) return remove;
    delete remove->value;
    delete remove;
    //i'm pretty sure it returns nullptr even if you don't return, but it's better practice this way.
    return nullptr;
}
#endif
float Main::timeScale = 1.f;
//dt updates are called before updates
MultiDelegate<float> Main::dtUpdates;
//regular (no argument) updates are called after dt updates
MultiDelegate<void> Main::Updates;
constexpr static int afk_sleep_time = 16;
int main(int argc, char* args[])
{
    {
        srand(SDL_GetPerformanceCounter());
        rand();
        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            //i used to encapsulate returns within definitions to preserve the number of characters (i.e. so i wouldn't have to write "return" every time), but over time i realised that in the long run it's less efficient overall because it takes much more time to debug the code. I think terry davis got it right with only using definitions without args.
            ThrowError("video could not initialize: ", SDL_GetError());
            return 1;
        }
    }
    Main::Start();
    WindowManager::screenSurface = SDL_GetWindowSurface(WindowManager::window);
    {
        constexpr uint8_t maxRGBAVal = 255;
        SDL_FillRect(WindowManager::screenSurface, NULL, SDL_MapRGBA(WindowManager::screenSurface->format, maxRGBAVal, maxRGBAVal, maxRGBAVal, 0));
        frequency = static_cast<float>(SDL_GetPerformanceFrequency());
    }
    Player::Init();
    Physics::Init();
    double tempDTCumulative = .0;
    uint tempDTIndex = 0;
    Main::StartDTCounter();
    do {
        if (Main::timeScale == .0f) {
            Main::RegisterInput();
            Main::CheckPauseState();
        pause_screen:
            Main::ClearInput();
            SDL_Delay(afk_sleep_time);
            continue;
        }
        //all normal behaviours after early update and before physics::update().
        Main::EarlyUpdate();
        if (Main::CheckPauseState()) goto pause_screen;
        Main::Updates();
        Main::dtUpdates(Main::DeltaTime());
#define SHOW_FPS
#ifdef SHOW_FPS
        //cout << 1.f / Main::DeltaTime() << '\n';
        printf("fps: %f\n", 1.f / Main::DeltaTime());
        tempDTCumulative += static_cast<double>(Main::DeltaTime());
        tempDTIndex++;
#endif
        Main::LateUpdate();
        Main::StartDTCounter();
        SDL_Delay(1);
    } while (!quit);
#ifdef SHOW_FPS
    float averageDT = tempDTCumulative / static_cast<double>(tempDTIndex);
    cout << "average delta time over life of program is " << averageDT << ", thus average fps is " << 1.f / averageDT << '\n';
#endif
    Close();
    return 0;
}