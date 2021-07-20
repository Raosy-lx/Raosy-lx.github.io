TARGET = ./main
XX = g++
CFLAGS = -g --std=c++11
LIBS = -lprotobuf -lpthread
SOURCE = minitask_common_message_helper.cc minitask_common_message.pb.cc readmessage.cc

$(TARGET): $(SOURCE)
	$(XX) $^ -o $@ $(CFLAGES) $(LIBS)
