/*=============================================================================
Blobby Volley 2
Copyright (C) 2006 Jonathan Sieber (jonathan_sieber@yahoo.de)
Copyright (C) 2006 Daniel Knobe (daniel-knobe@web.de)

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
=============================================================================*/

/* header include */
#include "State.h"

/* includes */
#include <algorithm>
#include <cstdio>

#include "LocalGameState.h"
#include "ReplaySelectionState.h"
#include "NetworkState.h"
#include "NetworkSearchState.h"
#include "OptionsState.h"

#include "BlobbyApp.h"
#include "SoundManager.h"
#include "IMGUI.h"
#include "TextManager.h"
#include "SpeedController.h"
#include "InputManager.h"
#include "UserConfig.h"
#include "IScriptableComponent.h"

/* implementation */

State::State() = default;

void State::playSound(const std::string& sound, float volume) {
	assert(m_App);
	m_App->getSoundManager().playSound(sound, volume);
}

bool State::is_exiting() const {
	return getInputMgr().exit();
}

bool State::clicked() const {
	return getInputMgr().click();
}

void State::setApp(BlobbyApp* app) {
	assert(app);
	m_App = app;
}

void State::switchState(State* newState)
{
	assert(m_App);
	m_App->switchToState(std::unique_ptr<State>(newState));
}

BlobbyApp& State::getApp() const
{
	assert(m_App);
	return *m_App;
}

IMGUI& State::getIMGUI() const {
	return getApp().getIMGUI();
}

InputManager& State::getInputMgr() const {
	return getApp().getInputManager();
}

// --------------------------------------------------------------------------------------------------------------------------------
//  concrete implementation of MainMenuState and CreditsState
// -----------------------------------------------------------

MainMenuState::MainMenuState()
{
	// set main menu fps
	SpeedController::getMainInstance()->setGameSpeed(75);
}

MainMenuState::~MainMenuState() = default;

void MainMenuState::step_impl()
{
	IMGUI& imgui = getIMGUI();

	imgui.doCursor();
	imgui.doImage(GEN_ID, Vector2(400.0, 300.0), "background");
	imgui.doOverlay(GEN_ID, Vector2(0.0, 0.0), Vector2(800.0, 600.0));
	#ifdef BLOBBY_MODERN_UI
    imgui.doImage(GEN_ID,Vector2(400,125),"gfx/titel.bmp",Vector2(480,210));
    if(imgui.doButton(GEN_ID,Vector2(40,270),"SINGLE PLAYER")) {
#ifdef BLOBBY_SWITCH2
        getInputMgr().controllers().resetPlayers();
#endif
        switchState(new ControllerSetupState(1));
    }
    if(imgui.doButton(GEN_ID,Vector2(440,270),"TWO PLAYERS")) switchState(new ControllerSetupState(2));
    imgui.doText(GEN_ID,Vector2(40,309),"YOU AGAINST THE COMPUTER",TF_SMALL_FONT);
    imgui.doText(GEN_ID,Vector2(440,309),"TOGETHER ON THIS SWITCH",TF_SMALL_FONT);
    if(imgui.doButton(GEN_ID,Vector2(40,350),TextManager::MNU_LABEL_OPTIONS)) switchState(new OptionState());
    if(imgui.doButton(GEN_ID,Vector2(440,350),"CONTROLLERS")) switchState(new ControllerSetupState());
    if(imgui.doButton(GEN_ID,Vector2(40,410),TextManager::MNU_LABEL_ONLINE)) switchState(new OnlineSearchState());
    if(imgui.doButton(GEN_ID,Vector2(440,410),TextManager::MNU_LABEL_LAN)) switchState(new LANSearchState());
    if(imgui.doButton(GEN_ID,Vector2(40,470),TextManager::MNU_LABEL_REPLAY,TF_SMALL_FONT)) switchState(new ReplaySelectionState());
    if(imgui.doButton(GEN_ID,Vector2(440,470),TextManager::MNU_LABEL_CREDITS,TF_SMALL_FONT)) switchState(new CreditsState());
    if(imgui.doButton(GEN_ID,Vector2(40,530),TextManager::MNU_LABEL_EXIT,TF_SMALL_FONT)) getInputMgr().setEndBlobby();
    return;
#endif
    imgui.doImage(GEN_ID, Vector2(400.0, 175.0), "gfx/titel.bmp");
    const float left = 34, right = 34;
    const float rows[] = {300,340,380,420,460,500,540};
	if (imgui.doButton(GEN_ID, Vector2(right, rows[0]), TextManager::MNU_LABEL_ONLINE))
	{
		switchState( new OnlineSearchState() );
	}
	if (imgui.doButton(GEN_ID, Vector2(right, rows[1]), TextManager::MNU_LABEL_LAN))
	{
		switchState( new LANSearchState() );
	}
	if (imgui.doButton(GEN_ID, Vector2(left, rows[2]), TextManager::MNU_LABEL_START))
	{
		try
		{
			switchState(new LocalGameState());
		}
		catch (const ScriptException& except)
		{
			FILE* file = fopen("lualog.txt", "wb");
			fprintf(file, "Lua Error: %s\n",
				except.luaerror.c_str());
			fclose(file);
		}
	}

	if (imgui.doButton(GEN_ID, Vector2(left, rows[3]), TextManager::MNU_LABEL_OPTIONS))
	{
		switchState(new OptionState());
	}

	if (imgui.doButton(GEN_ID, Vector2(left, rows[4]), TextManager::MNU_LABEL_REPLAY))
	{
		switchState(new ReplaySelectionState());
	}

	if (imgui.doButton(GEN_ID, Vector2(right, rows[5]), TextManager::MNU_LABEL_CREDITS))
	{
		switchState(new CreditsState());
	}

	if (imgui.doButton(GEN_ID, Vector2(left, rows[6]), TextManager::MNU_LABEL_EXIT))
	{
		getInputMgr().setEndBlobby();
	}
}

