//
// Created by Zhang Shibo on 2016/3/1.
//

#ifndef DEZHOU_SERVER_EVENTPROCESS_H
#define DEZHOU_SERVER_EVENTPROCESS_H
#include <map>
#include <memory>
#include <string>

class CDesk;
class CMsgNode {
public:
    CMsgNode(){}
    ~CMsgNode(){}

    void SetMsgId(int iMsgId) { iMsgId_ = iMsgId; }
    int GetMsgId() { return iMsgId_; }

    void SetMsg(const std::string& strMsg) { strMsg_ = strMsg; }
    std::string GetMsg() { return strMsg_; }

private:
    int iMsgId_;
    std::string strMsg_;
};

class EventProcess {
public:
    bool Process(int iMsgId, const std::string& strInMsg, std::string& strOutProcessResult);
private:
    typedef std::map<int, std::shared_ptr<CDesk>> deskMap;
    deskMap mapDesk_;
};


#endif //DEZHOU_SERVER_EVENTPROCESS_H
