#include "Client.hpp"
#include <iostream>
#include <unistd.h>
#include <string.h>

std::mutex Client::mutex;

Client::Client(const std::string& ip, unsigned short port) {
	if ((this->sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
		perror("a");
		exit(1);
	}

	this->serverAddr.sin_family = AF_INET;
	this->serverAddr.sin_port = htons(port);
	this->serverAddr.sin_addr.s_addr = inet_addr(ip.c_str());

	bzero(&(this->serverAddr.sin_zero), 8);
	
	std::cout << "[INFO] Connecting to server..." << std::endl;
}

Client::~Client() {
    close(this->sock);
}

int Client::sendMessage(const std::string& message){
    return sendto(this->sock, message.c_str(), message.length(), 0, (const sockaddr*) &this->serverAddr, sizeof(this->serverAddr));
}

int Client::receiveMessage() {
	char buffer[BUFLEN];
	int recvLen, len = sizeof(sockaddr_in);

	std::lock_guard<std::mutex> clientGuard(mutex);

	recvLen = recvfrom(this->sock, buffer, BUFLEN, 0, (sockaddr*) &this->serverAddr, (socklen_t*) &len);
    this->message = buffer;

	return recvLen;
}

int Client::nonBlockingReceiveMessage() {
	char buffer[BUFLEN];
	int recvLen, len = sizeof(sockaddr_in);

	std::lock_guard<std::mutex> clientGuard(mutex);

	recvLen = recvfrom(this->sock, buffer, BUFLEN, MSG_DONTWAIT, (sockaddr*) &this->serverAddr, (socklen_t*) &len);
    this->message = buffer;

	return recvLen;
}

std::string Client::getMessage() const {
	return this->message;
}