const char* MainMenuState::getStateName() const
{
	return "MainMenuState";
}

CreditsState::CreditsState()
{
	mYPosition = 600;
}

void CreditsState::step_impl()
{
	IMGUI& imgui = getIMGUI();
	imgui.doCursor();
	imgui.doImage(GEN_ID, Vector2(400.0, 300.0), "background");
	imgui.doOverlay(GEN_ID, Vector2(0.0, 0.0), Vector2(800.0, 600.0));

	const float xPosition = 50;

	imgui.doText(GEN_ID, Vector2(xPosition, mYPosition), TextManager::CRD_PROGRAMMERS);
	imgui.doText(GEN_ID, Vector2(xPosition, mYPosition+30), "Daniel Knobe");
	imgui.doText(GEN_ID, Vector2(xPosition, mYPosition+60), "  (daniel-knobe(at)web.de)", TF_SMALL_FONT);
	imgui.doText(GEN_ID, Vector2(xPosition, mYPosition+85), "Jonathan Sieber");
	imgui.doText(GEN_ID, Vector2(xPosition, mYPosition+115), "  (jonathan_sieber(at)yahoo.de)", TF_SMALL_FONT);
	imgui.doText(GEN_ID, Vector2(xPosition, mYPosition+140), "Erik Schultheis");
	imgui.doText(GEN_ID, Vector2(xPosition, mYPosition+170), "  (erik-schultheis(at)freenet.de)", TF_SMALL_FONT);

	imgui.doText(GEN_ID, Vector2(xPosition, mYPosition+210), TextManager::CRD_GRAPHICS);
	imgui.doText(GEN_ID, Vector2(xPosition, mYPosition+240), "Silvio Mummert");
	imgui.doText(GEN_ID, Vector2(xPosition, mYPosition+270), "  (mummertathome(at)t-online.de)", TF_SMALL_FONT);
	imgui.doText(GEN_ID, Vector2(xPosition, mYPosition+295), "Richard Bertrand");
	imgui.doText(GEN_ID, Vector2(xPosition, mYPosition+325), "  (ricbertrand(at)hotmail.com)", TF_SMALL_FONT);

	imgui.doText(GEN_ID, Vector2(xPosition, mYPosition+365), TextManager::CRD_THX);
	imgui.doText(GEN_ID, Vector2(xPosition, mYPosition+395), "Daniel Skoraszewsky");
	imgui.doText(GEN_ID, Vector2(xPosition, mYPosition+425), "  (skoraszewsky(at)t-online.de)", TF_SMALL_FONT);

	if (mYPosition > 20)
		mYPosition -= 2.5;

	if (imgui.doButton(GEN_ID, Vector2(74.0, 530.0), TextManager::LBL_OK))
	{
		switchState(new MainMenuState());
	}

#if SDL_VERSION_ATLEAST(2, 0, 14)
	{
		SDL_version linked;
		SDL_GetVersion(&linked);

		if (SDL_VERSIONNUM(linked.major, linked.minor, linked.patch) >=
			SDL_VERSIONNUM(2, 0, 14))
		{
			if (imgui.doButton(GEN_ID, Vector2(294.0, 530.0), "www.blobbyvolley.de"))
			{
				SDL_OpenURL("https://www.blobbyvolley.de");
			}
		}
	}
#endif
}

const char* CreditsState::getStateName() const
{
	return "CreditsState";
}


void ControllerSetupState::init() {
    UserConfig config; config.loadFile("config.xml");
    mRumble=config.getBool("rumble",true);
#ifdef BLOBBY_SWITCH2
    auto& pads=getInputMgr().controllers();
    pads.setRumble(mRumble);
    pads.beginAssignment();
    if(mPlayers==1) pads.ensurePlayer(LEFT_PLAYER);
#endif
}

