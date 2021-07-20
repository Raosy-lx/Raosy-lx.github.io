#include "minitask_common_message_helper.h"

namespace minitask
{
// 键值记录
void BuildType(KeyValType* stMsgKV, string key, string val) {
    stMsgKV->set_key(key);
    if (!val.empty())
    {
        stMsgKV->set_val(val);
    }
}

// 构建键地址记录
void BuildType(KeyAddrType* stMsgKA, string key, string domain, string addr, string port) {
    stMsgKA->Clear();
    stMsgKA->set_key(key);
    AddrType *tsockaddr = stMsgKA->mutable_sockaddr();
    tsockaddr->set_domain(domain);
    tsockaddr->set_addr(addr);
    tsockaddr->set_port(port);
}

// 检查pb协议包的完整性
int check_pack_complete(const void* data, unsigned int length) {
    if (length < sizeof(ComHead) )
    {
        return 0;
    }

    if (length > _MAX_NEW_MSG_LENGTH_)
    {
        return -1;
    }

    ComHead* head = (ComHead*)data;
    if (ntohl(head->magicnum) != 0x20210720 ||
        (ntohs(head->type) != 0x1))
    {
        return -2;
    }

    if (length >= ntohl(head->length))
    {
        return ntohl(head->length);
    }
    return 0;
}

}