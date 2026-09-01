#include "Utilities.h"

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <unordered_map>
#include <vector>

namespace Utilities
{

    const std::string Carnet = "61"; // 202200061 - Cambiar por el carnet de cada uno

    namespace
    {
        std::unordered_map<std::string, std::string> pathToLetter;

        const std::vector<std::string> Alfabeto = {
            "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M",
            "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z"};

        int nextLetterIndex = 0;
    }

    bool GetLetra(const std::string &path, std::string &letterOut, std::string &errMsg)
    {
        auto it = pathToLetter.find(path);
        if (it == pathToLetter.end())
        {
            if (nextLetterIndex < static_cast<int>(Alfabeto.size()))
            {
                pathToLetter[path] = Alfabeto[nextLetterIndex];
                nextLetterIndex++;
            }
            else
            {
                errMsg = "no hay letras disponibles, demasiados discos";
                return false;
            }
        }

        letterOut = pathToLetter[path];
        errMsg.clear();
        return true;
    }

    bool ConvertBytes(int size, const std::string &unit, long long &outBytes, std::string &errMsg)
    {
        std::string u = unit;
        std::transform(u.begin(), u.end(), u.begin(),
                       [](unsigned char c)
                       { return std::toupper(c); });

        if (u == "B")
        {
            outBytes = static_cast<long long>(size);
        }
        else if (u == "K")
        {
            outBytes = static_cast<long long>(size) * 1024LL;
        }
        else if (u == "M" || u.empty())
        {
            outBytes = static_cast<long long>(size) * 1024LL * 1024LL;
        }
        else
        {
            errMsg = "unidad invalida: " + unit;
            return false;
        }

        return true;
    }

    bool CreateParentDir(const std::string &path, std::string &errMsg)
    {
        namespace fs = std::filesystem;

        fs::path dir = fs::path(path).parent_path();
        if (!dir.empty())
        {
            std::error_code ec;
            fs::create_directories(dir, ec);
            if (ec)
            {
                errMsg = "no se pudo crear el directorio padre: " + dir.string();
                return false;
            }
        }

        return true;
    }

    void GetFileNames(const std::string &path, std::string &dotFileName, std::string &outputImage)
    {
        namespace fs = std::filesystem;

        outputImage = path;

        fs::path p(path);
        p.replace_extension(".dot");
        dotFileName = p.string();
    }

}