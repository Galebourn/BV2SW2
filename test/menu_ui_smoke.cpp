#include <SDL.h>
#include <cassert>
#include <memory>
#include <string>
#include "BlobbyApp.h"
#include "SoundManager.h"
#include "state/OptionsState.h"
#include "FileSystem.h"
#include "UserConfig.h"
#include "SpeedController.h"
#include "IMGUI.h"
#include "state/State.h"

static void key(SDL_Keycode code)
{
    SDL_Event event{};
    event.type = SDL_KEYDOWN;
    event.key.keysym.sym = code;
    SDL_PushEvent(&event);
}
static void touch(Uint32 type, float x, float y)
{
    SDL_Event event{};
    event.type = type;
    event.tfinger.fingerId = 42;
    event.tfinger.x = x / 800.0f;
    event.tfinger.y = y / 600.0f;
    SDL_PushEvent(&event);
}
static void shot(BlobbyApp& app, const char* path)
{
    SDL_Surface* surface = SDL_CreateRGBSurfaceWithFormat(0,800,600,32,SDL_PIXELFORMAT_ARGB8888);
    assert(surface);
    assert(SDL_RenderReadPixels(SDL_GetRenderer(app.getWindow()),nullptr,surface->format->format,surface->pixels,surface->pitch)==0);
    assert(SDL_SaveBMP(surface,path)==0);
    SDL_FreeSurface(surface);
}
int main(int argc, char** argv)
{
    (void)argc;
    SDL_SetHint(SDL_HINT_RENDER_DRIVER,"software");
    assert(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO|SDL_INIT_JOYSTICK)==0);
    FileSystem fs(argv[0]);
    fs.setWriteDir("/tmp/bv2-menu-test");
    fs.addToSearchPath("data");
    UserConfig config;
    config.loadFile("config.xml");
    SpeedController speed(75);
    SpeedController::setMainInstance(&speed);
    BlobbyApp app(std::unique_ptr<State>(new MainMenuState()),config);
    auto& gui=app.getIMGUI();
    auto& renderer=app.getRenderManager();
    renderer.refresh();
    bool yes=false,no=false;
    auto dialog=[&]() {
        app.getInputManager().updateInput(); gui.begin();
        gui.doOverlay(1,Vector2(0,0),Vector2(800,600));
        gui.doText(2,Vector2(400,200),"DO YOU WANT TO QUIT?",TF_ALIGN_CENTER);
        yes=gui.doButton(10,Vector2(280,300),"YES");
        no=gui.doButton(20,Vector2(440,300),"NO");
        gui.end(renderer); renderer.refresh();
    };
    dialog(); key(SDLK_RIGHT); dialog(); key(SDLK_RETURN); dialog();
    assert(no && !yes);
    key(SDLK_LEFT); dialog(); key(SDLK_RETURN); dialog();
    assert(yes && !no);
    shot(app,"/tmp/bv2-menu-test/dialog.bmp");

    gui.resetSelection();
    unsigned selected=0; float volume=0.5f; bool saved=false;
    const std::vector<std::string> entries{"HUMAN","EASY BOT","MEDIUM BOT","HARD BOT"};
    auto options=[&]() {
        app.getInputManager().updateInput(); gui.begin();
        gui.doOverlay(1,Vector2(0,0),Vector2(800,600));
        gui.doText(2,Vector2(40,25),"PLAYER");
        gui.doText(3,Vector2(430,25),"VOLUME");
        gui.doSelectbox(10,Vector2(40,70),Vector2(360,260),entries,selected);
        gui.doScrollbar(20,Vector2(430,70),volume);
        saved=gui.doButton(30,Vector2(430,210),"SAVE");
        gui.end(renderer); renderer.refresh();
    };
    options(); key(SDLK_RETURN); options(); key(SDLK_DOWN); options();
    assert(selected==1);
    key(SDLK_ESCAPE); options(); key(SDLK_RIGHT); options();
    assert(selected==1); // Right changes focus, not a list's vertical value.
    key(SDLK_RETURN); options(); key(SDLK_RIGHT); options();
    assert(volume>0.59f && volume<0.61f);
    key(SDLK_ESCAPE); options(); key(SDLK_DOWN); options();
    key(SDLK_RETURN); options(); assert(saved);
    shot(app,"/tmp/bv2-menu-test/options.bmp");
    touch(SDL_FINGERDOWN,460,220); options(); assert(saved);
    touch(SDL_FINGERUP,460,220); options();
    touch(SDL_FINGERDOWN,80,145); options(); assert(selected==2);
    touch(SDL_FINGERUP,80,145); options();
    touch(SDL_FINGERDOWN,530,80); options();
    touch(SDL_FINGERMOTION,610,80); options();
    assert(volume>0.89f && volume<0.91f);
    touch(SDL_FINGERUP,610,80); options();
    touch(SDL_FINGERDOWN,760,500); options();
    assert(volume>0.89f && volume<0.91f); // Released slider no longer follows touch.
    touch(SDL_FINGERUP,760,500); options();
    shot(app,"/tmp/bv2-menu-test/touch.bmp");
    // Back must choose Cancel, not a No toggle elsewhere on an options page.
    gui.resetSelection();
    bool toggled=false,cancelled=false;
    auto backMenu=[&]() {
        app.getInputManager().updateInput(); gui.begin();
        toggled=gui.doButton(10,Vector2(40,100),TextManager::LBL_NO);
        cancelled=gui.doButton(20,Vector2(440,500),TextManager::LBL_CANCEL);
        gui.end(renderer); renderer.refresh();
    };
    backMenu(); key(SDLK_ESCAPE); backMenu(); assert(cancelled && !toggled);

    // Render the actual shipped screens with the same renderer and font.
    auto screen=[&]() {
        app.getInputManager().updateInput(); gui.begin(); app.step();
        gui.end(renderer); renderer.refresh();
    };
    app.switchToState(std::unique_ptr<State>(new MainMenuState()));
    screen(); screen(); shot(app,"/tmp/bv2-menu-test/main-menu.bmp");
    key(SDLK_RIGHT); screen(); key(SDLK_RETURN); screen();
    assert(std::string(app.getCurrenStateName())=="ControllerSetupState");
    screen(); shot(app,"/tmp/bv2-menu-test/two-players.bmp");
    // Toggle rumble by touch, then check persistence instead of only drawing it.
    touch(SDL_FINGERDOWN,80,380); screen();
    touch(SDL_FINGERUP,80,380); screen();
    UserConfig savedConfig; savedConfig.loadFile("config.xml");
    assert(!savedConfig.getBool("rumble",true));
    touch(SDL_FINGERDOWN,80,380); screen();
    touch(SDL_FINGERUP,80,380); screen();
    savedConfig.loadFile("config.xml"); assert(savedConfig.getBool("rumble",false));
    key(SDLK_ESCAPE); screen();
    assert(std::string(app.getCurrenStateName())=="MainMenuState");
    screen(); key(SDLK_RETURN); screen();
    assert(std::string(app.getCurrenStateName())=="ControllerSetupState");
    screen(); shot(app,"/tmp/bv2-menu-test/single-player.bmp");
    app.switchToState(std::unique_ptr<State>(new OptionState()));
    screen(); screen(); shot(app,"/tmp/bv2-menu-test/player-options.bmp");
    app.switchToState(std::unique_ptr<State>(new GraphicOptionsState()));
    screen(); screen(); shot(app,"/tmp/bv2-menu-test/graphics-options.bmp");
    return 0;
}
