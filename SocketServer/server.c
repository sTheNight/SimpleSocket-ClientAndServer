#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#include <stdio.h>
#include <string.h>
#include <process.h>

#define PORT_NOW 11451
#define IP_NOW "127.0.0.1"
#define sizeOfMessage 100

void send_message(void* aID);

int main()
{
	// 初始化
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		printf("无法启动服务，请点击任意键退出...\n");
		getch();
		return 0;
	}
	printf("wVersion: %d.%d\n", LOBYTE(wsaData.wVersion), HIBYTE(wsaData.wVersion));
	printf("wHighVersion: %d.%d\n", LOBYTE(wsaData.wHighVersion), HIBYTE(wsaData.wHighVersion));
	printf("szDescription: %s\n", wsaData.szDescription);
	printf("szSystemStatus: %s\n\n", wsaData.szSystemStatus);
	// 创建套接字
	SOCKET socket_r = socket(AF_INET, SOCK_STREAM, 0);
	// 检查套接字是否创建成功，INVALID_SOCKET
	if (socket_r != INVALID_SOCKET)
	{
		printf("套接字创建成功（%d）\n", socket_r);
	}
	else
	{
		printf("套接字创建失败\n");
		WSACleanup();
		return 0;
	}
	// 声明服务端的 SOCKADDR
	SOCKADDR_IN sockaddr_n;
	sockaddr_n.sin_family = AF_INET;
	sockaddr_n.sin_port = htons(PORT_NOW);
	sockaddr_n.sin_addr.S_un.S_addr = inet_addr(IP_NOW);
	// 绑定 sockaddr
	if (bind(socket_r, (SOCKADDR*)&sockaddr_n, sizeof(sockaddr_n)) == SOCKET_ERROR)
	{
		// 检查 bind 是否返回 SOCKET_ERROR
		printf("绑定失败\n");
		closesocket(socket_r);
		WSACleanup();
		return 0;
	}
	// 监听并最多允许五个连接
	if (listen(socket_r, 5) == SOCKET_ERROR)
	{
		// 检查 listen 是否返回 SOCKET_ERROR
		printf("监听失败\n");
		closesocket(socket_r);
		WSACleanup();
		return 0;
	}
	printf("监听中...\n");
	// 声明一个用于记录客户端信息的 SOCKADDR
	SOCKADDR_IN sockaddr_client;
	int sockaddr_client_len = sizeof(sockaddr_client);
	// 等待连接并存储返回值
	SOCKET accept_r = accept(socket_r, (SOCKADDR*)&sockaddr_client, &sockaddr_client_len);
	// 检查 accept 是否返回 INVALID_SOCKET
	if (accept_r != INVALID_SOCKET)
	{
		printf("有新的连接,IP:%s\n\n", inet_ntoa(sockaddr_client.sin_addr));
	}
	else
	{
		printf("接受连接失败\n");
		closesocket(socket_r);
		WSACleanup();
		return 0;
	}
	// 创建新线程发送消息，传递 accept_r
	_beginthread(send_message, 0, (void*)accept_r);
	printf("接收服务已启动（%d）\n", socket_r);
	while (1)
	{
		char recv_cache[sizeOfMessage];
		int recv_r = recv(accept_r, recv_cache, sizeOfMessage, 0);
		if (recv_r > 0)
		{
			if (strcmp(recv_cache, ":quit") != 0)
			{
				printf("len:%-3d, message:%s\n", recv_r, recv_cache);
			}
			else
			{
				printf("接收到退出指令\n");
				break;
			}
		}
		else if (recv_r == 0)
		{
			printf("连接关闭\n");
			break;
		}
	}
	closesocket(accept_r);
	closesocket(socket_r);
	WSACleanup();
	printf("程序已退出，点按任意键继续...\n");
	getch();
	return 0;
}
void send_message(void* aID)
{
	// 此处不应将变量命名为 socket_r 但之前是这么写的，省事就不改了
	SOCKET socket_r = (SOCKET)aID;
	printf("发送服务已启动（%d）\n", socket_r);
	while (1)
	{
		char send_temp[sizeOfMessage];
		scanf("%[^\n]", send_temp);
		while (getchar() != '\n');
		int send_r = send(socket_r, send_temp, strlen(send_temp) + 1, 0);
		// 检查 send 函数是否返回 SOCKET_ERROR
		if (send_r == SOCKET_ERROR)
		{
			printf("发送失败\n");
			break;
		}
		printf("发送已执行, 你发送的信息是：%s\n", send_temp);
	}
	printf("已关闭\n");
	closesocket(socket_r);
	_endthread();
}