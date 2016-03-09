//
// Created by Zhang Shibo on 2016/3/1.
//

#include "EventProcess.h"
#include "Desk.h"
using namespace std;

bool EventProcess::Process(int iMsgId, const string& strInMsg, string& strOutProcessResult) {
    int iDeskNo;
    auto findDeskRet = mapDesk_.find(iDeskNo);
    if (mapDesk_.end() == findDeskRet) {
        //没有找到对应的桌子
        return false;
    }
    auto desk = findDeskRet->second;
    //处理相应的消息
    shared_ptr<CMsgNode> msgNode {new CMsgNode()};
    msgNode->SetMsgId(iMsgId);
    msgNode->SetMsg(strInMsg);
    if (desk->Process(msgNode)) {
        strOutProcessResult = msgNode->GetMsg();
        return true;
    } else {
        return false;
    }
}

