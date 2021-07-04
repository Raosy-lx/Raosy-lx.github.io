#include <WinSock2.h>
#include <stdio.h>
#include <Windows.h>
#pragma comment(lib,"WS2_32.lib")

int main()
{
	WSADATA data;
	WORD w = MAKEWORD(2,0);
	char sztext[] = "欢迎你\r\n";
	::WSAStartup(w,&data);
	SOCKET s,s1;
	s = ::socket(AF_INET, SOCK_STREAM, 0);
	sockaddr_in addr, addr2;
	int n = sizeof(addr2);
	addr.sin_family = AF_INET;
	addr.sin_port = htons(75);
	addr.sin_addr.S_un.S_addr = INADDR_ANY;
	::bind(s,(sockaddr*)&addr, sizeof(addr));
	::listen(s, 5);
	printf("服务器已经启动\r\n");

	while (true)
	{
		s1 = ::accept(s, (sockaddr*)&addr2, &n);
		if (s1!=NULL)
		{
			printf("%s 已经连接上\r\n", inet_ntoa(addr2.sin_addr));
			::send(s1, sztext, sizeof(sztext), 0);
		}
		::closesocket(s);
		::closesocket(s1);
		::WSACleanup();
		if (getchar())
		{
			return 0;
		}
		else
		{
			::Sleep(100);
		}
	}
	return 0;
}