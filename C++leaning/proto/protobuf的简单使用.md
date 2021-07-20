# Protobuf的简单使用

- protoc --cpp_out=.  minitask_common_message.proto

- g++ readmessage.cc minitask_common_message.pb.cc -o main --std=c++11 -lprotobuf -lpthread

#### 1、helper的作用

- 定义了文件的commonhead，包含了打包标记，总长度以及body长度
- 定义了packmsg函数，用于对结构化数据进行序列化以及加commonhead打包，打包后文件可用于网络传输发送
- 定义了check_msg_complete函数，用于对msg长度实行判定，以及msg的类型是否符合预期
- 定义了unpackmsg函数，用于对序列化的数据读取，读取到结构化数据中，接收网络传输包，而后对其解包读取数据


