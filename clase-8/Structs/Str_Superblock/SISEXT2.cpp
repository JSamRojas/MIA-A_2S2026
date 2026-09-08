#include "SUPERBLOCK.h"

#include "../Str_Fileblock/FILEBLOCK.h"
#include "../Str_Folderblock/FOLDERBLOCK.h"
#include "../Str_Inode/INODE.h"

#include <algorithm>
#include <cstring>
#include <ctime>

namespace Structs
{

    // ------------------------------------------------------------------
    // Crea la estructura minima de un sistema de archivos EXT2 recien
    // formateado:
    //
    //   Inodo 0 -> carpeta raiz "/"   (contiene: ".", "..", y una entrada
    //                                  vacia que luego se llena con
    //                                  "users.txt")
    //   Inodo 1 -> archivo "users.txt" con el usuario root por defecto
    //
    // Se llama una sola vez, justo despues de crear los bitmaps
    // ------------------------------------------------------------------
    bool SUPERBLOCK::Create_UsersTXT(const std::string &path, std::string &errMsg)
    {
        // ================================================================
        // PASO 1: Crear el INODO de la carpeta raiz (inodo numero 0)
        // ================================================================
        INODE root_Inode{};
        std::memset(&root_Inode, 0, sizeof(root_Inode));

        root_Inode.I_uid = 1;
        root_Inode.I_gid = 1;
        root_Inode.I_size = 0;
        root_Inode.I_atime = static_cast<float>(std::time(nullptr));
        root_Inode.I_ctime = static_cast<float>(std::time(nullptr));
        root_Inode.I_mtime = static_cast<float>(std::time(nullptr));

        // I_block[0] apunta al bloque que va a crear este inodo (el
        // folderblock de la raiz); el resto de punteros quedan en -1
        // (sin usar)
        root_Inode.I_block[0] = Sb_blocks_count;
        for (int i = 1; i < 15; ++i)
            root_Inode.I_block[i] = -1;

        root_Inode.I_type[0] = '0'; // '0' = carpeta
        root_Inode.I_perm[0] = '7';
        root_Inode.I_perm[1] = '7';
        root_Inode.I_perm[2] = '7';

        // Se escribe el inodo raiz en el disco, en la siguiente posicion
        // libre de la tabla de inodos
        if (!root_Inode.Serialize(path, Sb_first_ino, errMsg))
            return false;

        // Se marca esa posicion como "usada" en el bitmap de inodos.
        if (!Update_Inode_Bitmap(path, errMsg))
            return false;

        // Se actualizan los contadores del superbloque: un inodo mas
        // usado, uno menos libre, y el puntero al siguiente inodo libre
        // avanza Sb_inode_size bytes.
        Sb_inodes_count++;
        Sb_free_inodes_count--;
        Sb_first_ino += Sb_inode_size;

        // ================================================================
        // PASO 2: Crear el BLOQUE de la carpeta raiz (su contenido)
        // ================================================================
        // Toda carpeta EXT2 arranca con 4 entradas: "." (ella misma),
        // ".." (su padre, que para la raiz tambien es ella misma), y dos
        // entradas vacias ("-") disponibles para archivos/carpetas
        // futuras. Aqui la posicion 2 se reserva para "users.txt"
        FOLDERBLOCK root_Block{};
        std::memset(&root_Block, 0, sizeof(root_Block));

        // Entrada 0: "."
        root_Block.B_content[0].B_name[0] = '.';
        root_Block.B_content[0].B_inodo = 0;

        // Entrada 1: ".."
        root_Block.B_content[1].B_name[0] = '.';
        root_Block.B_content[1].B_name[1] = '.';
        root_Block.B_content[1].B_inodo = 0;

        // Entrada 2: vacia (se llenara mas abajo con "users.txt")
        root_Block.B_content[2].B_name[0] = '-';
        root_Block.B_content[2].B_inodo = -1;

        // Entrada 3: vacia
        root_Block.B_content[3].B_name[0] = '-';
        root_Block.B_content[3].B_inodo = -1;

        // Se marca esa posicion como usada en el bitmap de bloques
        if (!Update_Block_Bitmap(path, errMsg))
            return false;

        // Se escribe el folderblock de la raiz en el disco
        if (!root_Block.Serialize(path, Sb_first_blo, errMsg))
            return false;

        // Se actualizan los contadores del superbloque para el bloque
        // recien creado
        Sb_blocks_count++;
        Sb_free_blocks_count--;
        Sb_first_blo += Sb_block_size;

        // ================================================================
        // PASO 3: Preparar el contenido del archivo users.txt
        // ================================================================
        std::string usersTXT = "1,G,root\n1,U,root,root,123\n";

        // Se relee el inodo raiz desde el disco (ya en su posicion final,
        // Sb_inode_start + 0, la primera entrada de la tabla de inodos)
        // solo para actualizar su fecha de acceso
        if (!root_Inode.Deserialize(path, Sb_inode_start + 0, errMsg))
            return false;

        root_Inode.I_atime = static_cast<float>(std::time(nullptr));

        if (!root_Inode.Serialize(path, Sb_inode_start + 0, errMsg))
            return false;

        // Se relee el folderblock de la raiz (Sb_block_start + 0, la
        // primera entrada de la zona de bloques) para poder editar su
        // entrada vacia y apuntarla al nuevo archivo
        if (!root_Block.Deserialize(path, Sb_block_start + 0, errMsg))
            return false;

        // Se llena la entrada 2 (la que estaba vacia con "-") con el
        // nombre "users.txt" y el numero de inodo que va a tener ese
        // archivo (Sb_inodes_count, es decir, el proximo inodo a crear:
        // el numero 1)
        std::memset(root_Block.B_content[2].B_name, 0, sizeof(root_Block.B_content[2].B_name));
        {
            const std::string userFileName = "users.txt";
            std::size_t n = std::min(userFileName.size(), sizeof(root_Block.B_content[2].B_name));
            std::memcpy(root_Block.B_content[2].B_name, userFileName.data(), n);
        }
        root_Block.B_content[2].B_inodo = Sb_inodes_count;

        // Se vuelve a guardar el folderblock de la raiz, ya actualizado
        if (!root_Block.Serialize(path, Sb_block_start + 0, errMsg))
            return false;

        // ================================================================
        // PASO 4: Crear el INODO de users.txt (inodo numero 1)
        // ================================================================
        INODE users_Inode{};
        std::memset(&users_Inode, 0, sizeof(users_Inode));

        users_Inode.I_uid = 1;
        users_Inode.I_gid = 1;
        users_Inode.I_size = static_cast<int32_t>(usersTXT.size());
        users_Inode.I_atime = static_cast<float>(std::time(nullptr));
        users_Inode.I_ctime = static_cast<float>(std::time(nullptr));
        users_Inode.I_mtime = static_cast<float>(std::time(nullptr));

        users_Inode.I_block[0] = Sb_blocks_count;
        for (int i = 1; i < 15; ++i)
            users_Inode.I_block[i] = -1;

        users_Inode.I_type[0] = '1'; // 1 = archivo
        users_Inode.I_perm[0] = '7';
        users_Inode.I_perm[1] = '7';
        users_Inode.I_perm[2] = '7';

        if (!Update_Inode_Bitmap(path, errMsg))
            return false;

        if (!users_Inode.Serialize(path, Sb_first_ino, errMsg))
            return false;

        Sb_inodes_count++;
        Sb_free_inodes_count--;
        Sb_first_ino += Sb_inode_size;

        // ================================================================
        // PASO 5: Crear el BLOQUE de users.txt (su contenido real)
        // ================================================================
        FILEBLOCK users_Block{};
        std::memset(&users_Block, 0, sizeof(users_Block));

        std::size_t n = std::min(usersTXT.size(), sizeof(users_Block.B_content));
        std::memcpy(users_Block.B_content, usersTXT.data(), n);

        if (!users_Block.Serialize(path, Sb_first_blo, errMsg))
            return false;

        if (!Update_Block_Bitmap(path, errMsg))
            return false;

        Sb_blocks_count++;
        Sb_free_blocks_count--;
        Sb_first_blo += Sb_block_size;

        errMsg.clear();
        return true;
    }

}