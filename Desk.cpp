//
// Created by Zhang Shibo on 2016/3/1.
//
#include "Desk.h"
#include <algorithm>
#include <set>
#include <time.h>
#include "EventProcess.h"
#include "HandTypeJudge.h"

using namespace std;
using namespace Poker;

CDesk::CDesk(int iMaxPlayerNum, int iSmallBlindBet)
    :iMaxPlayerNum_(iMaxPlayerNum),
     iSmallBlindBet_(iSmallBlindBet),
     iBankerPos_(-1),
     iSmallBlindPos_(-1),
     iBigBlindPos_(-1),
     iStartPos_(-1),
     iCurrentMaxChip_(-1),
     iLastValidPos_(-1),
     iExpectPos_(-1),
     handTypeJudge_(CHandTypeJudge::GetInstance()){
    InitWhenDeskInit(); //初始化位置信息
}

CDesk::~CDesk()
{ }

bool CDesk::Process(shared_ptr<CMsgNode> msgNode) {
    //根据msgId处理做相应的操作

    int iCurrentPosition;
    PlayerStatus status;
    //判断该位置是否有人
    auto findRet = mapPlayer_.find(iCurrentPosition);
    if (mapPlayer_.end() == findRet) {
        return false; //error
    }

    //判断该座位是否跟期望的一致
    if (iCurrentPosition != iExpectPos_) {
        return false; //error
    }

    //判断本轮是否结束
    bool bIsThisRoundFinished = IsThisRoundFinished(iCurrentPosition, status);
    if (bIsThisRoundFinished) {

    } else {
        //确定下一个玩家的位置
        iExpectPos_ = FindNextValidPlayerPosition(iCurrentPosition);
    }
    return true;
}

bool CDesk::InitWhenDeskInit(){ //初始化桌子
    set<int> setPos;//初始化位置队列
    srand(time(NULL)); //产生随机数
    do {
        setPos.insert(rand() % iMaxPlayerNum_);
    } while(setPos.size() != iMaxPlayerNum_);
    for (auto& it : setPos) {
        qPosition_.push(it);
    }
    return true;
}

bool CDesk::InitWhenNewRound() {
    pot_.PotInit(); //重置池子

    int iBankerPos_; //庄家位置
    int iSmallBlindPos_; //小盲位置
    int iBigBlindPos_; //大盲位置
    int iStartPos_ = iBigBlindPos_; //第一轮默认是大盲位置
    int iCurrentMaxChip_; //当前轮中最大的注
    int iLastValidPos_; //最后一个下注有效位，要么是加注，要么是allin是当局最大的，要么是跟注，弃牌或者allin不够大的不算
    int iExpectPos_; //期望的下一个玩家位置，同时也是每次返回给前端的数据
    playerMap mapPlayer_; //当前玩家
    observerPlayerList listObserverPlayer_; //当前观望的玩家
}

bool CDesk::AddPlayer() {
    if (mapPlayer_.size() >= iMaxPlayerNum_) {
        return false;
    }
    int iPos = qPosition_.front();
    qPosition_.pop();
    qPosition_.push(iPos);
    mapPlayer_.insert(make_pair(iPos, shared_ptr<CPlayer>(new CPlayer())));
    return true;
}

void CDesk::StartNewRoundGame() {
    if (mapPlayer_.size() <= 1) {
        return; //桌上人数不够
    }
    auto findRet = mapPlayer_.find(++iBankerPos_); //庄家位置按顺时针方向移动
    while (mapPlayer_.end() == findRet) {
        findRet = mapPlayer_.find(++iBankerPos_);
        iBankerPos_ = iBankerPos_ >= iMaxPlayerNum_ ? -1 : iBankerPos_;
    }

    iSmallBlindPos_ = FindNextPlayerPosition(iBankerPos_); //寻找小盲位
    iBigBlindPos_ = FindNextPlayerPosition(iSmallBlindPos_); //寻找大盲位

    for (auto playerIt : mapPlayer_) {//将用户改为准备好状态
        if (Poker::PlayerStatus::WAITING != playerIt.second->iPlayerStatus_) {
            playerIt.second->iPlayerStatus_ = Poker::PlayerStatus::READY;
        }
    }
}

