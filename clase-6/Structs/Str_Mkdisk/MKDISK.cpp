#include "MKDISK.h"
#include "../Str_Mbr/MBR.h"
#include "../../Utils/Utilities.h"

#include <filesystem>
#include <fstream>
#include <vector>

namespace Structs
{

    bool Struct_MKDISK(const MKDISK &disk, std::string &errMsg)
    {
        long long sizeB = 0;

        if (!Utilities::ConvertBytes(disk.Size, disk.Unit, sizeB, errMsg))
        {
            errMsg = "Error: No se pudo convertir el size del disco (" + errMsg + ")";
            return false;
        }

        if (!MakeDisk(disk, sizeB, errMsg))
            return false;

        if (!CreateMBR(disk, sizeB, errMsg))
            return false;

        return true;
    }

    bool MakeDisk(const MKDISK &disk, long long sizeB, std::string &errMsg)
    {
        namespace fs = std::filesystem;

        fs::path filePath(disk.Path);
        fs::path dir = filePath.parent_path();

        if (!dir.empty())
        {
            std::error_code ec;
            fs::create_directories(dir, ec);
            if (ec)
            {
                errMsg = "Error: No se pudo crear la carpeta";
                return false;
            }
        }

        std::ofstream file(disk.Path, std::ios::binary | std::ios::out | std::ios::trunc);
        if (!file.is_open())
        {
            errMsg = "Error: No se pudo crear el archivo";
            return false;
        }

        const std::size_t bufSize = 1024 * 1024; // 1 MB
        std::vector<char> buffer(bufSize, 0);

        long long remaining = sizeB;
        while (remaining > 0)
        {
            std::size_t wSize = (remaining < static_cast<long long>(bufSize))
                                    ? static_cast<std::size_t>(remaining)
                                    : bufSize;

            file.write(buffer.data(), static_cast<std::streamsize>(wSize));
            if (!file)
            {
                errMsg = "Error: Error al escribir el disco";
                return false;
            }

            remaining -= static_cast<long long>(wSize);
        }

        return true;
    }

}