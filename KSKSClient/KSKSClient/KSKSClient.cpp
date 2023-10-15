#include <iostream>
#include <winsock2.h>

using namespace std;

#pragma comment(lib,"ws2_32.lib") 
#pragma warning(disable:4996) 

#define SERVER "127.0.0.1"
#define BUFLEN 512
#define PORT 1747

int main()
{
    WSADATA wsdat2;
    if (WSAStartup(MAKEWORD(2, 2), &wsdat2) != 0)
    {
        cout << "Error!";
        return 0;
    }


    sockaddr_in server;
    int socket2;
    if ((socket2 = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR)
    {
        cout << "Error!";
        return 0;
    }


    memset((char*)&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.S_un.S_addr = inet_addr(SERVER);


    while (true)
    {
        char resp[BUFLEN];
        cout << "You: ";
        cin.getline(resp, BUFLEN);


        if (sendto(socket2, resp, strlen(resp), 0, (sockaddr*)&server, sizeof(sockaddr_in)) == SOCKET_ERROR)
        {
            cout << "Error!";
            return 0;
        }


        char ans_mes[BUFLEN] = {};
        int lenth2 = sizeof(sockaddr_in);
        int ans_mes_length;
        if (ans_mes_length = recvfrom(socket2, ans_mes, BUFLEN, 0, (sockaddr*)&server, &lenth2) == SOCKET_ERROR)
        {
            cout << "Error!";
            return 0;
        }

        cout << inet_ntoa(server.sin_addr) << ":" << ntohs(server.sin_port) << " - " << ans_mes << endl;
    }

    closesocket(socket2);
    WSACleanup();
}
