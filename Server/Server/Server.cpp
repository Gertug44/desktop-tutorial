#pragma once

#include <stdio.h>
#include <tchar.h>
//библитоека ввода и вывода
#include <iostream>
//доступ к некоторым функциям
#pragma comment(lib, "ws2_32.lib")
//подколючение бибилиотеки winsock для работы с сетью
#include <winsock2.h>
#include <string>
#include <WS2tcpip.h>


#pragma warning(disable: 4996)
SOCKET Connections[100]; //клиенты
int Counter = 0; //индекс соединения (для идентификациии)

//прослушивание и отправка сообщений
void ClientHandler(int index) {
	int msg_size;
	while (true) {
		//recv(Connections[index], msg, sizeof(msg), NULL); //получение сообщения
		int bytw = recv(Connections[index], (char*)&msg_size, sizeof(int), NULL);
		if (bytw < 1) {
			std::string indexstr = std::to_string(Connections[index]);
			std::string str = "User number " + indexstr + " disconnect";
			int mes_size = sizeof(str);
			std::cout << Connections[index] << " " << bytw << " Disconnect\n";
			for (int i = 0; i < Counter; i++) {
				if (i == index) { continue; }
				if (Connections[i] != 0) {
					send(Connections[i], (char*)&mes_size, sizeof(int), NULL);
					send(Connections[i], str.c_str(), sizeof(str), NULL);
				}
			}
			closesocket(Connections[index]);
			Connections[index] = NULL;
			break;
		}
		else{
			char* msg = new char[msg_size + 1];
			msg[msg_size] = '\0';
			std::string indexstr = std::to_string(Connections[index]);
			int bytesRecive = recv(Connections[index], msg, msg_size, NULL);
			std::cout << Connections[index] << " " << bytesRecive << " write a message in chat\n";
			//std::string str1= "Hello, i'm new server. I recive youre message, but now i cant work up with it";
			//int msg_size = str1.size();
			//send(Connections[index], (char*)&msg_size, sizeof(int), NULL);
			//send(Connections[index], str1.c_str(), msg_size, NULL); //отправка сообщения
			std::string str = "User number " + indexstr + ": " + msg;
			int mes_size = str.size();
			for (int i = 0; i < Counter; i++) {
				if (i == index) {
					continue;
				}
				if (Connections[i] != 0) {
					send(Connections[i], (char*)&mes_size, sizeof(int), NULL);
					send(Connections[i], str.c_str(), mes_size, NULL);
				}		
			}
		}
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
	hints.ai_flags = AI_PASSIVE;

	//SOCKADDR_IN addr; //структура для хранения адреса
	//int sizeofaddr = sizeof(addr);
	result = getaddrinfo(NULL, "666", &hints, &addrResult);
	if (result != 0)
	{
		std::cout << "Get addrinfo failed with error" << result << std::endl; //проверка удачности загрузки библиотеки
		WSACleanup();
		exit(1);
	}

	SOCKET ConnectSocket = socket(addrResult->ai_family, addrResult->ai_socktype, addrResult->ai_protocol);
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
	result = bind(ConnectSocket, addrResult->ai_addr, (int)addrResult->ai_addrlen);
	if (result == SOCKET_ERROR) { //проверка подключения
		std::cout << "Error: binding socket failed.\n";
		closesocket(ConnectSocket);
		ConnectSocket = INVALID_SOCKET;
		freeaddrinfo(addrResult);
		WSACleanup();
		return 1;
	}

	//SOCKADDR_IN addr; //структура для хранения адреса
	//int sizeofaddr = sizeof(addr);
	//addr.sin_addr.s_addr = inet_addr("127.0.0.1"); //хранит IP
	//addr.sin_addr.s_addr = inet_addr("192.168.208.1"); //хранит IP
	//addr.sin_port = htons(666); //порт для идентификации
	//addr.sin_family = AF_INET; //семейство протоколов

	//SOCKET sListen = socket(AF_INET, SOCK_STREAM, NULL); //создание сокета
	//семейство протоколов, указание соединения
	//bind(ConnectSocket, addrResult->ai_addr, (int)addrResult->ai_addrlen); //привязка адреса сокету
	//сокет, указатель на структуру созданную, размерность 
	result = listen(ConnectSocket, SOMAXCONN);
	if (result == SOCKET_ERROR) { //проверка подключения
		std::cout << "Error: listening socket failed.\n";
		closesocket(ConnectSocket);
		freeaddrinfo(addrResult);
		WSACleanup();
		return 1;
	}

	//listen(sListen, SOMAXCONN); //прослушивание
	//сокет созданный, максимально допкустимое число запросов ожидаемых обработки

	SOCKET newConnection; //сокет для удержание соеднинения с клиентом
	for (int i = 0; i < 100; i++) {
		newConnection = accept(ConnectSocket, NULL, NULL);//возвращает указатель на новый сокет
		//результат содержит сведение о клиенте - IP 2 параметр

		if (newConnection == 0) {
			std::cout << "Error #2\n";
		}
		else {
			std::cout << "Client Connected! " << newConnection << "\n"; //вывод о соединении
			std::string str= "Hello. Youre number is "+ std::to_string(newConnection);
			int msg_size = (int)str.size();
			send(newConnection, (char*)&msg_size, sizeof(int), NULL);
			send(newConnection, str.c_str(), msg_size, NULL); //отправка сообщения
			for (int i = 0; i < Counter; i++) {
				if (Connections[i] != 0) {
					std::string str = "User number " + std::to_string(newConnection) + " join to the chat";
					int mes_size = sizeof(str);
					send(Connections[i], (char*)&mes_size, sizeof(int), NULL);
					send(Connections[i], str.c_str(), sizeof(str), NULL);
				}
			}

			Connections[i] = newConnection; //добавление нового клиента
			Counter++; //увеличение индекса

			//создание потока с прослушиванием сообщений
			//по итогу принимаются новые сообщения и ожидаются соообщения
			//новый клиент -> новый поток прослушивания клиента
			CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientHandler, (LPVOID)(i), NULL, NULL);
			//парметры , 3)указатель на процедуру  4)аргумент переданный предыдущим 
		}
	}

	system("pause");
	return 0;
}

