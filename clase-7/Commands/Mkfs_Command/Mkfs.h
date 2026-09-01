#pragma once

#include <string>
#include <vector>

#include "../CommandResult.h"

namespace Commands
{

    struct MKFS
    {
        std::string Id;
        std::string Type;
    };

    CommandResult Mkfs_Command(const std::vector<std::string> &tokens);

}