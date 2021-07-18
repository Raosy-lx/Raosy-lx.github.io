#include "./minitask_common_message.pb.h"
#include <iostream>
#include <fstream>

void set_keyvalpack()
{
    minitask::KeyValPack KVpackage;

    minitask::KeyValType *p1 = KVpackage.add_keyvaltmp(); //新增一对键值
    p1->set_key("mike");
    p1->set_val("mikekey");

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
    minitask::KeyValPack KVpackage;
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

int main(int argc, char* argv[]) {
    set_keyvalpack();
    std::cout << "set OK" << std::endl;
    get_keyvalpack();
    return 0;
}