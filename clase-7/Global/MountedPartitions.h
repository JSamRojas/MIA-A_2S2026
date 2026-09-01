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

    // Busca la particion montada con ese id: lee el MBR de su disco y
    // devuelve una copia de la particion encontrada, junto con el path
    // del disco fisico
    bool GetMountedPartition(const std::string &id,
                             Structs::PARTITION &partOut,
                             std::string &pathOut,
                             std::string &errMsg);

}