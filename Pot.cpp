//
// Created by Administrator on 2016/3/8.
//

#include "Pot.h"
#include <algorithm>
#include <iostream>

using namespace std;
bool CPot::AddChip(int iPlayerSeatNo, int iChip) {
    bool bIsFound = false;
    for (auto& item : vecPlayerChips_) {
        if (item.iPlayerSeatNo_ == iPlayerSeatNo) { //找到了
            bIsFound = true;
            item.AddChip(iChip); //为该用户新增筹码
            break;
        }
    }
    if (!bIsFound) { //没找到，在此池中新增用户
        CPlayerMapToChip newPlayer;
        newPlayer.SetPlayerSeatNo(iPlayerSeatNo);
        newPlayer.AddChip(iChip);
        vecPlayerChips_.push_back(newPlayer);
    }
    iTotalCount_ += iChip; //池中总筹码数增加
    return true;
}

bool CPot::DelChip(int iPlayerSeatNo, int iChip) {
    for (auto& item : vecPlayerChips_) {
        if (item.iPlayerSeatNo_ == iPlayerSeatNo) { //找到了
            item.DelChip(iChip); //删除相应的筹码
            iTotalCount_ -= iChip; //池中总筹码数减少
            return true;
        }
    }
    return false;//没找到
}

bool CPot::MovePlayerToFoldList(int iPlayerSeatNo) {
    for (auto player = vecPlayerChips_.begin(); player != vecPlayerChips_.end(); ++player) {
        if (player->GetPlayerSeatNo() == iPlayerSeatNo) { //找到了
            player->PlayerFold(true); //为该玩家做弃牌标记
            vecFoldPlayerChips_.push_back(*player); //将该玩家加入到弃牌队列中
            vecPlayerChips_.erase(player); //从底池中删除该玩家
            return true;
        }
    }
    return false; //没找到
}

bool CPot::SortPotByChip() {
    sort(vecPlayerChips_.begin(), vecPlayerChips_.end(), [](const CPlayerMapToChip& p1, const CPlayerMapToChip& p2){ return p1.iChip_ > p2.iChip_; });
    return true;
}

bool CPot::IsHasThisPlayer(int iPlayerSeatNo) {
    for (auto& player : vecPlayerChips_) {
        if (player.GetPlayerSeatNo() == iPlayerSeatNo) {
            return true;
        }
    }
    return false;
}

bool CTotalPot::PotInit() {
    vecPot_.clear();    //将池子重新置为1个
    vecPot_.push_back(CPot());
    iPotIndex_ = 0; //初始化各项参数
    iMaxPoolSize_ = 1;
    return true;
}

bool CTotalPot::AddChip(int iPlayerSeatNo, int iChip) {
    if (iPotIndex_ >= iMaxPoolSize_ || iPotIndex_ < 0) {
        return false; //error
    }

    if (iChip <= 0) {
        return false;
    }
    auto& currentPot = vecPot_[iPotIndex_];
    currentPot.AddChip(iPlayerSeatNo, iChip);
    return true;
}

bool CTotalPot::PlayerFold(int iPlayerSeatNo) {
    if (iPotIndex_ >= iMaxPoolSize_ || iPotIndex_ < 0) {
        return false; //error
    }
    auto& currentPot = vecPot_[iPotIndex_];
    return currentPot.MovePlayerToFoldList(iPlayerSeatNo);
}

bool CTotalPot::CalcPot() {
    bool ret = CalcPotPart(iPotIndex_);
    if (!ret) { return false; }
    iPotIndex_ = iMaxPoolSize_ - 1; //下一轮的池子是当前池中所有池子中的最后一个
    return true;
}

bool CTotalPot::CalcPotPart(int iPotIndex) {
    if (iPotIndex >= iMaxPoolSize_ || iPotIndex < 0) {
        return false; //当前索引值比池子个数大，error
    }
    auto& currentPot = vecPot_[iPotIndex];
    currentPot.SortPotByChip();//排序
    int iPotSize = currentPot.GetPotSize();
    if (0 == iPotSize) { //该池中没有金币，error
        return false;
    }
    auto& vecPlayerMapToChips = currentPot.GetPlayerChipsConstRef(); //取出当前的池子
    int iMaxChipInPot = vecPlayerMapToChips[0].GetChip(),
        iMinChipInPot = vecPlayerMapToChips[iPotSize - 1].GetChip();
    if (iMaxChipInPot == iMinChipInPot) { //金币一致，不需要分池
        return true;
    }
    for (auto& eachPlayer : vecPlayerMapToChips) { //需要分池
        int iDiff = eachPlayer.GetChip() - iMinChipInPot; //计算和池中最小金币的差额
        if (0 == iDiff) {
            break; //已经分完
        }
        else {
            int iPlayerSeatNo = eachPlayer.GetPlayerSeatNo(); //需要对该玩家分池
            vecPot_[iPotIndex].DelChip(iPlayerSeatNo, iDiff); //将该玩家的金币从当前池中减去
            if (iPotIndex + 1 == iMaxPoolSize_) { //说明当前池子已经是最后一个了，需要新增池子
                CPot pot;
                vecPot_.push_back(pot);
                iMaxPoolSize_ += 1;
            }
            vecPot_[iPotIndex + 1].AddChip(iPlayerSeatNo, iDiff);
        }
    }
    CalcPotPart(iPotIndex + 1); //继续计算下一个池子的金币
    return true;
}

void CTotalPot::GetWinnerChip(int iPlayerSeatNo) {
    for (auto pot = vecPot_.rbegin(); pot != vecPot_.rend(); ++pot) {

    }
}
void CTotalPot::Print() {
    for (auto item : vecPot_) {
        auto temp = item.GetPlayerChipsConstRef();
        for (auto i : temp) {
            cout << i.GetPlayerSeatNo() << " : " << i.GetChip() << endl;
        }
        cout << endl;
    }
}