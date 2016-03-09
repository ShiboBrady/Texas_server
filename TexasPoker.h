//
// Created by Zhang Shibo on 2016/3/1.
//
#ifndef __TEXASPOKER_H__
#define __TEXASPOKER_H__

#include <vector>
#include <map>

namespace Poker
{
enum HandType        //牌型
{
    HIGH_CARD = 1,   //高牌
    ONE_PAIR,        //一对
    TWO_PAIR,        //两对
    THREE_OF_A_KIND, //三条
    STRAIGHT,        //顺子
    FLUSH,           //同花
    FULL_HOUSE,      //葫芦
    FOUR_OF_A_KIND,  //四条
    STRAIGHT_FLUSH,  //同花顺
    ROYAL_FLUSH,     //皇家同花顺
};

enum Round     //回合数
{
    PRE_FLOP,  //翻牌前
    FLOP = 3,  //翻牌
    TURN,      //转牌
    RIVER,     //河牌
};

enum CardPoint
{
    CARD_2 = 2,
    CARD_3,
    CARD_4,
    CARD_5,
    CARD_6,
    CARD_7,
    CARD_8,
    CARD_9,
    CARD_T,
    CARD_J,
    CARD_Q,
    CARD_K,
    CARD_A, //14
};

enum Suit     //花色
{
    CLUB,     //梅花
    DIAMOND,  //方块
    HEART,    //红桃
    SPADE     //黑桃
};

enum PlayerStatus //玩家状态
{
    READY,        //坐下
    CHECK,        //让牌
    CALL,         //跟注
    RAISE,        //加注
    FOLD,         //弃牌
    ALLIN,        //全下
    WAITING,      //观看
};

struct CCardsInfo //每张牌 包括数值与花色
{
    int iNum_;   //牌值
    Suit iSuit_; //花色
};

struct CCombination  //五张牌 及其 牌型
{
    std::vector<CCardsInfo> cards_; //牌
    HandType type_; //牌型
};

}
#endif /*__TEXASPOKER_H__*/