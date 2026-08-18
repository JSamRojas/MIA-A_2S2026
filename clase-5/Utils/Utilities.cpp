#include "Utilities.h"

#include <algorithm>
#include <cctype>

namespace Utilities
{

    bool ConvertBytes(int size, const std::string &unit, long long &outBytes, std::string &errMsg)
    {
        std::string u = unit;
        std::transform(u.begin(), u.end(), u.begin(),
                       [](unsigned char c)
                       { return std::toupper(c); });

        if (u == "K")
        {
            outBytes = static_cast<long long>(size) * 1024LL;
        }
        else if (u == "M" || u.empty())
        {
            outBytes = static_cast<long long>(size) * 1024LL * 1024LL;
        }
        else
        {
            errMsg = "unidad invalida: " + unit;
            return false;
        }

        return true;
    }

}