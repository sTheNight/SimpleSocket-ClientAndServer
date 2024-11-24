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
	// �����ͻ��˵� SOCKADDR
	SOCKADDR_IN sockaddr_client;
	sockaddr_client.sin_port = htons(PORT_NOW);
	sockaddr_client.sin_family = AF_INET;
	sockaddr_client.sin_addr.S_un.S_addr = inet_addr(IP_NOW);

	// �ȴ����ӷ����
	printf("������Ѱ�����...\n");
	while (connect(socket_r, (SOCKADDR*)&sockaddr_client, sizeof(sockaddr_client)) == SOCKET_ERROR)
	{
		// ��� connect �Ƿ񷵻� SOCKET_ERROR
		printf("����ʧ�ܣ�������...\n");
		Sleep(1000); // �ȴ�1�������
	}
	printf("�����Ϸ����, IP:%s\n\n", inet_ntoa(sockaddr_client.sin_addr));
	// �������߳̽�����Ϣ
	_beginthread(recv_message, 0, (void*)socket_r);
	printf("���ͷ�����������%d��\n", socket_r);
	while (1)
	{
		char send_temp[sizeOfMessage];
		scanf("%[^\n]", send_temp); // ��Ҫʹ�� %[^\n] �����������ʽ���������������ո���жϵ�����
		while (getchar() != '\n');
		int send_r = send(socket_r, send_temp, strlen(send_temp) + 1, 0);
		// ��� send �Ƿ񷵻� SOCKET_ERROR
		if (send_r == SOCKET_ERROR)
		{
			printf("����˹ر�����\n");
			break;
		}
		if (strcmp(send_temp, ":quit") == 0)
		{
			printf("�����˳�ָ��\n");
			break;
		}
		printf("������ִ��, �㷢�͵���Ϣ�ǣ�%s\n", send_temp);
	}

	printf("�������˳������������������...\n");
	getch();
	closesocket(socket_r);
	WSACleanup();
	return 0;
}

void recv_message(void* aID)
{
	SOCKET socket_r = (SOCKET)aID;
	printf("���շ�����������%d��\n", socket_r);
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
	closesocket(socket_r);
	_endthread();
}