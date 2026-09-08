#include "SUPERBLOCK.h"

#include <fstream>

namespace Structs
{

    bool SUPERBLOCK::Serialize(const std::string &path, long long offset, std::string &errMsg)
    {
        // Se abre en modo lectura/escritura SIN truncar: el archivo del
        // disco ya existe (creado por MKDISK y ya lleno con su tamaño
        // real), aqui solo se sobreescriben los bytes desde "offset"
        std::fstream file(path, std::ios::binary | std::ios::in | std::ios::out);
        if (!file.is_open())
        {
            errMsg = "ERROR: No se pudo abrir el archivo al serializar el superbloque";
            return false;
        }

        file.seekp(offset, std::ios::beg);

        // Se escriben los bytes crudos de la estructura
        file.write(reinterpret_cast<const char *>(this), sizeof(SUPERBLOCK));

        if (!file)
        {
            errMsg = "ERROR: No se pudo escribir el superbloque en el archivo";
            return false;
        }

        errMsg.clear();
        return true;
    }

    bool SUPERBLOCK::Deserialize(const std::string &path, long long offset, std::string &errMsg)
    {
        std::ifstream file(path, std::ios::binary);
        if (!file.is_open())
        {
            errMsg = "ERROR: No se pudo abrir el archivo al deserializar el superbloque";
            return false;
        }

        file.seekg(offset, std::ios::beg);

        // Se leen sizeof(SUPERBLOCK) bytes y se vuelcan directamente sobre
        // los campos de esta misma estructura
        file.read(reinterpret_cast<char *>(this), sizeof(SUPERBLOCK));

        if (!file)
        {
            errMsg = "ERROR: No se pudo leer el superbloque del archivo";
            return false;
        }

        errMsg.clear();
        return true;
    }

}