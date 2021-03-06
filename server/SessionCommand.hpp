#pragma once

#include "Command.hpp"
#include "Sessions.hpp"
#include <string>

class SessionCommand : public Command {
public:
    SessionCommand(Sessions&);
    SessionCommand(Connectable*, Sessions&, const std::string&);
    void execute() override;
    std::string name() const override;
};