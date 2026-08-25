#pragma once

#include <string>

namespace Structs
{

    struct MKDISK
    {
        int Size = 0;
        std::string Unit;
        std::string Fit;
        std::string Path;
    };

    // Convierte el size a bytes, crea el archivo fisico y le escribe el MBR.
    // Devuelve true si todo salio bien; si no, errMsg trae el mensaje de error
    bool Struct_MKDISK(const MKDISK &disk, std::string &errMsg);

    bool MakeDisk(const MKDISK &disk, long long sizeB, std::string &errMsg);

}