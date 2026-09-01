#pragma once

#include <cstdint>
#include <string>

namespace Structs
{

#pragma pack(push, 1)
    struct PARTITION
    {
        char Partition_status[1];
        char Partition_type[1];
        char Partition_fit[1];
        int32_t Partition_start;
        int32_t Partition_size;
        char Partition_name[16];
        int32_t Partition_number;
        char Partition_id[4];

        void CreatePartition(int partStart, int partSize,
                             const std::string &partType,
                             const std::string &partFit,
                             const std::string &partName);

        void MountPartition(int number, const std::string &id);

        void Print() const;
    };
#pragma pack(pop)

    // se usan para llenar Mbr_partitions al crear el MBR:
    // status='2', type='0', fit='0', start=-1, size=-1, name[0]='0',
    // number=0, id[0]='0' (resto de cada arreglo queda en 0).
    PARTITION EmptyPartition();

}