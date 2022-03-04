#include "FollowCommand.hpp"
#include <iostream>
#include <algorithm>

FollowCommand::FollowCommand(Sessions& sessions)
    : Command(sessions) {

}

FollowCommand::FollowCommand(Connection connection, Sessions& sessions, const std::string& payload)
    : Command(connection, sessions, payload) {

}

void FollowCommand::execute() {
    std::string username = this->payload.substr(0, this->payload.find(","));
    std::string auxSession = this->payload.substr(this->payload.find(",") + 1);
    std::string session = auxSession.substr(0, auxSession.find(","));
    std::string auxUserToFollow = auxSession.substr(auxSession.find(",") + 1);
    std::string userToFollow = auxUserToFollow.substr(0, auxUserToFollow.find(","));

    if (!this->sessions.accountExists(username)) {
        if (this->connection.sendMessage("perfil nao existe\n") < 0) {
            perror("sendto()");
            exit(1);
        }
        std::cout << "Account @" << username << " does not exist. Aborting operation.\n";
        return;
    }

    if (!this->sessions.accountExists(userToFollow)) {
        if (this->connection.sendMessage("perfil para seguir nao existe\n") < 0) {
            perror("sendto()");
            exit(1);
        }
        std::cout << "Account @" << userToFollow << " does not exist to follow. Aborting operation.\n";
        return;
    }

    if (!this->sessions.hasSession(username, std::stoul(session))) {
        if (this->connection.sendMessage("sessao invalida\n") < 0) {
            perror("sendto()");
            exit(1);
        }
        std::cout << "Invalid session [" << session << "] for account @" << username << ". Aborting operation.\n";
        return;
    }

    if (userToFollow == username) {
        if (this->connection.sendMessage("perfil nao pode seguir a si mesmo\n") < 0) {
            perror("sendto()");
            exit(1);
        }
        std::cout << "Account @" << userToFollow << " cannot follow itself. Aborting operation.\n";
        return;
    }

    std::vector<Account>::iterator it = this->sessions.getAccounts().begin();
    while (it != this->sessions.getAccounts().end()) {
        if (it->getUsername() == username)
            break;
        ++it;
    }

    it->follow(userToFollow);

    std::cout << "Account @" << username << " just followed account @" << userToFollow << ".\n";

    std::cout << "@" << username <<  " is following: ";
    for (const Account& acc : it->getFollowing()) {
        std::cout << "@" << acc.getUsername() << " ";
    }
    std::cout << std::endl;

    if (this->connection.sendMessage("seguir," + username + "," + session + "," + userToFollow + "\n") < 0) {
        perror("sendto()");
        exit(1);
    }
}

std::string FollowCommand::name() const {
    return "Follows user";
}