#include "./minitask_common_message.pb.h"
#include <iostream>
#include <fstream>
#include <string.h>

void set_keyvalpack()
{
    minitask::KeyValTab KVpackage;

    minitask::KeyValType *p1 = KVpackage.add_keyvaltmp(); //新增一对键值
    p1->set_key("mike");
    //p1->set_val("mikekey");

    std::fstream output("pb.xxx", std::ios::out | std::ios::trunc | std::ios::binary);

    bool flag = KVpackage.SerializeToOstream(&output);//序列化
    if (!flag)
    {
        std::cerr << "Failed to write file." << std::endl;
        return;
    }

    output.close();//关闭文件
}

void get_keyvalpack()
{
    minitask::KeyValTab KVpackage;
    std::fstream input("./pb.xxx", std::ios::in | std::ios::binary);
    bool flag = KVpackage.ParseFromIstream(&input);  //反序列化
    if (!flag)
    {
        std::cerr << "Failed to read file." << std::endl;
        return;
    }
    input.close(); //关闭文件

    for (int i = 0; i < KVpackage.keyvaltmp_size(); i++)
    {
        const minitask::KeyValType& KVtmp = KVpackage.keyvaltmp(i);//取第i对keyvalue
        std::cout << "第" << i + 1 << "对键值：\n";
        std::cout << "key = " << KVtmp.key() << std::endl;
        std::cout << "val = " << KVtmp.val() << std::endl;
    }
}

std::string set_keyaddrtab()
{
    minitask::KeyAddrTab KApackage;

    minitask::KeyAddrType *p1 = KApackage.add_kadrecord(); //新增一对键地址
    minitask::AddrType *p1addr = new minitask::AddrType;
    p1addr->set_addr("127.0.0.1");
    p1addr->set_domain("AF_INET");
    p1addr->set_port("5574");
    p1->set_key("mike");
    p1->set_allocated_sockaddr(p1addr);

    //std::fstream output("pbKA.xxx", std::ios::out | std::ios::trunc | std::ios::binary);
    std::string output = "";
    bool flag = KApackage.SerializeToString(&output);//序列化
    if (!flag)
    {
        std::cerr << "Failed to write file." << std::endl;
        return "";
    }

    //output.close();//关闭文件
    return output;
}

void get_keyaddrtab(std::string input)
{
    minitask::KeyAddrTab KApackage;
    //std::fstream input("./pbKA.xxx", std::ios::in | std::ios::binary);
    bool flag = KApackage.ParseFromArray(&input, input.size());  //反序列化
    if (!flag)
    {
        std::cerr << "Failed to read file." << std::endl;
        return;
    }
    //input.close(); //关闭文件

    for (int i = 0; i < KApackage.kadrecord_size(); i++)
    {
        const minitask::KeyAddrType& KAtmp = KApackage.kadrecord(i);//取第i对keyaddress
        std::cout << "第" << i + 1 << "对键地址：\n";
        std::cout << "key = " << KAtmp.key() << std::endl;
        const minitask::AddrType& ADtmp = KAtmp.sockaddr();
        std::cout << "domain = " << ADtmp.domain() << std::endl;
        std::cout << "addr = " << ADtmp.addr() << std::endl;
        std::cout << "port = " << ADtmp.port() << std::endl;
    }
}



int main(int argc, char* argv[]) {
    set_keyvalpack();
    std::cout << "set OK" << std::endl;
    get_keyvalpack();
    std::cout << "get OK" << std::endl;
    std::string output = set_keyaddrtab();
    std::cout << "set OK" << std::endl;
    get_keyaddrtab(output);
    std::cout << "get OK" << std::endl;
    return 0;
}