void ControllerSetupState::step_impl() {
    auto& ui=getIMGUI();
    ui.doCursor();
    ui.doImage(GEN_ID,Vector2(400,300),"background");
    ui.doOverlay(GEN_ID,Vector2(0,0),Vector2(800,600),Color(8,16,30),0.94f);
    ui.doText(GEN_ID,Vector2(400,24),mPlayers==2 ? "TWO PLAYERS" : mPlayers==1 ? "SINGLE PLAYER" : "CONTROLLERS",TF_ALIGN_CENTER);
    ui.doText(GEN_ID,Vector2(400,70),"PAIRED JOY-CONS OR PRO: PRESS L+R TO JOIN",TF_SMALL_FONT|TF_ALIGN_CENTER);
    ui.doText(GEN_ID,Vector2(400,94),"SINGLE JOY-CON: HOLD SIDEWAYS AND PRESS SL+SR",TF_SMALL_FONT|TF_ALIGN_CENTER);
#ifdef BLOBBY_SWITCH2
    auto& pads=getInputMgr().controllers();
    pads.joinPlayers(mPlayers==1 ? 1 : 2);
#endif
    for(int p=0;p<2;++p) {
        const float x=24+p*400;
        ui.doOverlay(GEN_ID,Vector2(x,135),Vector2(x+352,250),Color(16,30,55),1);
        ui.doText(GEN_ID,Vector2(x+16,150),p==0 ? "PLAYER 1" : "PLAYER 2");
        std::string name=p==1 && mPlayers==1 ? "COMPUTER OPPONENT" : "PRESS L+R OR SL+SR";
#ifdef BLOBBY_SWITCH2
        if(p==0 || mPlayers!=1) name=pads.playerName(p);
#endif
        ui.doText(GEN_ID,Vector2(x+16,205),name,TF_SMALL_FONT);
    }
    if(ui.doButton(GEN_ID,Vector2(40,280),"PAIRED / PRO",TF_SMALL_FONT)) {
#ifdef BLOBBY_SWITCH2
        pads.pairControllers();
#endif
        mMessage="PRESS L+R ON EACH PAIR OR PRO CONTROLLER";
    }
    if(ui.doButton(GEN_ID,Vector2(440,280),"SPLIT JOY-CONS",TF_SMALL_FONT)) {
        mMessage="DETACH JOY-CONS, THEN PRESS SL+SR ON EACH";
#ifdef BLOBBY_SWITCH2
        if(!pads.splitPairs()) mMessage="COULD NOT SPLIT: TRY HOME - CONTROLLERS";
#endif
    }
    ui.doText(GEN_ID,Vector2(400,326),mMessage,TF_SMALL_FONT|TF_ALIGN_CENTER);
    if(ui.doButton(GEN_ID,Vector2(40,370),mRumble ? "RUMBLE: ON" : "RUMBLE: OFF",TF_SMALL_FONT)) {
        mRumble=!mRumble;
        UserConfig config; config.loadFile("config.xml"); config.setBool("rumble",mRumble); config.saveFile("config.xml");
#ifdef BLOBBY_SWITCH2
        pads.setRumble(mRumble);
#endif
    }
    if(ui.doButton(GEN_ID,Vector2(440,370),"TEST RUMBLE",TF_SMALL_FONT)) {
#ifdef BLOBBY_SWITCH2
        pads.hit(LEFT_PLAYER); pads.hit(RIGHT_PLAYER);
#endif
    }
    ui.doText(GEN_ID,Vector2(40,425),"MOVE: STICK   JUMP: RIGHT FACE BUTTON",TF_SMALL_FONT);
    ui.doText(GEN_ID,Vector2(40,449),"PAUSE: PLUS / MINUS",TF_SMALL_FONT);
    bool ready=true;
#ifdef BLOBBY_SWITCH2
    ready=pads.ready(mPlayers);
#endif
    if(mPlayers>0) {
        if(ready) {
            if(ui.doButton(GEN_ID,Vector2(200,515),"START MATCH",TF_SMALL_FONT)) {
#ifdef BLOBBY_SWITCH2
                pads.endAssignment();
#endif
                switchState(new LocalGameState(mPlayers));
            }
        } else ui.doText(GEN_ID,Vector2(160,515),"WAITING FOR PLAYERS",TF_SMALL_FONT);
    }
    if(ui.doButton(GEN_ID,Vector2(540,515),TextManager::LBL_CANCEL,TF_SMALL_FONT)) {
#ifdef BLOBBY_SWITCH2
        pads.endAssignment();
#endif
        switchState(new MainMenuState());
    }
}
