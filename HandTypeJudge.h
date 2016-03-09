//
// Created by Administrator on 2016/3/9.
//

#ifndef DEZHOU_SERVER_HANDTYPEJUDGE_H
#define DEZHOU_SERVER_HANDTYPEJUDGE_H

#include "TexasPoker.h"

class CHandTypeJudge {
public:
    static CHandTypeJudge* GetInstance();
    bool GetBestCombinationFromFiveCards(Poker::CCombination& cardOutCombination);  //五张牌的最佳牌型
    bool GetBestCombinationFromSevenCards(const std::vector<Poker::CCardsInfo>& vecInCards, Poker::CCombination& cardOutCombination); //七张牌的最佳牌型

private:
    CHandTypeJudge() { calcPlace(); }
    CHandTypeJudge(const CHandTypeJudge& handTypeJudge) {};
    void operator = (const CHandTypeJudge& handTypeJudge) {};

    void calcPlace();
    static std::vector<std::vector<int>> place_;
    static CHandTypeJudge* handTypeJudge_;
};


#endif //DEZHOU_SERVER_HANDTYPEJUDGE_H
