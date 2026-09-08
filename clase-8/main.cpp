#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "Analyzer/Analyzer.h"

// Divide un texto en lineas (equivalente a separar por "\n")
static std::vector<std::string> splitLines(const std::string &text)
{
    std::vector<std::string> lines;
    std::istringstream stream(text);
    std::string line;
    while (std::getline(stream, line))
    {
        lines.push_back(line);
    }
    return lines;
}

int main()
{

    // Aqui se define el codigo/texto a analizar dentro del propio programa.
    std::string code =
        "mkdisk -size=30 -unit=M -path=/home/jonatan/Descargas/Disco1.mia\n"
        "fdisk -size=3000 -path=/home/jonatan/Descargas/Disco1.mia -name=Particion1\n"
        "fdisk -type=E -unit=k -name=Particion2 -size=3000 -path=/home/jonatan/Descargas/Disco1.mia -fit=BF\n"
        "fdisk -type=L -unit=k -name=Particion21 -size=1000 -path=/home/jonatan/Descargas/Disco1.mia -fit=BF\n"
        "fdisk -type=L -unit=k -name=Part22 -size=1024 -path=/home/jonatan/Descargas/Disco1.mia -fit=BF\n"
        "fdisk -size=3000 -path=/home/jonatan/Descargas/Disco1.mia -name=Particion3\n"
        "fdisk -size=3000 -path=/home/jonatan/Descargas/Disco1.mia -name=Particion4\n"
        "mount -path=/home/jonatan/Descargas/Disco1.mia -name=Particion1\n"
        "mkfs -id=611A\n"
        "rep -id=611A -path=/home/jonatan/Descargas/Reportes/repDisco1.jpg -name=disk\n"
        "rep -id=611A -path=/home/jonatan/Descargas/Reportes/repMBR.jpg -name=mbr\n"
        "\n"
        "comandoInexistente -x=1";

    std::vector<std::string> lines = splitLines(code);

    for (const auto &line : lines)
    {
        // Se manda un vector con
        // un solo elemento, ya que Analyzer solo usa inputs[0].
        Analyzer::Analyze(std::vector<std::string>{line});
    }

    return 0;
}