#include "MBR.h"

#include <cctype>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <fstream>
#include <random>

namespace Structs
{

    namespace
    {
        // comparacion case-insensitive
        bool equalFold(const std::string &a, const std::string &b)
        {
            if (a.size() != b.size())
                return false;

            for (std::size_t i = 0; i < a.size(); ++i)
            {
                if (std::tolower(static_cast<unsigned char>(a[i])) !=
                    std::tolower(static_cast<unsigned char>(b[i])))
                    return false;
            }

            return true;
        }

        // recorta bytes nulos por ambos lados
        std::string trimNulBoth(const std::string &s)
        {
            std::size_t start = s.find_first_not_of('\0');
            if (start == std::string::npos)
                return "";

            std::size_t end = s.find_last_not_of('\0');
            return s.substr(start, end - start + 1);
        }
    }

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
        // El archivo ya fue creado (y rellenado) por MakeDisk, asi que se abre
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

    PARTITION *MBR::GetFirstPartitionAvailable(int &startOut, int &indexOut, std::string &errMsg)
    {
        int offset = static_cast<int>(sizeof(MBR));

        for (int i = 0; i < 4; ++i)
        {
            if (Mbr_partitions[i].Partition_start == -1)
            {
                startOut = offset;
                indexOut = i;
                errMsg.clear();
                return &Mbr_partitions[i];
            }
            else
            {
                offset += Mbr_partitions[i].Partition_size;
            }
        }

        startOut = -1;
        indexOut = -1;
        errMsg.clear();
        return nullptr;
    }

    const PARTITION *MBR::GetPartitionByID(const std::string &id, std::string &errMsg) const
    {
        std::string inputID = trimNulBoth(id);

        for (int i = 0; i < 4; ++i)
        {
            const PARTITION &p = Mbr_partitions[i];

            std::string partitionID(p.Partition_id, sizeof(p.Partition_id));
            partitionID = trimNulBoth(partitionID);

            if (equalFold(partitionID, inputID))
            {
                errMsg.clear();
                return &p;
            }
        }

        errMsg = "No se encontro la particion con el id: " + id;
        return nullptr;
    }

    PARTITION *MBR::GetPartitionByName(const std::string &name, int &indexOut, std::string &errMsg)
    {

        std::string inputName = trimNulBoth(name);

        for (int i = 0; i < 4; ++i)
        {
            PARTITION &p = Mbr_partitions[i];

            std::string partitionName(p.Partition_name, sizeof(p.Partition_name));
            partitionName = trimNulBoth(partitionName);

            if (equalFold(partitionName, inputName))
            {
                indexOut = i;
                errMsg.clear();
                return &p;
            }
        }

        indexOut = -1;
        errMsg = "No se encontro la particion con el nombre: " + name;
        return nullptr;
    }

    void MBR::UpdatePartitionNumber()
    {
        int number = 1;

        for (int i = 0; i < 4; ++i)
        {
            PARTITION &partition = Mbr_partitions[i];

            if (partition.Partition_status[0] != 0 && partition.Partition_type[0] == 'P')
            {
                partition.Partition_number = number;
                number++;
            }
            else if (partition.Partition_type[0] == 'E')
            {
                partition.Partition_number = 0;
            }
        }
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