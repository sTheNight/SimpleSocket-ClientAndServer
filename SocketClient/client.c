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

void recv_message(void* aID);

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
	// 创建客户端的 SOCKADDR
	SOCKADDR_IN sockaddr_client;
	sockaddr_client.sin_port = htons(PORT_NOW);
	sockaddr_client.sin_family = AF_INET;
	sockaddr_client.sin_addr.S_un.S_addr = inet_addr(IP_NOW);

	// 等待连接服务端
	printf("正在搜寻服务端...\n");
	while (connect(socket_r, (SOCKADDR*)&sockaddr_client, sizeof(sockaddr_client)) == SOCKET_ERROR)
	{
		// 检查 connect 是否返回 SOCKET_ERROR
		printf("连接失败，重试中...\n");
		Sleep(1000); // 等待1秒后重试
	}
	printf("已连上服务端, IP:%s\n\n", inet_ntoa(sockaddr_client.sin_addr));
	// 创建新线程接收消息
	_beginthread(recv_message, 0, (void*)socket_r);
	printf("发送服务已启动（%d）\n", socket_r);
	while (1)
	{
		char send_temp[sizeOfMessage];
		scanf("%[^\n]", send_temp); // 需要使用 %[^\n] 来控制输入格式，否则会遇到输入空格就中断的问题
		while (getchar() != '\n');
		int send_r = send(socket_r, send_temp, strlen(send_temp) + 1, 0);
		// 检查 send 是否返回 SOCKET_ERROR
		if (send_r == SOCKET_ERROR)
		{
			printf("服务端关闭连接\n");
			break;
		}
		if (strcmp(send_temp, ":quit") == 0)
		{
			printf("触发退出指令\n");
			break;
		}
		printf("发送已执行, 你发送的信息是：%s\n", send_temp);
	}

	printf("进程已退出，请输入任意键继续...\n");
	getch();
	closesocket(socket_r);
	WSACleanup();
	return 0;
}

void recv_message(void* aID)
{
	SOCKET socket_r = (SOCKET)aID;
	printf("接收服务已启动（%d）\n", socket_r);
	while (1)
	{
		char recv_temp[sizeOfMessage + 1];
		int recv_r = recv(socket_r, recv_temp, sizeOfMessage, 0);
		if (recv_r > 0)
		{
			if (strcmp(recv_temp, ":quit") != 0)
			{
				printf("len:%-3d, message:%s\n", recv_r, recv_temp);
				continue;
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
	closesocket(socket_r);
	_endthread();
}