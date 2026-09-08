#pragma once

#include <string>

namespace Structs
{

    // Equivalente a type FILEBLOCK struct { B_content [64]byte }
    // Un bloque de archivo: 64 bytes crudos de contenido
#pragma pack(push, 1)
    struct FILEBLOCK
    {
        char B_content[64];

        bool Serialize(const std::string &path, long long offset, std::string &errMsg);

        bool Deserialize(const std::string &path, long long offset, std::string &errMsg);

        void Print() const;
    };
#pragma pack(pop)

}