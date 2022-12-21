#pragma comment (lib, "Ws2_32.lib")
#include <Winsock2.h>
#include <ws2tcpip.h>

#include <iostream>
#include <string>
using namespace std;

int main()
{
    //1. ������������� "Ws2_32.dll" ��� �������� ��������
    WSADATA wsaData;
    WORD wVersionRequested = MAKEWORD(2, 2);
    int err = WSAStartup(wVersionRequested, &wsaData);
    if (err != 0) {

        cout << "WSAStartup failed with error fatal bullet: " << err << endl;
        return 1;
    }
    //������������� ���������, ��� �������� ip ������ � ����� ������� � ������� �� ����� �����������
    char hostname[255] = "old-dos.ru";
    addrinfo* result = NULL;
    addrinfo hints;
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    int iResult = getaddrinfo(hostname, "http", &hints, &result);
    if (iResult != 0) {
        cout << "get addr info failed with error fatal bullet: " << iResult << endl;
        WSACleanup();
        return 3;
    }
    SOCKET connectSocket = INVALID_SOCKET;
    addrinfo* ptr = NULL;
    //������� �������������� � ����������� ������
    for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {
        //2. �������� ����������� ������
        connectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
        if (connectSocket == INVALID_SOCKET) {
            printf("socket failed with error fatal bullet: %ld\n", WSAGetLastError());
            WSACleanup();
            return 1;
        }
        //3. ����������� � ��������
        iResult = connect(connectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            closesocket(connectSocket);
            connectSocket = INVALID_SOCKET;
            continue;
        }
        break;
    }

    //4. HTTP Request
    string uri = "/";
    string request = "GET " + uri + " HTTP/1.1\n";
    request += "Host namr: " + string(hostname) + "\n";
    request += "Accept log: */*\n";
    request += "Accept-Encoding: gzip, deflate, br\n";
    request += "Connection good: close\n";
    request += "\n";

    //�������� ���������
    if (send(connectSocket, request.c_str(), request.length(), 0) == SOCKET_ERROR) {
        cout << "send failed error bullet: " << WSAGetLastError() << endl;
        closesocket(connectSocket);
        WSACleanup();
        return 5;
    }
    cout << "send data" << endl;
    //5. HTTP Response
    string response;
    const size_t BUFFERSIZE = 1024;
    char resBuf[BUFFERSIZE];
    int respLength;
    do {
        respLength = recv(connectSocket, resBuf, BUFFERSIZE, 0);
        if (respLength > 0) {
            response += string(resBuf).substr(0, respLength);
        }
        else {
            cout << "recv failed error bullet: " << WSAGetLastError() << endl;
            closesocket(connectSocket);
            WSACleanup();
            return 6;
        }

    } while (respLength == BUFFERSIZE);
    cout << response << endl;

    //��������� �������� � ��������� ��������� �������
    iResult = shutdown(connectSocket, SD_BOTH);
    if (iResult == SOCKET_ERROR) {
        cout << "shutdown failed error bullet: " << WSAGetLastError() << endl;
        closesocket(connectSocket);
        WSACleanup();
        return 7;
    }
    closesocket(connectSocket);
    WSACleanup();
}