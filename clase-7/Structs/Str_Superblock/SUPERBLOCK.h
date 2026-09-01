#pragma once

#include <cstdint>
#include <string>

namespace Structs
{

    // #pragma pack(1) evita que el compilador meta relleno entre
    // campos, para que el layout en memoria sea identico, byte a byte
#pragma pack(push, 1)
    struct SUPERBLOCK
    {
        int32_t Sb_filesystem_type;   // tipo de sistema de archivos (2 = EXT2)
        int32_t Sb_inodes_count;      // cantidad de inodos ya usados
        int32_t Sb_blocks_count;      // cantidad de bloques ya usados
        int32_t Sb_free_blocks_count; // cantidad de bloques libres
        int32_t Sb_free_inodes_count; // cantidad de inodos libres
        float Sb_mtime;               // fecha del ultimo montaje
        float Sb_umtime;              // fecha del ultimo desmontaje
        int32_t Sb_mnt_count;         // cantidad de veces que se ha montado
        int32_t Sb_magic;             // numero magico (0xEF53, identifica el FS como EXT2)
        int32_t Sb_inode_size;        // tamaño en bytes de un INODE (sizeof(INODE))
        int32_t Sb_block_size;        // tamaño en bytes de un bloque (sizeof(FILEBLOCK))
        int32_t Sb_first_ino;         // byte del disco donde va el PROXIMO inodo libre
        int32_t Sb_first_blo;         // byte del disco donde va el PROXIMO bloque libre
        int32_t Sb_bm_inode_start;    // byte del disco donde empieza el bitmap de inodos
        int32_t Sb_bm_block_start;    // byte del disco donde empieza el bitmap de bloques
        int32_t Sb_inode_start;       // byte del disco donde empieza la tabla de inodos
        int32_t Sb_block_start;       // byte del disco donde empieza la zona de bloques

        // ---- Str_Superblock/SUPERBLOCK.cpp ----

        // Escribe esta estructura (tal cual, en binario) en el archivo del
        // disco, en la posicion "offset"
        bool Serialize(const std::string &path, long long offset, std::string &errMsg);

        // Lee sizeof(SUPERBLOCK) bytes desde "offset" y los vuelca sobre
        // esta misma estructura
        bool Deserialize(const std::string &path, long long offset, std::string &errMsg);

        // ---- Str_Superblock/Bitmaps.cpp ----

        // Inicializa los bitmaps de inodos y bloques llenandolos de '0'
        // (todo libre), justo despues de crear el sistema de archivos
        bool Create_Bit_Maps(const std::string &path, std::string &errMsg);

        // Marca con 1 la siguiente posicion libre del bitmap de inodos
        // (la posicion Sb_inodes_count)
        bool Update_Inode_Bitmap(const std::string &path, std::string &errMsg);

        // Marca con 1 la siguiente posicion libre del bitmap de bloques
        // (la posicion Sb_blocks_count)
        bool Update_Block_Bitmap(const std::string &path, std::string &errMsg);

        // ---- Str_Superblock/SisExt2.cpp ----

        // Crea el inodo y bloque de la carpeta raiz ("/"), y dentro de ella
        // el archivo users.txt con el usuario root por defecto
        bool Create_UsersTXT(const std::string &path, std::string &errMsg);
    };
#pragma pack(pop)

}