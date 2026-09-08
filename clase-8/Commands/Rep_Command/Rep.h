#pragma once

#include <string>
#include <vector>

#include "../CommandResult.h"

namespace Commands
{

    struct REP
    {
        std::string Name;
        std::string Path;
        std::string Id;
        std::string Path_file;
    };

    CommandResult Rep_Command(const std::vector<std::string> &tokens);

}