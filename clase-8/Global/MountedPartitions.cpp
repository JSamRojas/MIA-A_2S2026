#include "MountedPartitions.h"

namespace GlobalNS
{

    std::unordered_map<std::string, std::string> MountedPartitions;

    bool GetEssentialRep(const std::string &id,
                         Structs::MBR &mbrOut,
                         std::string &diskPathOut,
                         std::string &errMsg)
    {
        auto it = MountedPartitions.find(id);
        if (it == MountedPartitions.end() || it->second.empty())
        {
            errMsg = "no se encontro la particion montada";
            return false;
        }

        std::string path = it->second;

        if (!mbrOut.DeserializeMBR(path, errMsg))
        {
            return false;
        }

        diskPathOut = path;
        errMsg.clear();
        return true;
    }

    bool GetMountedPartition(const std::string &id,
                             Structs::PARTITION &partOut,
                             std::string &pathOut,
                             std::string &errMsg)
    {
        // 1) Se busca el id en la tabla de particiones montadas
        auto it = MountedPartitions.find(id);
        if (it == MountedPartitions.end() || it->second.empty())
        {
            errMsg = "id de particion invalido: la particion no esta montada";
            return false;
        }

        std::string path = it->second;

        // 2) Se lee el MBR del disco donde vive esa particion
        Structs::MBR mbr{};
        if (!mbr.DeserializeMBR(path, errMsg))
        {
            return false;
        }

        // 3) Se busca, dentro de ese MBR, la particion con ese id exacto
        std::string gerr;
        const Structs::PARTITION *part = mbr.GetPartitionByID(id, gerr);
        if (part == nullptr)
        {
            errMsg = gerr;
            return false;
        }

        // 4) Se copia la particion encontrada hacia afuera (el MBR local
        //    "mbr" desaparece al salir de esta funcion, asi que no se
        //    puede devolver un puntero a su interior).
        partOut = *part;
        pathOut = path;
        errMsg.clear();
        return true;
    }

}