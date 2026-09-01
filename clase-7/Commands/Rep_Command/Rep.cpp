#include "Rep.h"

#include <algorithm>
#include <cctype>
#include <regex>
#include <set>
#include <sstream>

#include "../../Global/MountedPartitions.h"
#include "../../Reports/Rep_Disk/Disk.h"

namespace Commands
{

    // ---- Helpers locales a este archivo --------------------------------

    static std::string toLowerStr(std::string s)
    {
        std::transform(s.begin(), s.end(), s.begin(),
                       [](unsigned char c)
                       { return std::tolower(c); });
        return s;
    }

    static std::string joinTokens(const std::vector<std::string> &tokens)
    {
        std::string result;
        for (size_t i = 0; i < tokens.size(); ++i)
        {
            if (i > 0)
                result += " ";
            result += tokens[i];
        }
        return result;
    }

    static bool contains(const std::vector<std::string> &list, const std::string &value)
    {
        return std::find(list.begin(), list.end(), value) != list.end();
    }

    // ---- Get_type_report --------------------------------------------------
    static bool Get_type_report(const REP &reporte, std::string &errMsg)
    {
        Structs::MBR mbrREP{};
        std::string diskpathREP;

        if (!GlobalNS::GetEssentialRep(reporte.Id, mbrREP, diskpathREP, errMsg))
        {
            return false;
        }

        std::string repErr;
        bool ok = true;

        if (reporte.Name == "disk")
        {
            ok = Reports::ReporteDISK(mbrREP, reporte.Path, diskpathREP, repErr);
        }

        if (!ok)
        {
            std::printf("ERROR: %s\n", repErr.c_str());
        }

        return true;
    }

    // ---- Rep_Command -------------------------------------------------

    CommandResult Rep_Command(const std::vector<std::string> &tokens)
    {

        REP reporte{};

        std::string atributos = joinTokens(tokens);

        static const std::regex lexic(
            R"(-id=[^\s]+|-path="[^"]+"|-path=[^\s]+|-name=[^\s]+|-path_file_ls="[^"]+"|-path_file_ls=[^\s]+)",
            std::regex::icase);

        auto begin = std::sregex_iterator(atributos.begin(), atributos.end(), lexic);
        auto end = std::sregex_iterator();

        std::vector<std::string> found;
        for (auto it = begin; it != end; ++it)
        {
            found.push_back(it->str());
        }

        static const std::vector<std::string> validNames = {
            "mbr", "disk", "inode", "block", "bm_inode", "bm_bloc", "sb", "file", "ls", "tree"};

        for (const auto &fun : found)
        {
            size_t eqPos = fun.find('=');
            if (eqPos == std::string::npos)
            {
                return {false, "ERROR: Parametro invalido: " + fun};
            }

            std::string key = toLowerStr(fun.substr(0, eqPos));
            std::string value = fun.substr(eqPos + 1);

            if (value.size() >= 2 && value.front() == '"' && value.back() == '"')
            {
                value = value.substr(1, value.size() - 2);
            }

            if (key == "-name")
            {
                if (!contains(validNames, value))
                {
                    return {false, "ERROR: nombre de reporte invalido: " + value};
                }
                reporte.Name = value;
            }
            else if (key == "-id")
            {
                if (value.empty())
                {
                    return {false, "ERROR: id invalido, es de caracter obligatorio"};
                }
                reporte.Id = value;
            }
            else if (key == "-path")
            {
                if (value.empty())
                {
                    return {false, "ERROR: path invalido, es de caracter obligatorio"};
                }
                reporte.Path = value;
            }
            else if (key == "-path_file_ls")
            {
                reporte.Path_file = value;
            }
            else
            {
                return {false, "ERROR: Parametro invalido: " + key};
            }
        }

        if (reporte.Name.empty() || reporte.Id.empty() || reporte.Path.empty())
        {
            return {false, "ERROR: Faltan parametros obligatorios"};
        }

        std::string errMsg;
        if (!Get_type_report(reporte, errMsg))
        {
            return {false, errMsg};
        }

        return {true, "COMANDO REP: Reporte realizado con exito"};
    }

}