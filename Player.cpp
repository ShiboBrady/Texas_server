//
// Created by Zhang Shibo on 2016/2/29.
//

#include "Player.h"

bool CPlayer::Check() //让牌
{
    //用户筹码不变
    iPlayerStatus_ = Poker::PlayerStatus::CHECK;
    return true;
}
bool CPlayer::Call(int iChip) //跟注
{
    //用户筹码减去相应的数额
    iPlayerStatus_ = Poker::PlayerStatus::CALL;
    iChip_ -= iChip;
    iCurrentBetting_ += iChip;
    return true;
}

bool CPlayer::Fold() //弃牌
{
    //用户不能参与本局剩余轮
    iPlayerStatus_ = Poker::FOLD;
    return true;
}
bool CPlayer::Raise(int iChip) //加注
{
    //用户筹码减去相应的数额
    iPlayerStatus_ = Poker::PlayerStatus::RAISE;
    iChip_ -= iChip;
    iCurrentBetting_ += iChip;
    return true;
}

bool CPlayer::AllIn(int iChip) //全下
{
    //用户的筹码减为0
    iPlayerStatus_ = Poker::ALLIN;
    iCurrentBetting_ += iChip_;
    iChip_ = 0;
    return true;
}