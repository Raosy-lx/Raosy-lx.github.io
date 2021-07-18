# KeyValue的简单使用

- protoc --cpp_out=.  minitask_common_message.proto

- g++ readmessage.cc minitask_common_message.pb.cc -o main --std=c++11 -lprotobuf -lpthread