bool CDesk::PlayerCheckReview(int iPlayerSeatNo) { //让牌出牌规则检查
    if (Poker::Round::PRE_FLOP == iCurrentRound_) { //第一轮
        if (iPlayerSeatNo == iBigBlindPos_) { //只有大盲位有让牌权利
            if (mapPlayer_[iLastValidPos_]->iCurrentBetting_ == mapPlayer_[iBigBlindPos_]->iCurrentBetting_) { //上一位没弃牌，没全下的玩家的下注和自己相同
                return true;
            } else {
                return false;
            }
        } else { //不是大盲位
            return false;
        }
    } else if (iPlayerSeatNo == iStartPos_ || mapPlayer_[iLastValidPos_]->iPlayerStatus_ == Poker::PlayerStatus::CHECK){ //其他轮，只有从庄家左手边的那个人有让牌权利，后边的玩家可以选择让牌or加注，一旦选择加注或全下，则后面的玩家就没有让牌权利
        return true;
    } else {
        return false;
    }
}

bool CDesk::PlayerCallReview(int iPlayerSeatNo, int iChip) { //跟注出牌规则检查
    if (iStartPos_ == iPlayerSeatNo) { //起始位置不能跟注
        return false;
    } else if (mapPlayer_[iPlayerSeatNo]->iCurrentBetting_ + iChip != iCurrentMaxChip_) {//跟注金额跟当前牌桌上最大注不一致
        return false;
    }
    return true;
}

bool CDesk::PlayerFoldReview(int iPlayerSeatNo) { //弃牌出牌规则检查
    return true;
}

bool CDesk::PlayerRaiseReview(int iPlayerSeatNo, int iChip) { //加注出牌规则检查
    if (iStartPos_ == iPlayerSeatNo) { //起始位置不能跟注
        return true;
    } else if (mapPlayer_[iPlayerSeatNo]->iCurrentBetting_ + iChip <= iCurrentMaxChip_) {//跟注金额比当前牌桌上最大注小
        return false;
    }
    return true;
}

bool CDesk::PlayerAllInReview(int iPlayerSeatNo, int iChip) { //全下出牌规则检查
    if (mapPlayer_[iPlayerSeatNo]->iChip_ != iChip) { //没有将剩余的筹码全部押下
        return false;
    }
    return true;
}

bool CDesk::PlayerCheck(int iPlayerSeatNo) {
    if (!PlayerCheckReview(iPlayerSeatNo)) {
        return false;
    }
    auto player = mapPlayer_[iPlayerSeatNo];
    player->Check();
    iLastValidPos_ = iPlayerSeatNo;
    return true;
}

bool CDesk::PlayerCall(int iPlayerSeatNo, int iChip) {
    if (!PlayerCallReview(iPlayerSeatNo, iChip)) {
        return false;
    }
    auto player = mapPlayer_[iPlayerSeatNo];
    player->Call(iChip);
    iLastValidPos_ = iPlayerSeatNo;
    pot_.AddChip(iPlayerSeatNo, iChip); //将金额添加到底池中
    return true;
}

bool CDesk::PlayerFold(int iPlayerSeatNo) {
    if (!PlayerFoldReview(iPlayerSeatNo)) {
        return false;
    }
    auto player = mapPlayer_[iPlayerSeatNo];
    player->Fold();
    pot_.PlayerFold(iPlayerSeatNo);
    return true;
}

bool CDesk::PlayerRaise(int iPlayerSeatNo, int iChip) {
    if (!PlayerRaiseReview(iPlayerSeatNo, iChip)) {
        return false;
    }
    auto player = mapPlayer_[iPlayerSeatNo];
    player->Raise(iChip);
    iStartPos_ = iPlayerSeatNo;
    iLastValidPos_ = iPlayerSeatNo;
    iCurrentMaxChip_ = iChip + mapPlayer_[iPlayerSeatNo]->iCurrentBetting_;
    pot_.AddChip(iPlayerSeatNo, iChip); //将金额添加到底池中
    return true;
}

bool CDesk::PlayerAllIn(int iPlayerSeatNo, int iChip) {
    if (!PlayerAllInReview(iPlayerSeatNo, iChip)) {
        return false;
    }
    auto player = mapPlayer_[iPlayerSeatNo];
    player->AllIn(iChip);
    iLastValidPos_ = iPlayerSeatNo;
    if (player->iCurrentBetting_ > iCurrentMaxChip_) {
        iCurrentMaxChip_ = player->iCurrentBetting_;
        iStartPos_ = iPlayerSeatNo;
    }
    pot_.AddChip(iPlayerSeatNo, iChip); //将金额添加到底池中
    return true;
}

