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

        std::string gerr;
        const Structs::PARTITION *partition = mbrOut.GetPartitionByID(id, gerr);
        if (partition == nullptr)
        {
            errMsg = gerr;
            return false;
        }

        diskPathOut = path;
        errMsg.clear();
        return true;
    }

}