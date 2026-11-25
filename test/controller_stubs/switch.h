#pragma once
#include <cstdint>
#include <vector>
using u32=std::uint32_t;
using Result=int;
#define R_FAILED(x) ((x)!=0)
enum HidNpadIdType { HidNpadIdType_Handheld=32 };
enum HidNpadStyleTag { HidNpadStyleTag_NpadFullKey=1,HidNpadStyleTag_NpadHandheld=2,HidNpadStyleTag_NpadJoyDual=4,HidNpadStyleTag_NpadJoyLeft=8,HidNpadStyleTag_NpadJoyRight=16,HidNpadStyleSet_NpadStandard=31 };
enum { HidNpadJoyHoldType_Horizontal, HidNpadJoyDeviceType_Left };
struct HidAnalogStickState { int x=0,y=0; };
struct PadState { int id=0; };
struct FakePad { u32 style=0; std::uint64_t buttons=0; HidAnalogStickState sticks[2]; };
static FakePad fakePads[33];
struct HidVibrationDeviceHandle { int id=0; };
struct HidVibrationValue { float amp_low,freq_low,amp_high,freq_high; };
struct SentVibration { int id; float amplitude; };
static std::vector<SentVibration> vibrations;
inline void padConfigureInput(int,int) {}
inline void hidSetNpadJoyHoldType(int) {}
inline void padInitializeWithMask(PadState* p,std::uint64_t mask) { p->id=0; while(!(mask&(1ULL<<p->id))) ++p->id; }
inline void padUpdate(PadState*) {}
inline bool padIsConnected(PadState* p) { return fakePads[p->id].style!=0; }
inline u32 padGetStyleSet(PadState* p) { return fakePads[p->id].style; }
inline HidAnalogStickState padGetStickPos(PadState* p,int stick) { return fakePads[p->id].sticks[stick]; }
inline std::uint64_t padGetButtons(PadState* p) { return fakePads[p->id].buttons; }
inline void hidStartLrAssignmentMode() {}
inline void hidStopLrAssignmentMode() {}
inline void hidSetNpadJoyAssignmentModeDual(HidNpadIdType) {}
inline void hidMergeSingleJoyAsDualJoy(HidNpadIdType,HidNpadIdType) {}
inline Result hidSetNpadJoyAssignmentModeSingleWithDestination(HidNpadIdType,int,bool* moved,HidNpadIdType*) { *moved=false; return 0; }
inline Result hidInitializeVibrationDevices(HidVibrationDeviceHandle* handles,int count,HidNpadIdType id,HidNpadStyleTag) {
    for(int i=0;i<count;++i) { handles[i].id=static_cast<int>(id); }
    return 0;
}
inline void hidSendVibrationValue(HidVibrationDeviceHandle h,const HidVibrationValue* v) { vibrations.push_back({h.id,v->amp_high}); }
