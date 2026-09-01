#include "Mkfs.h"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstring>
#include <ctime>
#include <regex>
#include <sstream>

#include "../../Global/MountedPartitions.h"
#include "../../Structs/Str_Fileblock/FILEBLOCK.h"
#include "../../Structs/Str_Inode/INODE.h"
#include "../../Structs/Str_Partition/PARTITION.h"
#include "../../Structs/Str_Superblock/SUPERBLOCK.h"

namespace Commands
{

    // ---- Helpers locales a este archivo --------------------------------

    static std::string toLowerStr(std::string s)
    {
        std::transform(s.begin(), s.end(), s.begin(),
                       [](unsigned char c)
                       { return std::tolower(c); });
        return s;
    }

    static std::string joinTokens(const std::vector<std::string> &tokens)
    {
        std::string result;
        for (size_t i = 0; i < tokens.size(); ++i)
        {
            if (i > 0)
                result += " ";
            result += tokens[i];
        }
        return result;
    }

    // ------------------------------------------------------------------
    //
    // Calcula cuantas estructuras "n" (inodos) caben en el espacio de la
    // particion, con la formula:
    //
    //   n = (Size_particion - Size_Superbloque) / (4 + Size_Inodo + 3*Size_Bloque)
    //
    // El 4 representa 1 byte de bitmap por inodo + 3 bytes de bitmap
    // por sus 3 bloques asociados, el "3 * Size_Bloque" representa que
    // cada inodo puede llegar a usar hasta 3 bloques de datos
    // ------------------------------------------------------------------
    static int32_t calculate_N(const Structs::PARTITION &part)
    {
        long long numerador = static_cast<long long>(part.Partition_size) -
                              static_cast<long long>(sizeof(Structs::SUPERBLOCK));

        long long denominador = 4 +
                                static_cast<long long>(sizeof(Structs::INODE)) +
                                (3 * static_cast<long long>(sizeof(Structs::FILEBLOCK)));

        double n = std::floor(static_cast<double>(numerador) / static_cast<double>(denominador));

        return static_cast<int32_t>(n);
    }

    // ------------------------------------------------------------------
    //
    // Arma el superbloque inicial calculando en que byte del disco
    // empieza cada zona:
    //
    //   [ SUPERBLOCK ][ bitmap inodos (n) ][ bitmap bloques (3n) ][ inodos (n) ][ bloques (3n) ]
    // ------------------------------------------------------------------
    static Structs::SUPERBLOCK Create_SuperBlock(const Structs::PARTITION &part, int32_t n_Value)
    {
        int32_t bm_inode_start = part.Partition_start + static_cast<int32_t>(sizeof(Structs::SUPERBLOCK));

        // n_Value = cantidad de inodos representables en el bitmap de inodos
        int32_t bm_block_start = bm_inode_start + n_Value;

        // hay 3 bloques por cada n (3 tipos de bloque distintos)
        int32_t inode_start = bm_block_start + (3 * n_Value);

        // n_Value tambien indica la cantidad de estructuras INODE
        int32_t block_start = inode_start + (static_cast<int32_t>(sizeof(Structs::INODE)) * n_Value);

        Structs::SUPERBLOCK sb{};
        std::memset(&sb, 0, sizeof(sb));

        sb.Sb_filesystem_type = 2; // EXT2
        sb.Sb_inodes_count = 0;
        sb.Sb_blocks_count = 0;
        sb.Sb_free_inodes_count = n_Value;
        sb.Sb_free_blocks_count = n_Value * 3;
        sb.Sb_mtime = static_cast<float>(std::time(nullptr));
        sb.Sb_umtime = static_cast<float>(std::time(nullptr));
        sb.Sb_mnt_count = 1;
        sb.Sb_magic = 0xEF53;
        sb.Sb_inode_size = static_cast<int32_t>(sizeof(Structs::INODE));
        sb.Sb_block_size = static_cast<int32_t>(sizeof(Structs::FILEBLOCK));
        sb.Sb_first_ino = inode_start;
        sb.Sb_first_blo = block_start;
        sb.Sb_bm_inode_start = bm_inode_start;
        sb.Sb_bm_block_start = bm_block_start;
        sb.Sb_inode_start = inode_start;
        sb.Sb_block_start = block_start;

        return sb;
    }

