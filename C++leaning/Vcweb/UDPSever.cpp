#include <WinSock2.h>
#include <stdio.h>
#include <Windows.h>
#pragma comment(lib,"WS2_32.lib")

int main()
{
	WSADATA data;
	WORD w = MAKEWORD(2,0);
	char sztext[] = "��ӭ��\r\n";
	::WSAStartup(w,&data);
	SOCKET s;
	s = ::socket(AF_INET, SOCK_DGRAM, 0);
	sockaddr_in addr, addr2;
	int n = sizeof(addr2);
	char buff[10] = {0};
	addr.sin_family = AF_INET;
	addr.sin_port = htons(75);
	addr.sin_addr.S_un.S_addr = INADDR_ANY;
	::bind(s,(sockaddr*)&addr, sizeof(addr));
	printf("UDP�������Ѿ�����\r\n");

	while (true)
	{
		if (::recvfrom(s, buff, 10, 0, (sockaddr*)&addr2, &n)!=0)
		{
			printf("%s �Ѿ�������\r\n", inet_ntoa(addr2.sin_addr));
			printf("%s\r\n", buff);
			::sendto(s, sztext, sizeof(sztext), 0, (sockaddr*)&addr2, n);
			break;
		}
	}
	::closesocket(s);
	::WSACleanup();
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