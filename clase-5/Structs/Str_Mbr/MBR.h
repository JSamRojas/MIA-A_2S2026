#pragma once

#include <cstdint>
#include <string>

#include "../Str_Partition/PARTITION.h"
#include "../Str_Mkdisk/MKDISK.h"

namespace Structs
{

#pragma pack(push, 1)
    struct MBR
    {
        int32_t Mbr_size;
        float Mbr_date;
        int32_t Mbr_signature_disk;
        char Mbr_disk_fit[1];
        PARTITION Mbr_partitions[4];

        bool SerializeMBR(const std::string &path, std::string &errMsg);

        bool DeserializeMBR(const std::string &path, std::string &errMsg);

        void Print() const;
    };
#pragma pack(pop)

    // Arma el MBR (usando disk.Fit y sizeB) y lo escribe al inicio de disk.Path
    bool CreateMBR(const MKDISK &disk, long long sizeB, std::string &errMsg);

}