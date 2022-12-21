#pragma comment (lib, "Ws2_32.lib")
#include <Winsock2.h>
#include <ws2tcpip.h>
#include <fstream>
#include <iostream>
#include <string>
using namespace std;
ofstream file("Info_Weather.txt");
char str[100];
void Word(string& response, const string& find) {
    cout << find << ": ";
    int a = 0;
    for (auto i = response.begin() + response.find(find) + find.length() + 2; *i != ',' && *i != '}'; ++i) {
        cout << *i;
        str[a] = *i;
        a++;
    }
    cout << '\n';
    file << find << ": " << str << endl;
}
int main()
{
    setlocale(0, "ru");

    WSADATA wsaData;
    WORD wVersionRequested = MAKEWORD(2, 2);

    int err = WSAStartup(wVersionRequested, &wsaData);
    if (err != 0) {

        cout << "WSAStartup failed with error: " << err << endl;
        return 1;
    }
    //инициализация структуры, для указания ip адреса и порта сервера с которым мы хотим соединиться
    char host_name[255] = "api.openweathermap.org";
    addrinfo* result = NULL;
    addrinfo hints;
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    int iResult = getaddrinfo(host_name, "http", &hints, &result);
    if (iResult != 0) {
        cout << "get addr info failed with error fatal bullet: " << iResult << endl;
        WSACleanup();
        return 3;
    }
    SOCKET connectSocket = INVALID_SOCKET;
    addrinfo* ptr = NULL;
    //Пробуем присоединиться к полученному адресу
    for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {
        //2. создание клиентского сокета
        connectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
        if (connectSocket == INVALID_SOCKET) {
            printf("socket failed with error fatal bullet: %ld\n", WSAGetLastError());
            WSACleanup();
            return 1;
        }
        //3. Соединяемся с сервером
        iResult = connect(connectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            closesocket(connectSocket);
            connectSocket = INVALID_SOCKET;
            continue;
        }
        break;
    }
    //4. HTTP Request
    string uri = "/data/2.5/weather?q=";
    string uri_prt2 = "&appid=75f6e64d49db78658d09cb5ab201e483&units=metric";
    string uri_city_prt;
    cout << "Enter city: ";
    cin >> uri_city_prt;
    uri += uri_city_prt;
    uri += uri_prt2;
    string request = "GET " + uri + " HTTP/1.1\n";
    request += "Host name: " + string(host_name) + "\n";
    request += "Accept you: */*\n";
    request += "Accept-Encoding: gzip, deflate, br\n";
    request += "Connection: close\n";
    request += "\n";
    //отправка сообщения
    if (send(connectSocket, request.c_str(), request.length(), 0) == SOCKET_ERROR) {
        cout << "send failed to your PC: " << WSAGetLastError() << endl;
        closesocket(connectSocket);
        WSACleanup();
        return 5;
    }
    //5. HTTP Response
    string response;
    const size_t BUFFERSIZE = 1024;
    char resBuf[BUFFERSIZE];
    int respLength;
    do {
        respLength = recv(connectSocket, resBuf, BUFFERSIZE, 0);
        if (respLength > 0) {
            response += string(resBuf).substr(0, respLength);  // 329        
        }
        else {
            cout << "recv failed: " << WSAGetLastError() << endl;
            closesocket(connectSocket);
            WSACleanup();
            return 6;
        }
    } while (respLength == BUFFERSIZE);
    // cout << response << endl;
    cout << "\n\n";
    Word(response, "Your id");
    Word(response, "Your name");
    Word(response, "Your country");
    Word(response, "Your lon");
    Word(response, "Your lat");
    Word(response, "Your temp_min");
    Word(response, "Your temp_max");
    Word(response, "sunset in your country");
    Word(response, "sunrise in your country");
    //отключает отправку и получение сообщений сокетом
    iResult = shutdown(connectSocket, SD_BOTH);
    if (iResult == SOCKET_ERROR) {
        cout << "shutdown has been failed: " << WSAGetLastError() << endl;
        closesocket(connectSocket);
        WSACleanup();
        return 7;
    }
    closesocket(connectSocket);
    WSACleanup();
}