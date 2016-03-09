//
// Created by Zhang Shibo on 2016/2/29.
//

#ifndef DEZHOU_SERVER_PLAYER_H
#define DEZHOU_SERVER_PLAYER_H

#include <string>
#include "TexasPoker.h"

class CDesk;
class CPlayer
{
    friend class CDesk;
public:
    bool Check(); //让牌
    bool Call(int iChip); //跟注
    bool Fold(); //弃牌
    bool Raise(int iChip); //加注
    bool AllIn(int iChip); //全下

private:
    int iSeatNo_; //座号
    int iChip_; //筹码
    int iCurrentBetting_; //当前的下注
    std::array<Poker::CCardsInfo, 2> arrBottomCard_; //底牌
    Poker::CCombination CardsOptimumCombination_; //最优组合
    Poker::PlayerStatus iPlayerStatus_; //玩家当前状态
    CDesk* desk_; //所在的桌子
};

#endif //DEZHOU_SERVER_PLAYER_H
