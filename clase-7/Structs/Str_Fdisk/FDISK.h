#pragma once

#include <string>

namespace Structs
{

    struct FDISK
    {
        int Size = 0;
        std::string Unit;
        std::string Path;
        std::string Type;
        std::string Fit;
        std::string Name;
    };

    // Convierte el size a bytes y crea la particion (primaria, extendida
    // o logica segun fdisk.Type) dentro del disco en fdisk.Path
    bool Struct_FDISK(const FDISK &fdisk, std::string &errMsg);

}