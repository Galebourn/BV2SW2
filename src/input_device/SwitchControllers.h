#pragma once
#ifdef BLOBBY_SWITCH2
#include <switch.h>
#include <SDL.h>
#include <array>
#include <algorithm>
#include <string>
#include "ControllerMapping.h"
#include "PlayerInput.h"

// Own native pad state: SDL's Switch driver has fixed joystick slots and
// suppresses analog axes for sideways Joy-Cons. Keep assignment and rumble
// attached to the actual Npad slot, including the separate handheld slot.
class SwitchControllers {
    struct Slot {
        PadState pad{};
        ControllerFrame frame{}, previous{};
        HidVibrationDeviceHandle handles[2]{};
        int handleCount = 0;
        u32 style = 0, pulseStart = 0;
        bool pulsing = false;
    };
    std::array<Slot,9> slots;
    std::array<int,2> players{{-1,-1}};
    std::array<int,8> splitDestination;
    bool enabled = true;
    bool assigning = false;
    static HidNpadIdType id(int i) { return i==8 ? HidNpadIdType_Handheld : static_cast<HidNpadIdType>(i); }
    void stop(Slot& slot) {
        if (!slot.pulsing) return;
        HidVibrationValue zero{0.0f,160.0f,0.0f,320.0f};
        for (int j=0;j<slot.handleCount;++j) hidSendVibrationValue(slot.handles[j],&zero);
        slot.pulsing=false;
    }
public:
    SwitchControllers() {
        splitDestination.fill(-1);
        padConfigureInput(8,HidNpadStyleSet_NpadStandard);
        hidSetNpadJoyHoldType(HidNpadJoyHoldType_Horizontal);
        for(int i=0;i<9;++i) padInitializeWithMask(&slots[i].pad,1ULL << id(i));
    }
    ~SwitchControllers() { stopAll(); endAssignment(); restorePairs(); }
    void stopAll() { for(auto& slot:slots) stop(slot); }
    void setRumble(bool value) { enabled=value; if(!enabled) stopAll(); }
    bool rumbleEnabled() const { return enabled; }
    void update() {
        const u32 now=SDL_GetTicks();
        for(int i=0;i<9;++i) {
            auto& s=slots[i]; s.previous=s.frame;
            padUpdate(&s.pad);
            const u32 style=padIsConnected(&s.pad) ? padGetStyleSet(&s.pad) : 0;
            if(style!=s.style) { stop(s); s.handleCount=0; s.style=style; s.previous={}; }
            if(!style) { s.frame={}; continue; }
            const auto layout=(style&HidNpadStyleTag_NpadJoyLeft) ? ControllerLayout::LeftSingle :
                (style&HidNpadStyleTag_NpadJoyRight) ? ControllerLayout::RightSingle : ControllerLayout::Full;
            const auto l=padGetStickPos(&s.pad,0), r=padGetStickPos(&s.pad,1);
            s.frame=mapController(layout,l.x,l.y,r.x,r.y,padGetButtons(&s.pad));
            if(s.pulsing && u32(now-s.pulseStart)>=65) stop(s);
        }
    }
    const ControllerFrame& frame(int i) const { return slots[i].frame; }
    const ControllerFrame& previous(int i) const { return slots[i].previous; }
    bool connected(int i) const { return i>=0 && i<9 && slots[i].style!=0; }
    bool ready(int count) const {
        for(int p=0;p<count;++p) if(!connected(players[p])) return false;
        return count<2 || players[0]!=players[1];
    }
    void resetPlayers() { stopAll(); players={{-1,-1}}; }
    void beginAssignment() { resetPlayers(); hidStartLrAssignmentMode(); assigning=true; }
    void endAssignment() { if(assigning) hidStopLrAssignmentMode(); assigning=false; }
    void joinPlayers(int count) {
        for(int i=0;i<9;++i) if(connected(i) && slots[i].frame.join && !slots[i].previous.join) {
            if(count==1) { players[0]=i; continue; }
            if(players[0]==i || players[1]==i) continue;
            for(int p=0;p<count;++p) if(!connected(players[p])) { players[p]=i; break; }
        }
    }
    void ensurePlayer(PlayerSide side) {
        if(side!=LEFT_PLAYER && side!=RIGHT_PLAYER) return;
        if(connected(players[side])) return;
        if(connected(8) && players[1-side]!=8) { players[side]=8; return; }
        for(int i=0;i<8;++i) if(connected(i) && players[1-side]!=i) { players[side]=i; break; }
    }
    std::string playerName(int p) const {
        const int i=players[p]; if(!connected(i)) return "PRESS L+R OR SL+SR";
        const u32 style=slots[i].style;
        if(style&HidNpadStyleTag_NpadHandheld) return "ATTACHED JOY-CONS";
        if(style&HidNpadStyleTag_NpadJoyLeft) return "LEFT JOY-CON / SIDEWAYS";
        if(style&HidNpadStyleTag_NpadJoyRight) return "RIGHT JOY-CON / SIDEWAYS";
        if(style&HidNpadStyleTag_NpadJoyDual) return "PAIRED JOY-CONS";
        return "PRO / FULL CONTROLLER";
    }
    bool splitPairs() {
        resetPlayers(); bool ok=true;
        for(int i=0;i<8;++i) if(slots[i].style&HidNpadStyleTag_NpadJoyDual) {
            bool moved=false; HidNpadIdType dest{};
            Result rc=hidSetNpadJoyAssignmentModeSingleWithDestination(id(i),HidNpadJoyDeviceType_Left,&moved,&dest);
            if(R_FAILED(rc)) ok=false;
            else if(moved) splitDestination[i]=static_cast<int>(dest);
        }
        return ok;
    }
    void restorePairs() {
        for(int i=0;i<8;++i) if(splitDestination[i]>=0) {
            hidSetNpadJoyAssignmentModeDual(id(i));
            hidSetNpadJoyAssignmentModeDual(id(splitDestination[i]));
            hidMergeSingleJoyAsDualJoy(id(i),id(splitDestination[i]));
            splitDestination[i]=-1;
        }
    }
    void pairControllers() { resetPlayers(); restorePairs(); for(int i=0;i<8;++i) hidSetNpadJoyAssignmentModeDual(id(i)); }
    PlayerInputAbs input(PlayerSide side) const {
        if(side!=LEFT_PLAYER && side!=RIGHT_PLAYER) return {};
        const int i=players[side]; if(!connected(i)) return {};
        const auto& f=slots[i].frame;
        return {f.x < -10000 || bool(f.dpad&1),f.x > 10000 || bool(f.dpad&4),f.select};
    }
    void hit(PlayerSide side) {
        if(!enabled || (side!=LEFT_PLAYER && side!=RIGHT_PLAYER)) return;
        const int i=players[side]; if(!connected(i)) return;
        auto& s=slots[i];
        if(!s.handleCount) {
            const int count=(s.style&(HidNpadStyleTag_NpadJoyLeft|HidNpadStyleTag_NpadJoyRight)) ? 1 : 2;
            if(R_FAILED(hidInitializeVibrationDevices(s.handles,count,id(i),static_cast<HidNpadStyleTag>(s.style)))) return;
            s.handleCount=count;
        }
        const HidVibrationValue pulse{0.20f,160.0f,0.35f,320.0f};
        for(int j=0;j<s.handleCount;++j) hidSendVibrationValue(s.handles[j],&pulse);
        s.pulseStart=SDL_GetTicks(); s.pulsing=true;
    }
};
#endif
