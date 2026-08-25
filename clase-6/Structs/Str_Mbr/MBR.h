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

        // Devuelve un puntero directo al slot libre dentro de Mbr_partitions
        // (o nullptr si no hay espacio), junto con el offset (byte de inicio
        // en el disco) y el indice del slot
        PARTITION *GetFirstPartitionAvailable(int &startOut, int &indexOut, std::string &errMsg);

        // Se compara el id recibido contra Partition_id (4 bytes,
        // con relleno en 0), que es el campo que MountPartition llena
        const PARTITION *GetPartitionByID(const std::string &id, std::string &errMsg) const;

        // Se busca por Partition_name (recortado de ceros) entre
        // particiones ya creadas (Partition_start != -1)
        PARTITION *GetPartitionByName(const std::string &name, int &indexOut, std::string &errMsg);

        // Asigna numeros secuenciales (1,2,3...) a las particiones
        // primarias usadas, y 0 a la extendida
        void UpdatePartitionNumber();

        void Print() const;
    };
#pragma pack(pop)

    // Arma el MBR (usando disk.Fit y sizeB) y lo escribe al inicio de disk.Path
    bool CreateMBR(const MKDISK &disk, long long sizeB, std::string &errMsg);

}