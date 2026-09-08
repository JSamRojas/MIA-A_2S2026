#pragma once

#include <string>

#include "../../Structs/Str_Mbr/MBR.h"

namespace Reports
{

    bool ReporteMBR(const Structs::MBR &mbr, const std::string &path, const std::string &diskPath, std::string &errMsg);

}