int CDesk::FindNextValidPlayerPosition(int iCurrentSeatNo) {
    int iNextPosition;
    Poker::PlayerStatus status;
    do {
        iNextPosition = FindNextPlayerPosition(iCurrentSeatNo);
        if (-1 == iNextPosition) {
            return -1; //error.
        }
        status = mapPlayer_[iNextPosition]->iPlayerStatus_;
        iCurrentSeatNo = iNextPosition;
    } while (status == Poker::PlayerStatus::ALLIN || status == Poker::PlayerStatus::FOLD);
    return iNextPosition;
}

//该函数调用之前假定已经做好了出牌规则检查
bool CDesk::IsThisRoundFinished(int iCurrentPos, Poker::PlayerStatus status) {
    if (Poker::Round::PRE_FLOP == iCurrentRound_ && iStartPos_ == iBigBlindPos_) { //第一轮 且 无人加注
       if (iCurrentPos == iBigBlindPos_ && (CHECK == status || FOLD == status)) { //当前到了大盲位 且 当前大盲位是让牌或者弃牌状态
           return true;
       } else { //当前没到大盲位 或者 到了大盲位，大盲位是加牌或者全下状态
           return false;
       }
    } else { //非第一轮 或者 第一轮有人加注
        if (FindNextPlayerPosition(iCurrentPos) == iStartPos_ &&
                ( CALL == status || CHECK == status || FOLD == status || ALLIN == status)) { //到了开始位的前一个人 且 此人跟牌 、弃牌 或 让牌 或者全下的金额不是本轮最高的
            return true;
        } else { //没到开始位的前一个人, 或者 到了开始位的前一个人，此人加注或者全下了
            return false;
        }
    }
}

int CDesk::FindNextPlayerPosition(int iCurrentSeatNo) {
    auto currentPos = mapPlayer_.find(iCurrentSeatNo);
    while (mapPlayer_.end() == currentPos) //出错，没找到该玩家
    {
        return -1; //this seat has not player
    }

    if (mapPlayer_.rbegin()->first == currentPos->first) //下一个玩家在第一个位置
    {
        return mapPlayer_.begin()->first;
    }
    else //正常情况的下一位玩家
    {
        return (++currentPos)->first;
    }
}

bool CDesk::CalcPlayersCardsType() {
    for (auto& player : mapPlayer_) { //以此存放用户的牌信息
        vector<CCardsInfo> vecCards(arrCommunityCards_.begin(), arrCommunityCards_.end()); //用公共牌初始化
        vecCards.push_back(player.second->arrBottomCard_[0]); //塞进用户的两张牌
        vecCards.push_back(player.second->arrBottomCard_[1]);
        CCombination combination;
        if (!handTypeJudge_->GetBestCombinationFromSevenCards(vecCards, combination)) { //从7张牌中选出最优组合
            //error
        } else {
            player.second->CardsOptimumCombination_ = move(combination);
        }
    }
    return true;
}

