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
	// ��ʼ��
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		printf("�޷�������������������˳�...\n");
		getch();
		return 0;
	}
	printf("wVersion: %d.%d\n", LOBYTE(wsaData.wVersion), HIBYTE(wsaData.wVersion));
	printf("wHighVersion: %d.%d\n", LOBYTE(wsaData.wHighVersion), HIBYTE(wsaData.wHighVersion));
	printf("szDescription: %s\n", wsaData.szDescription);
	printf("szSystemStatus: %s\n\n", wsaData.szSystemStatus);
	// �����׽���
	SOCKET socket_r = socket(AF_INET, SOCK_STREAM, 0);
	// ����׽����Ƿ񴴽��ɹ���INVALID_SOCKET
	if (socket_r != INVALID_SOCKET)
	{
		printf("�׽��ִ����ɹ���%d��\n", socket_r);
	}
	else
	{
		printf("�׽��ִ���ʧ��\n");
		WSACleanup();
		return 0;
	}
	// ��������˵� SOCKADDR
	SOCKADDR_IN sockaddr_n;
	sockaddr_n.sin_family = AF_INET;
	sockaddr_n.sin_port = htons(PORT_NOW);
	sockaddr_n.sin_addr.S_un.S_addr = inet_addr(IP_NOW);
	// �� sockaddr
	if (bind(socket_r, (SOCKADDR*)&sockaddr_n, sizeof(sockaddr_n)) == SOCKET_ERROR)
	{
		// ��� bind �Ƿ񷵻� SOCKET_ERROR
		printf("��ʧ��\n");
		closesocket(socket_r);
		WSACleanup();
		return 0;
	}
	// ��������������������
	if (listen(socket_r, 5) == SOCKET_ERROR)
	{
		// ��� listen �Ƿ񷵻� SOCKET_ERROR
		printf("����ʧ��\n");
		closesocket(socket_r);
		WSACleanup();
		return 0;
	}
	printf("������...\n");
	// ����һ�����ڼ�¼�ͻ�����Ϣ�� SOCKADDR
	SOCKADDR_IN sockaddr_client;
	int sockaddr_client_len = sizeof(sockaddr_client);
	// �ȴ����Ӳ��洢����ֵ
	SOCKET accept_r = accept(socket_r, (SOCKADDR*)&sockaddr_client, &sockaddr_client_len);
	// ��� accept �Ƿ񷵻� INVALID_SOCKET
	if (accept_r != INVALID_SOCKET)
	{
		printf("���µ�����,IP:%s\n\n", inet_ntoa(sockaddr_client.sin_addr));
	}
	else
	{
		printf("��������ʧ��\n");
		closesocket(socket_r);
		WSACleanup();
		return 0;
	}
	// �������̷߳�����Ϣ������ accept_r
	_beginthread(send_message, 0, (void*)accept_r);
	printf("���շ�����������%d��\n", socket_r);
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
				printf("���յ��˳�ָ��\n");
				break;
			}
		}
		else if (recv_r == 0)
		{
			printf("���ӹر�\n");
			break;
		}
	}
	closesocket(accept_r);
	closesocket(socket_r);
	WSACleanup();
	printf("�������˳����㰴���������...\n");
	getch();
	return 0;
}
void send_message(void* aID)
{
	// �˴���Ӧ����������Ϊ socket_r ��֮ǰ����ôд�ģ�ʡ�¾Ͳ�����
	SOCKET socket_r = (SOCKET)aID;
	printf("���ͷ�����������%d��\n", socket_r);
	while (1)
	{
		char send_temp[sizeOfMessage];
		scanf("%[^\n]", send_temp);
		while (getchar() != '\n');
		int send_r = send(socket_r, send_temp, strlen(send_temp) + 1, 0);
		// ��� send �����Ƿ񷵻� SOCKET_ERROR
		if (send_r == SOCKET_ERROR)
		{
			printf("����ʧ��\n");
			break;
		}
		printf("������ִ��, �㷢�͵���Ϣ�ǣ�%s\n", send_temp);
	}
	printf("�ѹر�\n");
	closesocket(socket_r);
	_endthread();
}