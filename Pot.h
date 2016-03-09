//
// Created by Administrator on 2016/3/8.
//

#ifndef DEZHOU_SERVER_POT_H
#define DEZHOU_SERVER_POT_H

#include <vector>
#include <string>
class CPlayerMapToChip
{
public:
    friend class CPot;
    CPlayerMapToChip()
        :iPlayerSeatNo_(-1),
         iChip_(0),
         bIsFold_(false) {}

    void SetPlayerSeatNo(int iPlayerSeatNo) { iPlayerSeatNo_ = iPlayerSeatNo; }
    int GetPlayerSeatNo () const { return iPlayerSeatNo_; }

    void AddChip(int iChip) { iChip_ += iChip; }
    void DelChip(int iChip) { iChip_ -= iChip; }
    int GetChip() const { return iChip_; }

    void PlayerFold(bool bIsFold) { bIsFold_ = bIsFold; }
    bool IsPlayerFold() { return bIsFold_; }

private:
    int iPlayerSeatNo_;  //玩家编号
    int iChip_;          //在某池中所下筹码
    bool bIsFold_;       //是否弃牌
};

class CPot
{
public:
    CPot()
        :iTotalCount_(0) {}
    bool AddChip(int iPlayerSeatNo, int iChip);
    bool DelChip(int iPlayerSeatNo, int iChip);
    bool MovePlayerToFoldList(int iPlayerSeatNo);
    bool SortPotByChip();
    bool IsHasThisPlayer(int iPlayerSeatNo);
    const std::vector<CPlayerMapToChip>& GetPlayerChipsConstRef() const { return vecPlayerChips_; };
    int GetPotSize() { return vecPlayerChips_.size(); };
private:
    int iTotalCount_; //该池的筹码总数
    std::vector<CPlayerMapToChip> vecPlayerChips_; //该池中的金币和玩家的对应关系
    std::vector<CPlayerMapToChip> vecFoldPlayerChips_; //该池中弃牌玩家和他所下注的对应关系
};

class CTotalPot {
public:
    CTotalPot()
        :iPotIndex_(0),
         iMaxPoolSize_(1),
         vecPot_({CPot()}){}

    bool PotInit();
    bool CalcPot();
    bool AddChip(int iPlayerSeatNo, int iChip);
    bool PlayerFold(int iPlayerSeatNo);
    const std::vector<CPot>& GetAllPots() { return vecPot_; }
    void Print();
    void GetWinnerChip(int iPlayerSeatNo);

private:
    bool CalcPotPart(int iPotIndex);

    int iPotIndex_; //当前池子的索引
    int iMaxPoolSize_; //当前总池子个数
    std::vector<CPot> vecPot_;  //总池
};

#endif //DEZHOU_SERVER_POT_H
