#include "server.hpp"
#include "client.hpp"

void Server::run() {
    while (1) {
        int pollCount = poll(&pollfds[0], pollfds.size(), -1);
        if (pollCount < 0) {
            std::cout << "Error during poll\n";
            exit(1);
        }

        for (size_t i = 0; i < pollfds.size(); ++i) {
            if (pollfds[i].revents & POLLIN) {
                if (pollfds[i].fd == serverSocket) {
                    handleNewConnection();
                } else {
                    handleClientMessage(pollfds[i].fd);
                }
            }
        }
    }
}


void Server::handleNewConnection() {
    sockaddr_in clientAddr;
    socklen_t clientAddrSize = sizeof(clientAddr);
    int clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &clientAddrSize);

    if (clientSocket < 0) {
        std::cout << "Error accepting new connection" << std::endl;
        return;
    }

    fcntl(clientSocket, F_SETFL, O_NONBLOCK);  // Non-blocking client socket what for?

    pollfd clientPoll;
    clientPoll.fd = clientSocket;
    clientPoll.events = POLLIN;
    pollfds.push_back(clientPoll);

    clients[clientSocket] = new Client(clientSocket);
    std::cout << "New client connected: " << clientSocket << "\n";
}


void Server::closeClientConnection(int clientSocket) {
    std::cout << "Client disconnected: " << clientSocket << "\n";
    close(clientSocket);

    // Remove clients
    for (size_t i = 0; i < pollfds.size(); ++i)
    {
        if (pollfds[i].fd == clientSocket) {
            pollfds.erase(pollfds.begin() + i);
            break;
        }
    }

    delete clients[clientSocket];
    clients.erase(clientSocket);
}

