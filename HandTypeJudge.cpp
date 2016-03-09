//
// Created by Administrator on 2016/3/9.
//

#include "HandTypeJudge.h"
#include <algorithm>

using namespace std;
using namespace Poker;

vector<vector<int>> CHandTypeJudge::place_;
CHandTypeJudge* CHandTypeJudge::handTypeJudge_ = nullptr;

CHandTypeJudge* CHandTypeJudge::GetInstance() {
    if (nullptr != handTypeJudge_) {
        return handTypeJudge_;
    }
    return handTypeJudge_;
}

bool CHandTypeJudge::GetBestCombinationFromFiveCards(Poker::CCombination& cardOutCombination) {  //五张牌的最佳牌型
    if (cardOutCombination.cards_.size() != 5) {
        return false;
    }
    //排序
    sort(cardOutCombination.cards_.begin(), cardOutCombination.cards_.end(), [](const Poker::CCardsInfo &cards1, const Poker::CCardsInfo &cards2){return cards1.iNum_ < cards2.iNum_;});

    //判断是否是顺子
    int iCardsSeq = 0;
    bool isFound = false;
    for (iCardsSeq = 0; iCardsSeq < 4; ++ iCardsSeq) {//每张牌依次跟后张牌比较
        if (cardOutCombination.cards_[iCardsSeq].iNum_ + 1 != cardOutCombination.cards_[iCardsSeq + 1].iNum_) {
            break;
        }
    }

    if (( 3 == iCardsSeq && CARD_2 == cardOutCombination.cards_[0].iNum_ && CARD_A ==cardOutCombination.cards_[4].iNum_) //A1234
        || 4 == iCardsSeq) {//其他顺子
        cardOutCombination.type_ = STRAIGHT;
        isFound = true;
    }

    //判断是否是同花
    for (iCardsSeq = 0; iCardsSeq < 5; ++ iCardsSeq) {
        if (cardOutCombination.cards_[0].iSuit_ != cardOutCombination.cards_[iCardsSeq].iSuit_) {
            break;
        }
    }

    if (5 == iCardsSeq) {
        cardOutCombination.type_ = cardOutCombination.type_ == STRAIGHT ? STRAIGHT_FLUSH : FLUSH;
        isFound = true;
    }

    if (STRAIGHT_FLUSH == cardOutCombination.type_  //同花顺
        && CARD_T == cardOutCombination.cards_[0].iNum_ && CARD_A == cardOutCombination.cards_[4].iNum_) {//TJQKA
        cardOutCombination.type_ = ROYAL_FLUSH; //皇家同花顺
    }

    if (isFound) {//是顺子，同花，同花顺，皇家同花顺中的一种
        return true;
    }

    //非顺子，同花，同花顺，皇家同花顺中的任一种
    int flag = 0;
    for (int iIndexPre = 0; iIndexPre < cardOutCombination.cards_.size(); ++ iIndexPre) {
        for (int iIndexAfter = iIndexPre + 1; iIndexAfter < cardOutCombination.cards_.size(); ++iIndexAfter) {
            if (cardOutCombination.cards_[iIndexPre].iNum_ == cardOutCombination.cards_[iIndexAfter].iNum_) {
                ++flag;
            }
        }
    }

    switch (flag) {
        case 0: //高牌
            cardOutCombination.type_ = HIGH_CARD;
            break;
        case 1: //一对
            cardOutCombination.type_ = ONE_PAIR;
            break;
        case 2: //两对
            cardOutCombination.type_ = TWO_PAIR;
            break;
        case 3: //三条
            cardOutCombination.type_ = THREE_OF_A_KIND;
            break;
        case 4: //葫芦
            cardOutCombination.type_ = FULL_HOUSE;
            break;
        case 6: //四条
            cardOutCombination.type_ = FOUR_OF_A_KIND;
            break;
    }
    return true;
}

bool CHandTypeJudge::GetBestCombinationFromSevenCards(const std::vector<Poker::CCardsInfo>& vecInCards, Poker::CCombination& cardOutCombination) { //七张牌的最佳牌型
    if (vecInCards.size() != 7) {
        return false;
    }

    //将牌组合成各种组合
    vector<CCombination> vecCombination;
    for (auto placeIt = place_.begin(); placeIt != place_.end(); ++placeIt) {
        CCombination aCombination;
        for (auto eachPlaceIt = placeIt->begin(); eachPlaceIt != placeIt->end(); ++eachPlaceIt) {
            aCombination.cards_.push_back(vecInCards[*eachPlaceIt]);
        }
        vecCombination.push_back(aCombination);
    }

    for (auto& eachCombination : vecCombination) {    //对各种组合进行牌型判断
        if (!GetBestCombinationFromFiveCards(eachCombination)) {
            continue;//error, continue next combination
        }
    }

    //找出最大的牌型
    auto maxComb = max_element(vecCombination.begin(), vecCombination.end(), [](const CCombination &comb1, const CCombination &comb2){ return comb1.type_ < comb2.type_; });
    cardOutCombination = move(*maxComb);
    return true;
}

void CHandTypeJudge::calcPlace() {
    int i = 0 ,j = 0;
    int b[5] = {0};
    int iTotalNumCount_ = 7;
    int iChosenNumCount_ = 5;
    while(b[0] < iTotalNumCount_ ) {
        while( i < iTotalNumCount_) {
            if(j == iChosenNumCount_ - 1 ) {
                place_.push_back(vector<int>(b, b + iChosenNumCount_));
            } else {
                j ++;
            }
            i ++;
            b[j] = i;
        }
        do {
            j --;
            b[j] ++;
        } while(b[j] >= iTotalNumCount_ );
        i = b[j];
    }
}
