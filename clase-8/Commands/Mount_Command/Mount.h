#pragma once

#include <string>
#include <vector>

#include "../CommandResult.h"

namespace Commands
{

    struct MOUNT
    {
        std::string Path;
        std::string Name;
        std::string List;
    };

    CommandResult Mount_Command(const std::vector<std::string> &tokens);

}