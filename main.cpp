#include <iostream>
#include <algorithm>
//#include "Pot.h"
#include "HandTypeJudge.h"
#include "TexasPoker.h"
#include <stdio.h>
#include <stdlib.h>
using namespace std;

int main()
{
    //CTotalPot pot;
    //pot.AddChip(0, 18);
    //pot.AddChip(1, 13);
    //pot.AddChip(2, 18);
    //pot.AddChip(3, 5);
    //pot.Print();
    //pot.PlayerFold(3);

    //pot.CalcPot();
    //pot.Print();

    vector<Poker::CCardsInfo> vecCards {{5, Poker::Suit::CLUB},
                                        {5, Poker::Suit::HEART },
                                        {5, Poker::Suit::CLUB },
                                        {7, Poker::Suit::CLUB },
                                        {10, Poker::Suit::DIAMOND },
                                        {6, Poker::Suit::CLUB},
                                        {9, Poker::Suit::CLUB}};

    Poker::CCombination result;
    CHandTypeJudge* handTypeJudge = CHandTypeJudge::GetInstance();
    bool ret = handTypeJudge->GetBestCombinationFromSevenCards(vecCards, result);
    if (ret) {
        cout << "Hand num: ";
        for_each(result.cards_.begin(), result.cards_.end(), [](const Poker::CCardsInfo& Cards){ cout << Cards.iNum_ << " "; });
        cout << endl;
        cout << "Hand Type: " << result.type_ << endl;
    }
    return 0;
}