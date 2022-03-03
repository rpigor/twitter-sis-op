#include "TweetCommand.hpp"
#include <iostream>

TweetCommand::TweetCommand(Sessions& sessions, std::vector<Tweet>& tweets)
    : Command(sessions), tweets(tweets) {

}

TweetCommand::TweetCommand(Network network, Sessions& sessions, std::vector<Tweet>& tweets, const std::string& payload)
    : Command(network, sessions, payload), tweets(tweets) {

}

void TweetCommand::execute() const {
    std::string username = this->payload.substr(0, this->payload.find(","));
    std::string auxSession = this->payload.substr(this->payload.find(",") + 1);
    std::string session = auxSession.substr(0, auxSession.find(","));
    std::string auxMessage = auxSession.substr(auxSession.find(",") + 1);
    std::string message = auxMessage.substr(0, auxMessage.find(","));
    
    if (!this->sessions.accountExists(username)) {
        if (sendto(this->network.socket, std::string("perfil nao existe\n").c_str(), std::string("perfil nao existe\n").length(), 0, (struct sockaddr*) &this->network.clientAddr, sizeof(this->network.clientAddr)) == -1) {
            perror("sendto()");
            exit(1);
        }
        std::cout << "Account @" << username << " does not exist. Aborting operation.\n";
        return;
    }

    if (!this->sessions.hasSession(username, std::stoul(session))) {
        if (sendto(this->network.socket, std::string("sessao invalida\n").c_str(), std::string("sessao invalida\n").length(), 0, (struct sockaddr*) &this->network.clientAddr, sizeof(this->network.clientAddr)) == -1) {
            perror("sendto()");
            exit(1);
        }
        std::cout << "Invalid session [" << session << "] for account @" << username << ". Aborting operation.\n";
        return;
    }

    if (message.length() > 140) {
        if (sendto(this->network.socket, std::string("mensagem excede 140 caracteres\n").c_str(), std::string("mensagem excede 140 caracteres\n").length(), 0, (struct sockaddr*) &this->network.clientAddr, sizeof(this->network.clientAddr)) == -1) {
            perror("sendto()");
            exit(1);
        }
        std::cout << "Tweet message exceeds 140 characters. Abording operation.\n";
        return;
    }

    std::vector<Account>::iterator it = this->sessions.getAccounts().begin();
    while (it != this->sessions.getAccounts().end()) {
        if (it->getUsername() == username)
            break;
        ++it;
    }

    this->tweets.push_back(Tweet(*it, message));

    std::cout << "Account @" << username << " just tweeted: \"" << message << "\"." << std::endl;

    std::cout << "@" << username << "'s tweets: ";
    for (const Tweet& tt : this->tweets) {
        if (tt.getAccount().getUsername() == username) {
            std::cout << "\"" << tt.getMessage() << "\" ";
        }
    }
    std::cout << std::endl;

    if (sendto(this->network.socket, std::string("tweet," + username + "," + session + "," + message + "\n").c_str(), std::string("seguir," + username + "," + session + "," + message + "\n").length(), 0, (struct sockaddr*) &this->network.clientAddr, sizeof(this->network.clientAddr)) == -1) {
        perror("sendto()");
        exit(1);
    }
}

std::string TweetCommand::name() const {
    return "Tweeta mensagem";
}