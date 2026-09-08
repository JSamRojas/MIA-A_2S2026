#include "INODE.h"

#include <cstdio>
#include <ctime>
#include <fstream>

namespace Structs
{

    bool INODE::Serialize(const std::string &path, long long offset, std::string &errMsg)
    {
        std::fstream file(path, std::ios::binary | std::ios::in | std::ios::out);
        if (!file.is_open())
        {
            errMsg = "ERROR: No se pudo abrir el archivo al serializar el inodo";
            return false;
        }

        file.seekp(offset, std::ios::beg);
        file.write(reinterpret_cast<const char *>(this), sizeof(INODE));

        if (!file)
        {
            errMsg = "ERROR: No se pudo escribir el inodo en el archivo";
            return false;
        }

        errMsg.clear();
        return true;
    }

    bool INODE::Deserialize(const std::string &path, long long offset, std::string &errMsg)
    {
        std::ifstream file(path, std::ios::binary);
        if (!file.is_open())
        {
            errMsg = "ERROR: No se pudo abrir el archivo al deserializar el inodo";
            return false;
        }

        file.seekg(offset, std::ios::beg);
        file.read(reinterpret_cast<char *>(this), sizeof(INODE));

        if (!file)
        {
            errMsg = "ERROR: No se pudo leer el inodo del archivo";
            return false;
        }

        errMsg.clear();
        return true;
    }

    void INODE::Print() const
    {
        // Las fechas se guardan como timestamps Unix en un float
        auto formatTime = [](float unixTime, char *outBuf, std::size_t outSize)
        {
            std::time_t t = static_cast<std::time_t>(unixTime);
            std::tm tmResult{};
            gmtime_r(&t, &tmResult);
            std::strftime(outBuf, outSize, "%Y-%m-%dT%H:%M:%SZ", &tmResult);
        };

        char atimeBuf[32];
        char ctimeBuf[32];
        char mtimeBuf[32];
        formatTime(I_atime, atimeBuf, sizeof(atimeBuf));
        formatTime(I_ctime, ctimeBuf, sizeof(ctimeBuf));
        formatTime(I_mtime, mtimeBuf, sizeof(mtimeBuf));

        std::printf("I_uid: %d\n", I_uid);
        std::printf("I_gid: %d\n", I_gid);
        std::printf("I_size: %d\n", I_size);
        std::printf("I_atime: %s\n", atimeBuf);
        std::printf("I_ctime: %s\n", ctimeBuf);
        std::printf("I_mtime: %s\n", mtimeBuf);

        std::printf("I_block: [");
        for (int i = 0; i < 15; ++i)
        {
            std::printf("%d", I_block[i]);
            if (i < 14)
                std::printf(" ");
        }
        std::printf("]\n");

        std::printf("I_type: %.1s\n", I_type);
        std::printf("I_perm: %.3s\n", I_perm);
    }

}