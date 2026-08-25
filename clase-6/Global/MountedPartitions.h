#pragma once

#include <string>
#include <unordered_map>

#include "../Structs/Str_Mbr/MBR.h"

namespace GlobalNS
{

    // key = id de la particion montada, value = path del disco fisico
    extern std::unordered_map<std::string, std::string> MountedPartitions;

    bool GetEssentialRep(const std::string &id,
                         Structs::MBR &mbrOut,
                         std::string &diskPathOut,
                         std::string &errMsg);

}