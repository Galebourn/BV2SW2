#pragma once
enum PlayerSide { NO_PLAYER=-1, LEFT_PLAYER=0, RIGHT_PLAYER=1 };
struct PlayerInputAbs {
    bool left,right,jump;
    PlayerInputAbs(bool l=false,bool r=false,bool j=false):left(l),right(r),jump(j) {}
};
