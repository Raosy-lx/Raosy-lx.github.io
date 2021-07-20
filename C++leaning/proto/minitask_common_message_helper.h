
#ifndef _MINITASK_COMMON_MESSAGE_HELPER_H_
#define _MINITASK_COMMON_MESSAGE_HELPER_H_

#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <string>
#include <sstream>
#include <iostream>

#include "minitask_common_message.pb.h"
#include <arpa/inet.h>

using namespace std;

#define _MAX_NEW_MSG_LENGTH_ (30*1024*1024)

// 设置紧凑对齐
#pragma  pack(1)

// 数据包格式 ComHead + Body
typedef struct _ComHead
{
    uint32_t GetMagicNum() const
    {
        return ntohl(this->magicnum);
    }

    uint32_t GetBodyLen() const
    {
        return ntohl(this->body_length);
    }

    uint16_t GetProtoType() const
    {
        return ntohs(this->type);
    }

    void SetBodyLen(uint32_t iBodyLen)
    {
        this->body_length = htonl(iBodyLen);
    }

    void SetPkgLen(uint32_t iPkgLen)
    {
        this->length = htonl(iPkgLen);
    }

    uint32_t GetPkgLen() const
    {
        return ntohl(this->length);
    }

    uint32_t magicnum;          // 魔术字 用于防攻击 0x20210720
    uint16_t type;              // 协议类型 1: protobuf; 2: Json;
    uint32_t length;            // 总长度 (ComHead+Body)
    uint32_t body_length;       // body长度
    
}ComHead;

// 恢复默认对齐
#pragma pack()
namespace minitask
{
void BuildType(KeyValType* stMsgKV, string key, string val);
void BuildType(KeyAddrType* stMsgKA, string key, string domain, string addr, string port);

int check_pack_complete(const void* data, unsigned int length);     // pb包完整性检测

// 打包键地址表或键值表
template <typename T>
int pack_msg(string& strData, const T& stMsgTab) {
    string strMsgTab = "";
    if (!stMsgTab.SerializeToString(&strMsgTab))
    {
        return -1;
    }
    if (_MAX_NEW_MSG_LENGTH_ <= sizeof(ComHead) + strMsgTab.size())
    {
        return -2;
    }
    
    ComHead stHead;
    stHead.magicnum = htonl(0x20210720);
    stHead.type = htons(0x1);
    stHead.length = htonl(sizeof(ComHead) + strMsgTab.size());
    stHead.body_length = htonl(strMsgTab.size());

    strData.assign((char*)&stHead, sizeof(stHead));
    strData.append(strMsgTab);
    return 0;
}

// 解压包 键地址表或键值表
template <typename T>
int unpack_msg(const char* buffer, unsigned int buffer_length, T& stMsgTab) {
    if (buffer_length < sizeof(ComHead))
    {
        return -1;
    }

    ComHead *pstComHead = (ComHead*)buffer;
    uint32_t uiMsgBodyLen = pstComHead->GetBodyLen();
    if(pstComHead->GetMagicNum() != 0x20210720)
    {
        return -2;
    }
    if(uiMsgBodyLen > (buffer_length - sizeof(ComHead)))
    {
        return -3;
    }
    if(!stMsgTab.ParseFromArray(buffer + sizeof(ComHead), uiMsgBodyLen))
    {
        return -4;
    }
    return 0;
}

}

#endif
