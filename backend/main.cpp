#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

std::string readFile(const std::string& filename) {

    std::ifstream file(filename);

    if (!file.is_open()) {
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();

    return buffer.str();
}

void sendResponse(
    SOCKET clientSocket,
    const std::string& content,
    const std::string& contentType
) {

    std::string response =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: " + contentType + "\r\n"
        "Content-Length: " +
        std::to_string(content.length()) +
        "\r\n"
        "Connection: close\r\n"
        "\r\n" +
        content;

    send(
        clientSocket,
        response.c_str(),
        response.length(),
        0
    );
}

int main() {

    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);

    SOCKET serverSocket = socket(
        AF_INET,
        SOCK_STREAM,
        0
    );

    sockaddr_in serverAddress;

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(8080);

    bind(
        serverSocket,
        (sockaddr*)&serverAddress,
        sizeof(serverAddress)
    );

    listen(serverSocket, 5);

    std::cout << "Plagiarism Detector Backend Started!" << std::endl;
    std::cout << "Open http://localhost:8080 in your browser." << std::endl;

    while (true) {

        SOCKET clientSocket = accept(
            serverSocket,
            NULL,
            NULL
        );

        char buffer[4096] = {0};

        int bytesReceived = recv(
            clientSocket,
            buffer,
            sizeof(buffer) - 1,
            0
        );

        if (bytesReceived <= 0) {
            closesocket(clientSocket);
            continue;
        }

        buffer[bytesReceived] = '\0';

        std::string request(buffer);

        // Browser requested the main page
        if (request.find("GET / ") != std::string::npos) {

            std::string html =
                readFile("frontend/index.html");

            sendResponse(
                clientSocket,
                html,
                "text/html"
            );
        }

        // Browser requested CSS
        else if (
            request.find("GET /style.css")
            != std::string::npos
        ) {

            std::string css =
                readFile("frontend/style.css");

            sendResponse(
                clientSocket,
                css,
                "text/css"
            );
        }

        // Browser requested JavaScript
        else if (
            request.find("GET /script.js")
            != std::string::npos
        ) {

            std::string js =
                readFile("frontend/script.js");

            sendResponse(
                clientSocket,
                js,
                "application/javascript"
            );
        }

        // Unknown request
        else {

            std::string message =
                "<h1>404 - Not Found</h1>";

            sendResponse(
                clientSocket,
                message,
                "text/html"
            );
        }

        closesocket(clientSocket);
    }

    closesocket(serverSocket);
    WSACleanup();

    return 0;
}