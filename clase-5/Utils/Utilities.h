#pragma once

#include <string>

namespace Utilities
{

    //   K -> size * 1024
    //   M -> size * 1024 * 1024
    // Devuelve true y llena outBytes si la unidad es valida; si no,
    // devuelve false y llena errMsg
    bool ConvertBytes(int size, const std::string &unit, long long &outBytes, std::string &errMsg);

}