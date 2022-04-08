// Client.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#pragma once

#include <stdio.h>
#include <tchar.h>
//библитоека ввода и вывода
#include <iostream>
//доступ к некоторым функциям
#pragma comment(lib, "ws2_32.lib")
//подколючение бибилиотеки winsock для работы с сетью
#include <winsock2.h>
#include <WS2tcpip.h>
#include <string>

#pragma warning(disable: 4996)
SOCKET ConnectSocket = INVALID_SOCKET;
void ClientHandler() {
	int msg_size;
	while (true) {
		recv(ConnectSocket, (char*)&msg_size, sizeof(int), NULL);
		char* msg = new char[msg_size + 1];
		msg[msg_size] = '\0';
		recv(ConnectSocket, msg, msg_size, NULL);
		std::cout << msg << std::endl;
		delete[] msg;
	}
}

int main(int argc, char* argv[]) {
	//WSAStartup загрузка версии библиотеки
	WSAData wsaData;
	ADDRINFO hints;
	ADDRINFO* addrResult = NULL;
	int result;

	WORD DLLVersion = MAKEWORD(2, 1); //запрашиваемая версия библитоеки
	result = WSAStartup(DLLVersion, &wsaData);
	if (result != 0) { //загрузка бибилитотеки
		std::cout << "WAStartupp Error" << result << std::endl; //проверка удачности загрузки библиотеки
		exit(1);
	}

	ZeroMemory(&hints, sizeof(hints));

	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	//SOCKADDR_IN addr; //структура для хранения адреса
	//int sizeofaddr = sizeof(addr);
	result = getaddrinfo("192.168.1.107", "666", &hints, &addrResult);
	if (result != 0)
	{
		std::cout << "Get addrindo failed with error" << result <<std::endl; //проверка удачности загрузки библиотеки
		WSACleanup();
		exit(1);
	}

	ConnectSocket = socket(addrResult->ai_family, addrResult->ai_socktype, addrResult->ai_protocol);
	if (ConnectSocket == INVALID_SOCKET) {
		std::cout << "Socket creation failed" << result << std::endl; //проверка удачности загрузки библиотеки
		freeaddrinfo(addrResult);
		WSACleanup();
		exit(1);
	}
	//addr.sin_addr.s_addr = inet_addr("192.168.208.1"); //хранит IP
	//addr.sin_port = htons(1111); //порт для идентификации
	//addr.sin_family = AF_INET; //семейство протоколов
	//Connection = socket(AF_INET, SOCK_STREAM, NULL); //создание сокета
	result = connect(ConnectSocket, addrResult->ai_addr, (int)addrResult->ai_addrlen);
	if (result == SOCKET_ERROR) { //проверка подключения
		std::cout << "Error: failed connect to server.\n";
		closesocket(ConnectSocket);
		ConnectSocket = INVALID_SOCKET;
		freeaddrinfo(addrResult);
		WSACleanup();
		return 1;
	}
	std::cout << "Connected!\n"; //вывод сообщения о подключении
	//создание потока для чтения сообщений с сервера
	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientHandler, NULL, NULL, NULL);

	std::string msg1;
	while (true) {
		std::getline(std::cin, msg1);
		int msg_size = msg1.size();
		send(ConnectSocket, (char*)&msg_size, sizeof(int), NULL);
		send(ConnectSocket, msg1.c_str(), msg_size, NULL);
		Sleep(10);
	}

	system("pause");
	return 0;
}