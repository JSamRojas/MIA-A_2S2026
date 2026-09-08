#include "SUPERBLOCK.h"

#include <fstream>
#include <vector>

namespace Structs
{

    // ------------------------------------------------------------------
    // Inicializa los dos bitmaps (inodos y bloques) del sistema de
    // archivos recien creado, llenandolos con el caracter 0 (todo
    // libre). Se llama una sola vez, justo al formatear la particion
    // ------------------------------------------------------------------
    bool SUPERBLOCK::Create_Bit_Maps(const std::string &path, std::string &errMsg)
    {
        std::fstream file(path, std::ios::binary | std::ios::in | std::ios::out);
        if (!file.is_open())
        {
            errMsg = "ERROR: No se pudo abrir el archivo del disco";
            return false;
        }

        // ---- BITMAP DE INODOS ----
        // Se escriben Sb_free_inodes_count bytes con 0, empezando en
        // Sb_bm_inode_start (un byte por cada inodo posible)
        file.seekp(Sb_bm_inode_start, std::ios::beg);

        std::vector<char> inodeBitmap(static_cast<std::size_t>(Sb_free_inodes_count), '0');
        file.write(inodeBitmap.data(), static_cast<std::streamsize>(inodeBitmap.size()));
        if (!file)
        {
            errMsg = "ERROR: No se pudo escribir el bitmap de inodos";
            return false;
        }

        // ---- BITMAP DE BLOQUES ----
        // Se escriben Sb_free_blocks_count bytes con 0, empezando en
        // Sb_bm_block_start (un byte por cada bloque posible; recordar
        // que hay 3 bloques por cada n, por eso este bitmap es 3 veces
        // mas grande que el de inodos)
        file.seekp(Sb_bm_block_start, std::ios::beg);

        std::vector<char> blockBitmap(static_cast<std::size_t>(Sb_free_blocks_count), '0');
        file.write(blockBitmap.data(), static_cast<std::streamsize>(blockBitmap.size()));
        if (!file)
        {
            errMsg = "ERROR: No se pudo escribir el bitmap de bloques";
            return false;
        }

        errMsg.clear();
        return true;
    }

    // ------------------------------------------------------------------
    // Marca como "usado" (escribe 1) la siguiente posicion libre del
    // bitmap de inodos. La posicion es Sb_bm_inode_start + Sb_inodes_count,
    // es decir: el bitmap se va llenando en orden secuencial, un inodo a
    // la vez, segun cuantos se han creado hasta el momento
    // ------------------------------------------------------------------
    bool SUPERBLOCK::Update_Inode_Bitmap(const std::string &path, std::string &errMsg)
    {
        std::fstream file(path, std::ios::binary | std::ios::in | std::ios::out);
        if (!file.is_open())
        {
            errMsg = "ERROR: No se pudo abrir el archivo del disco";
            return false;
        }

        file.seekp(static_cast<long long>(Sb_bm_inode_start) + static_cast<long long>(Sb_inodes_count),
                   std::ios::beg);

        char bit = '1';
        file.write(&bit, 1);
        if (!file)
        {
            errMsg = "ERROR: No se pudo actualizar el bitmap de inodos";
            return false;
        }

        errMsg.clear();
        return true;
    }

    // ------------------------------------------------------------------
    // Igual que Update_Inode_Bitmap, pero para el bitmap de bloques:
    // marca con 1 la posicion Sb_bm_block_start + Sb_blocks_count
    // ------------------------------------------------------------------
    bool SUPERBLOCK::Update_Block_Bitmap(const std::string &path, std::string &errMsg)
    {
        std::fstream file(path, std::ios::binary | std::ios::in | std::ios::out);
        if (!file.is_open())
        {
            errMsg = "ERROR: No se pudo abrir el archivo del disco";
            return false;
        }

        file.seekp(static_cast<long long>(Sb_bm_block_start) + static_cast<long long>(Sb_blocks_count),
                   std::ios::beg);

        char bit = '1';
        file.write(&bit, 1);
        if (!file)
        {
            errMsg = "ERROR: No se pudo actualizar el bitmap de bloques";
            return false;
        }

        errMsg.clear();
        return true;
    }

}