#pragma once

#include <string>

namespace Utilities
{

    //   K -> size * 1024
    //   M -> size * 1024 * 1024
    // Devuelve true y llena outBytes si la unidad es valida; si no,
    // devuelve false y llena errMsg
    bool ConvertBytes(int size, const std::string &unit, long long &outBytes, std::string &errMsg);

    extern const std::string Carnet;

    // Asigna una letra unica (A, B, C, ...) por cada path de disco distinto,
    // en el orden en que se van montando particiones por primera vez
    bool GetLetra(const std::string &path, std::string &letterOut, std::string &errMsg);

    // Crea el directorio padre de "path" si no existe
    bool CreateParentDir(const std::string &path, std::string &errMsg);

    //   outputImage = path (la imagen final pedida por el usuario)
    //   dotFileName = path con la extension cambiada a ".dot"
    void GetFileNames(const std::string &path, std::string &dotFileName, std::string &outputImage);

}