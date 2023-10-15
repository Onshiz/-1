#include <iostream>
#include <winsock2.h>
#include <clocale>
#include <windows.h>
#include <sstream>
#include <vector>
#include <string>


using namespace std;

#pragma comment(lib,"ws2_32.lib")
#pragma warning(disable:4996) 

#define BUFLEN 512
#define PORT 1747

bool is_error = false;
int rejim = 0;


struct Command {
    string comanda;
    vector<string> parameters;
};

Command ParseCommand(const string& commandStr) {
    Command command;

    int number;

    istringstream get_mes_word(commandStr);
    string get_word;
    get_mes_word >> command.comanda;


    while (get_mes_word >> get_word) {
        if (get_word == ":")
        {
            break;
        }
        command.comanda += " " + get_word;
    }

    if (command.comanda == "clear display") {
        is_error = false;
        rejim = 1;
    }
    if (command.comanda == "draw pixel") {
        rejim = 2;
        is_error = false;
    }
    if (command.comanda == "draw line") {
        rejim = 5;
        is_error = false;
    }
    if (command.comanda == "draw rectangle")
    {
        rejim = 10;
        is_error = false;
    }
    if (command.comanda == "fill rectangle") {
        rejim = 15;
        is_error = false;
    }
    if (command.comanda == "draw ellipse") {
        rejim = 20;
        is_error = false;
    }
    if (command.comanda == "fill ellipse")
    {
        rejim = 25;
        is_error = false;
    }
    if (rejim == 0) {
        is_error = true;
    }


    if (!is_error)
    {
        while (get_mes_word >> get_word) {
            switch (rejim)
            {
            case 1: case 4: case 9: case 14: case 19: case 24: case 29:
                command.parameters.push_back(get_word);
                try {
                    number = stoi(get_word);
                }
                catch (const invalid_argument& e) {
                    is_error = false;
                    return command;
                }
                is_error = true;
                return command;
            case 2: case 3: case 5: case 6: case 7: case 8: case 10: case 11: case 12: case 13:
            case 15: case 16: case 17: case 18: case 20: case 21: case 22: case 23:
            case 25: case 26: case 27: case 28:
                command.parameters.push_back(get_word);
                try {
                    number = stoi(get_word);
                }
                catch (const invalid_argument& e) {
                    is_error = true;
                    return command;
                }
                is_error = false;
                rejim++;
                break;
            }
        }
        is_error = true;
        return command;
    }
    else {
        return command;
    }
}


int main()
{
    setlocale(LC_ALL, "");
    sockaddr_in server, client;


    WSADATA wsdat1;
    if (WSAStartup(MAKEWORD(2, 2), &wsdat1) != 0)
    {
        cout << "Error!";
        return 0;
    }

    SOCKET socket1;
    if ((socket1 = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET)
    {
        cout << "Error!";
        return 0;
    }


    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);


    if (bind(socket1, (sockaddr*)&server, sizeof(server)) == SOCKET_ERROR)
    {
        cout << "Error!";
        return 0;
    }
    while (true)
    {
        fflush(stdout);
        char your_mes[BUFLEN] = {};


        int lengt;
        int lengt2 = sizeof(sockaddr_in);
        if (lengt = recvfrom(socket1, your_mes, BUFLEN, 0, (sockaddr*)&client, &lengt2) == SOCKET_ERROR)
        {
            cout << "Error!";
            return 0;
        }

        cout << inet_ntoa(client.sin_addr) << ":" << ntohs(client.sin_port) << " - " << your_mes << endl;

        istringstream mes_line(your_mes);
        string line_word;

        while (getline(mes_line, line_word, '\n')) {
            Command command = ParseCommand(line_word);

            string firstWord, secondWord;
            istringstream nameStream(command.comanda);
            nameStream >> firstWord >> secondWord;


            if (!is_error) {
                cout << "Основная команда: " << command.comanda << endl;
                cout << "Параметры команды:";
                for (const string& param : command.parameters) {
                    cout << endl << "= " << param;
                }
                cout << endl;
            }
        }
        if (is_error)
        {

            if (sendto(socket1, your_mes, strlen(your_mes), 0, (sockaddr*)&client, sizeof(sockaddr_in)) == SOCKET_ERROR)
            {
                cout << "sendto() получил ошибку. Код ошибки: " << WSAGetLastError();
                return 3;
            }
        }

        if (!is_error)
        {

            if (sendto(socket1, your_mes, strlen(your_mes), 0, (sockaddr*)&client, sizeof(sockaddr_in)) == SOCKET_ERROR)
            {
                cout << "sendto() получил ошибку. Код ошибки: " << WSAGetLastError();
                return 3;
            }
        }


    }

    closesocket(socket1);
    WSACleanup();
}
