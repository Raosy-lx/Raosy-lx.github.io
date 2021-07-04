#include <WinSock2.h>
#include <stdio.h>
#include <Windows.h>
#pragma comment(lib,"WS2_32.lib")

int main()
{
	WSADATA data;
	WORD w = MAKEWORD(2,0);
	::WSAStartup(w,&data);
	SOCKET s;
	s = ::socket(AF_INET, SOCK_DGRAM, 0);
	sockaddr_in addr, addr2;
	int n = sizeof(addr2);
	char buff[10] = {0};
	addr.sin_family = AF_INET;
	addr.sin_port = htons(75);
	addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	printf("客户端已经启动\r\n");
	char sztext[] = "你好\r\n";
	if (::sendto(s, sztext, sizeof(sztext), 0, (sockaddr*)&addr, n)!=0)
	{
		::recvfrom(s, buff, sizeof(buff), 0, (sockaddr*)&addr, &n);
		printf("服务器说 %s\r\n", buff);
		::closesocket(s);
		::WSACleanup();
	}
	if (getchar())
	{
		return 0;
	}
	else
	{
		::Sleep(100);
	}
	return 0;
}