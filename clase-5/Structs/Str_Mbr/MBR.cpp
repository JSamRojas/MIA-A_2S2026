#include "MBR.h"

#include <cstdio>
#include <cstring>
#include <ctime>
#include <fstream>
#include <random>

namespace Structs
{

    namespace
    {
        int32_t randomSignature()
        {
            static std::mt19937 rng(std::random_device{}());
            static std::uniform_int_distribution<int32_t> dist(0, 2147483647);
            return dist(rng);
        }
    }

    bool CreateMBR(const MKDISK &disk, long long sizeB, std::string &errMsg)
    {
        char fByte;

        if (disk.Fit == "BF")
            fByte = 'B';
        else if (disk.Fit == "FF")
            fByte = 'F';
        else if (disk.Fit == "WF")
            fByte = 'W';
        else
        {
            errMsg = "ERROR: Ajuste no reconocido";
            return false;
        }

        MBR mbr{};
        std::memset(&mbr, 0, sizeof(MBR));

        mbr.Mbr_size = static_cast<int32_t>(sizeB);
        mbr.Mbr_date = static_cast<float>(std::time(nullptr));
        mbr.Mbr_signature_disk = randomSignature();
        mbr.Mbr_disk_fit[0] = fByte;

        for (int i = 0; i < 4; ++i)
            mbr.Mbr_partitions[i] = EmptyPartition();

        if (!mbr.SerializeMBR(disk.Path, errMsg))
            return false;

        return true;
    }

    bool MBR::SerializeMBR(const std::string &path, std::string &errMsg)
    {
        // El archivo ya fue creado (y rellenado) por MkDisk, asi que se abre
        // en modo lectura/escritura SIN truncar, para solo sobreescribir los
        // primeros sizeof(MBR) bytes
        std::fstream file(path, std::ios::binary | std::ios::in | std::ios::out);
        if (!file.is_open())
        {
            errMsg = "ERROR: No se pudo abrir el archivo al intentar serializarlo";
            return false;
        }

        file.seekp(0);
        file.write(reinterpret_cast<const char *>(this), sizeof(MBR));

        if (!file)
        {
            errMsg = "ERROR: No se pudo escribir el MBR en el archivo";
            return false;
        }

        return true;
    }

    bool MBR::DeserializeMBR(const std::string &path, std::string &errMsg)
    {
        std::ifstream file(path, std::ios::binary);
        if (!file.is_open())
        {
            errMsg = "ERROR: No se pudo deserializar el MBR";
            return false;
        }

        file.read(reinterpret_cast<char *>(this), sizeof(MBR));

        if (!file)
        {
            errMsg = "ERROR: No se pudo leer el archivo a deserealizar";
            return false;
        }

        return true;
    }

    void MBR::Print() const
    {
        std::printf("---------- MBR ----------\n");
        std::printf("Mbr_size: %d\n", Mbr_size);
        std::printf("Mbr_date: %.0f\n", Mbr_date);
        std::printf("Mbr_signature_disk: %d\n", Mbr_signature_disk);
        std::printf("Mbr_disk_fit: %c\n", Mbr_disk_fit[0]);
        std::printf("---------- END MBR ----------\n");
    }

}