    // ------------------------------------------------------------------
    //
    // Orquesta todo el formateo: busca la particion montada, calcula el
    // superbloque, crea los bitmaps, crea la carpeta raiz + users.txt, y
    // finalmente guarda el superbloque en el disco
    // ------------------------------------------------------------------
    static bool Create_MKFS(const MKFS &mkfs, std::string &errMsg)
    {
        // 1) Se obtiene la particion fisica y el path del disco a partir
        //    del id (buscando en la tabla de particiones montadas)
        Structs::PARTITION mounted_Part{};
        std::string part_Path;
        if (!GlobalNS::GetMountedPartition(mkfs.Id, mounted_Part, part_Path, errMsg))
        {
            return false;
        }

        // 2) Se calcula cuantos inodos/bloques caben en el espacio de la
        //    particion
        int32_t n_Value = calculate_N(mounted_Part);

        // 3) Se arma el superbloque inicial (todavia no se ha guardado en
        //    el disco, solo existe en memoria)
        Structs::SUPERBLOCK super_Block = Create_SuperBlock(mounted_Part, n_Value);

        // 4) Se inicializan los bitmaps de inodos y bloques
        if (!super_Block.Create_Bit_Maps(part_Path, errMsg))
        {
            return false;
        }

        // 5) Se crea la carpeta raiz "/" y el archivo users.txt dentro de
        //    ella
        if (!super_Block.Create_UsersTXT(part_Path, errMsg))
        {
            return false;
        }

        // 6) Finalmente se guarda el superbloque (ya actualizado con los
        //    contadores reales) en su posicion dentro del disco
        if (!super_Block.Serialize(part_Path, static_cast<long long>(mounted_Part.Partition_start), errMsg))
        {
            return false;
        }

        return true;
    }

    // ------------------------------------------------------------------
    // Punto de entrada del comando MKFS: analiza -id y -type, valida, y
    // delega el formateo real a Create_MKFS
    // ------------------------------------------------------------------
    CommandResult Mkfs_Command(const std::vector<std::string> &tokens)
    {

        MKFS mkfs{};

        std::string atributos = joinTokens(tokens);

        static const std::regex lexic(
            R"(-id=[^\s]+|-type=[^\s]+)",
            std::regex::icase);

        auto begin = std::sregex_iterator(atributos.begin(), atributos.end(), lexic);
        auto end = std::sregex_iterator();

        std::vector<std::string> found;
        for (auto it = begin; it != end; ++it)
        {
            found.push_back(it->str());
        }

        for (const auto &fun : found)
        {
            size_t eqPos = fun.find('=');
            if (eqPos == std::string::npos)
            {
                return {false, "ERROR: formato de parametros invalido: " + fun};
            }

            std::string key = toLowerStr(fun.substr(0, eqPos));
            std::string value = fun.substr(eqPos + 1);

            if (value.size() >= 2 && value.front() == '"' && value.back() == '"')
            {
                value = value.substr(1, value.size() - 2);
            }

            if (key == "-id")
            {
                if (value.empty())
                {
                    return {false, "ERROR COMANDO MKFS: parametro id vacio"};
                }
                mkfs.Id = value;
            }
            else if (key == "-type")
            {
                std::string v = toLowerStr(value);
                if (v != "full")
                {
                    return {false, "ERROR COMANDO MKFS: parametro type desconocido"};
                }
                mkfs.Type = v;
            }
            else
            {
                return {false, "ERROR COMANDO MKFS: parametro desconocido: " + key};
            }
        }

        if (mkfs.Id.empty())
        {
            return {false, "ERROR COMANDO MKFS: el parametro id es obligatorio"};
        }

        if (mkfs.Type.empty())
        {
            mkfs.Type = "full";
        }

        std::string errMsg;
        if (!Create_MKFS(mkfs, errMsg))
        {
            return {false, errMsg};
        }

        return {true, "COMANDO MKFS: particion formateada con exito"};
    }

}