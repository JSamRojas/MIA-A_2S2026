#pragma once

#include <cstdint>
#include <string>

namespace Structs
{

    // Equivalente a type FOLDERCONTENT struct { B_name [12]byte; B_inodo int32 }
    // Una entrada dentro de una carpeta: nombre + numero de inodo al que
    // apunta. Tamaño: 12 + 4 = 16 bytes
#pragma pack(push, 1)
    struct FOLDERCONTENT
    {
        char B_name[12];
        int32_t B_inodo;
    };
#pragma pack(pop)

    // Equivalente a type FOLDERBLOCK struct { B_content [4]FOLDERCONTENT }
    // Un bloque de carpeta: hasta 4 entradas (nombre + inodo)
    // Tamaño: 4 * 16 = 64 bytes (igual que FILEBLOCK)
#pragma pack(push, 1)
    struct FOLDERBLOCK
    {
        FOLDERCONTENT B_content[4];

        bool Serialize(const std::string &path, long long offset, std::string &errMsg);

        bool Deserialize(const std::string &path, long long offset, std::string &errMsg);

        void Print() const;
    };
#pragma pack(pop)

}