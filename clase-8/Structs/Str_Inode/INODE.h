#pragma once

#include <cstdint>
#include <string>

namespace Structs
{

    // Tamaño empacado: 4*3 (uid,gid,size) + 4*3 (atime,ctime,mtime) +
    // 4*15 (I_block) + 1 (I_type) + 3 (I_perm) = 88 bytes
#pragma pack(push, 1)
    struct INODE
    {
        int32_t I_uid;       // dueño del archivo/carpeta
        int32_t I_gid;       // grupo dueño
        int32_t I_size;      // tamaño en bytes del contenido (solo archivos)
        float I_atime;       // fecha de ultimo acceso
        float I_ctime;       // fecha de creacion
        float I_mtime;       // fecha de ultima modificacion
        int32_t I_block[15]; // punteros a bloques de datos (-1 = no usado)
        char I_type[1];      // 0 = carpeta, 1 = archivo
        char I_perm[3];      // permisos estilo UGO, ej. 777

        bool Serialize(const std::string &path, long long offset, std::string &errMsg);

        bool Deserialize(const std::string &path, long long offset, std::string &errMsg);

        void Print() const;
    };
#pragma pack(pop)

}