#pragma once

#include "Command.hpp"
#include "Sessions.hpp"
#include <string>

class FollowCommand : public Command {
public:
    FollowCommand(Sessions&);
    FollowCommand(Connection, Sessions&, const std::string&);
    void execute() override;
    std::string name() const override;
};