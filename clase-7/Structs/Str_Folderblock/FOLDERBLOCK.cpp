#include "FOLDERBLOCK.h"

#include <cstdio>
#include <fstream>

namespace Structs
{

    bool FOLDERBLOCK::Serialize(const std::string &path, long long offset, std::string &errMsg)
    {
        std::fstream file(path, std::ios::binary | std::ios::in | std::ios::out);
        if (!file.is_open())
        {
            errMsg = "ERROR: No se pudo abrir el archivo al serializar el folderblock";
            return false;
        }

        file.seekp(offset, std::ios::beg);
        file.write(reinterpret_cast<const char *>(this), sizeof(FOLDERBLOCK));

        if (!file)
        {
            errMsg = "ERROR: No se pudo escribir el folderblock en el archivo";
            return false;
        }

        errMsg.clear();
        return true;
    }

    bool FOLDERBLOCK::Deserialize(const std::string &path, long long offset, std::string &errMsg)
    {
        std::ifstream file(path, std::ios::binary);
        if (!file.is_open())
        {
            errMsg = "ERROR: No se pudo abrir el archivo al deserializar el folderblock";
            return false;
        }

        file.seekg(offset, std::ios::beg);
        file.read(reinterpret_cast<char *>(this), sizeof(FOLDERBLOCK));

        if (!file)
        {
            errMsg = "ERROR: No se pudo leer el folderblock del archivo";
            return false;
        }

        errMsg.clear();
        return true;
    }

    void FOLDERBLOCK::Print() const
    {
        for (int i = 0; i < 4; ++i)
        {
            std::printf("Contenido %d: \n", i + 1);
            std::printf("\tB_name: %.12s\n", B_content[i].B_name);
            std::printf("\tB_inodo: %d\n", B_content[i].B_inodo);
        }
    }

}