//void CDesk::HandTypeJudge()
//{
//    if (PRE_FLOP == iCurrentRound_) //此时只有两张牌
//    {
//        return;
//    }
//    map<int, vector<CCardsInfo> > mapTotalCards; //保存每个玩家的牌与公共牌的组合
//
//    //将某玩家的底牌与公共牌一起放到mapTotalCards中
//    for (map<int, shared_ptr<CPlayer> >::iterator mapIt = mapPlayer_.begin(); mapIt != mapPlayer_.end(); ++mapIt)
//    {
//        vector<CCardsInfo> eachPlayerCards(vecCommunityCards_);
//        eachPlayerCards.push_back(mapIt->second->vecBottomCard_[0]);
//        eachPlayerCards.push_back(mapIt->second->vecBottomCard_[1]);
//        mapTotalCards.insert(make_pair(mapIt->first, eachPlayerCards));
//    }
//
//    //对所有玩家的牌进行判断，取出最大的牌型
//    CCombinationPlace cp1(iCurrentRound_ + 2, 5);
//    for (map<int, vector<CCardsInfo> >::iterator mapIt = mapTotalCards.begin(); mapIt != mapTotalCards.end(); ++ mapIt)
//    {
//        //将某一个玩家的牌组合成各种组合
//        vector<CCombination> vecCombination;
//        for (vector<vector<int> >::iterator placeIt = cp1.place_.begin(); placeIt != cp1.place_.end(); ++placeIt)
//        {
//            CCombination aCombination;
//            for (vector<int>::iterator eachPlaceIt = (*placeIt).begin(); eachPlaceIt != (*placeIt).end(); ++eachPlaceIt)
//            {
//                aCombination.cards_.push_back((mapIt->second)[*eachPlaceIt]);
//            }
//            vecCombination.push_back(aCombination);
//        }
//
//        //对各种组合进行牌型判断
//        for (vector<CCombination>::iterator comIt = vecCombination.begin(); comIt != vecCombination.end(); ++comIt)
//        {
//            GetCardsType(*comIt);
//        }
//
//        CCombination HighestHandType = vecCombination[0];
//        //选出最大的牌型
//        for (vector<CCombination>::iterator comIt = vecCombination.begin(); comIt != vecCombination.end(); ++comIt)
//        {
//            if (comIt->type_ > HighestHandType.type_)
//            {
//                HighestHandType = (*comIt);
//            }
//        }
//        mapPlayer_[mapIt->first]->CardsOptimumCombination_ = HighestHandType;
//    }
//}
//
//void CDesk::GetCardsType(CCombination& combination)
//{
//
//    //排序
//    sort(combination.cards_.begin(), combination.cards_.end(), [](const CCardsInfo &cards1, const CCardsInfo &cards2){return cards1.iNum_ < cards2.iNum_;});
//
//    //判断是否是顺子
//    int iCardsSeq = 0;
//    bool isFound = false;
//    for (iCardsSeq = 0; iCardsSeq < 4; ++ iCardsSeq) //每张牌依次跟后张牌比较
//    {
//        if (combination.cards_[iCardsSeq].iNum_ + 1 != combination.cards_[iCardsSeq + 1].iNum_)
//        {
//            break;
//        }
//    }
//
//    if (( 3 == iCardsSeq && CARD_2 == combination.cards_[0].iNum_ && CARD_A == combination.cards_[4].iNum_) //A12345
//        || 4 == iCardsSeq) //其他顺子
//    {
//        combination.type_ = STRAIGHT;
//        isFound = true;
//    }
//
//    //判断是否是同花
//    for (iCardsSeq = 0; iCardsSeq < 5; ++ iCardsSeq)
//    {
//        if (combination.cards_[0].iSuit_ != combination.cards_[iCardsSeq].iSuit_)
//        {
//            break;
//        }
//    }
//
//    if (5 == iCardsSeq)
//    {
//        combination.type_ = combination.type_ == STRAIGHT ? STRAIGHT_FLUSH : FLUSH;
//        isFound = true;
//    }
//
//    if (STRAIGHT_FLUSH == combination.type_  //同花顺
//        && CARD_T == combination.cards_[0].iNum_ && CARD_A == combination.cards_[5].iNum_) //TJQKA
//    {
//        combination.type_ = ROYAL_FLUSH; //皇家同花顺
//    }
//
//    if (isFound) //是顺子，同花，同花顺，皇家同花顺中的一种
//    {
//        return;
//    }
//
//    //非顺子，同花，同花顺，皇家同花顺中的任一种
//    int flag = 0;
//    for (int iIndexPre = 0; iIndexPre < combination.cards_.size(); ++ iIndexPre)
//    {
//        for (int iIndexAfter = iIndexPre + 1; iIndexAfter < combination.cards_.size(); ++iIndexAfter)
//        {
//            if (combination.cards_[iIndexPre].iNum_ == combination.cards_[iIndexAfter].iNum_)
//            {
//                ++flag;
//            }
//        }
//    }
//
//    switch (flag)
//    {
//        case 0: //高牌
//            combination.type_ = HIGH_CARD;
//        case 1: //一对
//            combination.type_ = ONE_PAIR;
//        case 2: //两对
//            combination.type_ = TWO_PAIR;
//        case 3: //三条
//            combination.type_ = THREE_OF_A_KIND;
//        case 4: //葫芦
//            combination.type_ = FULL_HOUSE;
//        case 6: //四条
//            combination.type_ = FOUR_OF_A_KIND;
//    }
//}
//
//void CCombinationPlace::calcPlace()
//{
//    int i = 0 ,j = 0;
//    int b[5] = {0};
//    while(b[0] < iTotalNumCount_ ) {
//        while( i < iTotalNumCount_) {
//            if(j == iChosenNumCount_ - 1 ) {
//                place_.push_back(vector<int>(b, b + iChosenNumCount_));
//            } else {
//                j ++;
//            }
//            i ++;
//            b[j] = i;
//        }
//        do {
//            j --;
//            b[j] ++;
//        } while(b[j] >= iTotalNumCount_ );
//        i = b[j];
//    }
//}

