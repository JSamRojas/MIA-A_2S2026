#include "PARTITION.h"

#include <algorithm>
#include <cstdio>
#include <cstring>

namespace Structs
{

    PARTITION EmptyPartition()
    {
        PARTITION p{};
        std::memset(&p, 0, sizeof(PARTITION));

        // 0 = creada, 1 = montada, 2 = disponible
        p.Partition_status[0] = '2';
        p.Partition_type[0] = '0';
        p.Partition_fit[0] = '0';
        p.Partition_start = -1;
        p.Partition_size = -1;
        p.Partition_name[0] = '0';
        p.Partition_number = 0;
        p.Partition_id[0] = '0';

        return p;
    }

    void PARTITION::CreatePartition(int partStart, int partSize,
                                    const std::string &partType,
                                    const std::string &partFit,
                                    const std::string &partName)
    {
        // 0 = particion creada, 1 = particion montada, 2 = particion disponible
        Partition_status[0] = '0';

        Partition_start = static_cast<int32_t>(partStart);
        Partition_size = static_cast<int32_t>(partSize);

        if (!partType.empty())
            Partition_type[0] = partType[0];

        if (!partFit.empty())
            Partition_fit[0] = partFit[0];

        std::memset(Partition_name, 0, sizeof(Partition_name));
        std::size_t n = std::min(partName.size(), sizeof(Partition_name));
        std::memcpy(Partition_name, partName.data(), n);
    }

    void PARTITION::MountPartition(int number, const std::string &id)
    {
        Partition_status[0] = '1';
        Partition_number = static_cast<int32_t>(number);

        std::memset(Partition_id, 0, sizeof(Partition_id));
        std::size_t n = std::min(id.size(), sizeof(Partition_id));
        std::memcpy(Partition_id, id.data(), n);
    }

    void PARTITION::Print() const
    {
        std::printf("---------- PARTITION ----------\n");
        std::printf("Partition_status: %c\n", Partition_status[0]);
        std::printf("Partition_type: %c\n", Partition_type[0]);
        std::printf("Partition_fit: %c\n", Partition_fit[0]);
        std::printf("Partition_start: %d\n", Partition_start);
        std::printf("Partition_size: %d\n", Partition_size);
        std::printf("Partition_name: %.16s\n", Partition_name);
        std::printf("Partition_number: %d\n", Partition_number);
        std::printf("Partition_id: %.4s\n", Partition_id);
        std::printf("---------- END PARTITION ----------\n");
    }

}