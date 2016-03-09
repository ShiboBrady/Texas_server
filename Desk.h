//
// Created by Zhang Shibo on 2016/3/1.
//
#ifndef __DESK_H__
#define __DESK_H__

#include <map>
#include <list>
#include <vector>
#include <queue>
#include <string>
#include <memory>
#include "TexasPoker.h"
#include "Player.h"
#include "Pot.h"

class CMsgNode;
class CHandTypeJudge;
class CDesk
{
public:
    typedef std::map<int, std::shared_ptr<CPlayer>> playerMap;
    typedef std::list<std::shared_ptr<CPlayer>> observerPlayerList;
    CDesk(int iMaxPlayerNum, int iSmallBlindBet);
    ~CDesk();

    bool Process(std::shared_ptr<CMsgNode> msgNode ); //处理玩家消息

private:
    bool InitWhenDeskInit(); //桌子初始化时要初始化参数
    bool InitWhenNewRound(); //每开始新一局时初始化参数
    bool AddPlayer(); //增加玩家
    void StartNewRoundGame(); //开始一轮新的比赛
    int FindNextPlayerPosition(int iCurrentSeatNo); //查找下一个玩家的位置
    int FindNextValidPlayerPosition(int iCurrentSeatNo); //查找下一个有效玩家的位置，不包括弃牌的玩家和全下的玩家
    playerMap& GetPlayer() { return this->mapPlayer_; }
    bool PlayerCheck(int iPlayerSeatNo); //玩家让牌
    bool PlayerCall(int iPlayerSeatNo, int iChip);  //玩家跟注
    bool PlayerFold(int iPlayerSeatNo);  //玩家弃牌
    bool PlayerRaise(int iPlayerSeatNo, int iChip); //玩家加注
    bool PlayerAllIn(int iPlayerSeatNo, int iChip); //玩家全下
    bool PlayerCheckReview(int iPlayerSeatNo); //玩家让牌规则检查
    bool PlayerCallReview(int iPlayerSeatNo, int iChip);  //玩家跟注规则检查
    bool PlayerFoldReview(int iPlayerSeatNo);  //玩家弃牌规则检查
    bool PlayerRaiseReview(int iPlayerSeatNo, int iChip); //玩家加注规则检查
    bool PlayerAllInReview(int iPlayerSeatNo, int iChip); //玩家全下规则检查
    bool IsThisRoundFinished(int iCurrentPos, Poker::PlayerStatus status); //判断一轮是否结束
    bool CalcPlayersCardsType();

    Poker::Round iCurrentRound_; //当前回合数
    std::array<Poker::CCardsInfo, 5> arrCommunityCards_; //五张公共牌
    std::queue<int> qPosition_; //玩家的位置
    int iMaxPlayerNum_; //最多可容纳的玩家人数
    int iSmallBlindBet_; //小盲的固定下注数
    int iBankerPos_; //庄家位置
    int iSmallBlindPos_; //小盲位置
    int iBigBlindPos_; //大盲位置
    int iStartPos_;  //每一轮开始那个人的位置，或者是加注那个人的位子，在第一轮中，如果没有人加注，是大盲的位子
    int iCurrentMaxChip_; //当前轮中最大的注
    int iLastValidPos_; //最后一个下注有效位，要么是加注，要么是allin是当局最大的，要么是跟注，弃牌或者allin不够大的不算
    int iExpectPos_; //期望的下一个玩家位置，同时也是每次返回给前端的数据
    playerMap mapPlayer_; //当前玩家
    observerPlayerList listObserverPlayer_; //当前观望的玩家
    CTotalPot pot_; //当前的总池
    CHandTypeJudge* handTypeJudge_; //牌型的计算算法
};

#endif /*__DESK_H__*/