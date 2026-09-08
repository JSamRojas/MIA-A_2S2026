#include "Disk.h"

#include "../../Structs/Str_Ebr/EBR.h"
#include "../../Utils/Utilities.h"

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>

namespace Reports
{

    namespace
    {
        std::string trimNulls(const char *data, std::size_t len)
        {
            std::string s(data, len);
            std::size_t nul = s.find('\0');
            if (nul != std::string::npos)
                s = s.substr(0, nul);
            return s;
        }

        std::string fmt2(double v)
        {
            std::ostringstream oss;
            oss << std::fixed << std::setprecision(2) << v;
            return oss.str();
        }
    }

    bool ReporteDISK(const Structs::MBR &mbr, const std::string &path, const std::string &diskPath, std::string &errMsg)
    {
        if (!Utilities::CreateParentDir(path, errMsg))
        {
            return false;
        }

        std::string dotFileName, outputImage;
        Utilities::GetFileNames(path, dotFileName, outputImage);

        // sizeof(Structs::MBR) empacado (13 + 4*35 = 153)
        const double mbr_Size = 153.0;

        double total_Size = static_cast<double>(mbr.Mbr_size);
        double usable_Size = total_Size - mbr_Size;

        std::string disk_Name = std::filesystem::path(diskPath).filename().string();

        double mbr_Percentage = (mbr_Size / total_Size) * 100.0;

        std::ostringstream dot;
        dot << "digraph G {\n"
            << "\tlabelloc=\"t\";\n"
            << "\tlabel = \"Reporte de Disco: " << disk_Name << " (Size: "
            << static_cast<long long>(total_Size) << " bytes)\";\n"
            << "\tnode [shape=plaintext];\n\n"
            << "\ttabla [label=<\n"
            << "\t<table border=\"1\" cellborder=\"1\" cellspacing=\"0\" cellpadding=\"10\"\n"
            << "\tbgcolor=\"#F9F9F9\">\n"
            << "\t<tr><td rowspan=\"2\" bgcolor=\"#A95C68\" border=\"1\" color=\"black\"><b>MBR</b><br/>"
            << static_cast<long long>(mbr_Size) << " bytes<br/>"
            << fmt2(mbr_Percentage) << "% del Disco</td>";

        std::string partition_Rows;
        std::string logical_Rows;
        bool extendend_Found = false;
        double space_Used = mbr_Size;
        double free_Space = usable_Size;
        double free_Extended = 0.0;
        (void)space_Used;

        // RECORRER LAS PARTICIONES PRIMARIAS Y LAS EXTENDIDAS
        for (const auto &part : mbr.Mbr_partitions)
        {
            if (part.Partition_size == 0)
                continue;

            std::string part_Name = trimNulls(part.Partition_name, sizeof(part.Partition_name));
            char part_Type = part.Partition_type[0];
            double part_Size = static_cast<double>(part.Partition_size);
            double part_Percentage = (part_Size / total_Size) * 100.0;

            space_Used += part_Size;
            free_Space -= part_Size;

            // SI LA PARTICION ES EXTENDIDA
            if (part_Type == 'E')
            {
                extendend_Found = true;

                std::ostringstream row;
                row << "\n\t\t<td colspan=\"20\" bgcolor=\"#F0E68C\" border=\"1\" color=\"black\"><b>EXTENDIDA<br/>"
                    << fmt2(part_Percentage) << "% del Disco</b></td>";
                partition_Rows += row.str();

                std::ifstream file(diskPath, std::ios::binary);
                if (!file.is_open())
                {
                    errMsg = "error al abrir el archivo del disco";
                    return false;
                }

                // Nos movemos al inicio de la particion extendida
                Structs::EBR ebr{};
                file.seekg(part.Partition_start, std::ios::beg);
                file.read(reinterpret_cast<char *>(&ebr), sizeof(Structs::EBR));
                if (!file)
                {
                    errMsg = "error al leer el primer EBR";
                    return false;
                }

                // Si el EBR no existe, no hay particiones logicas; si existe,
                // se recorren todos los EBRs y por ende las particiones logicas
                while (true)
                {
                    std::string ebr_Name = trimNulls(ebr.Partition_name, sizeof(ebr.Partition_name));
                    double ebr_Size = static_cast<double>(ebr.Partition_size);
                    double ebr_Percent = (ebr_Size / total_Size) * 100.0;

                    std::ostringstream lrow;
                    lrow << "\n\t\t<td bgcolor=\"#D27D2D\" border=\"1\" color=\"black\">EBR</td>\n"
                         << "\t\t<td bgcolor=\"#C2B280\" border=\"1\" color=\"black\">Logica<br/>"
                         << ebr_Name << "<br/>" << fmt2(ebr_Percent) << "% del Disco</td>";
                    logical_Rows += lrow.str();

                    // Considerando un EBR y una particion logica
                    space_Used += ebr_Size + ebr_Size;

                    if (ebr.Partition_next == -1)
                    {
                        // Espacio libre dentro de la particion extendida,
                        // agregado luego de la ultima particion logica
                        free_Extended = static_cast<double>(part.Partition_size) -
                                        static_cast<double>(ebr.Partition_start - part.Partition_start) -
                                        ebr_Size;

                        if (free_Extended > 0)
                        {
                            std::ostringstream frow;
                            frow << "\n\t\t<td bgcolor=\"#E0E0E0\" border=\"1\" color=\"black\"> Espacio Libre en extendida<br/>"
                                 << fmt2((free_Extended / total_Size) * 100.0) << "% del Disco</td>";
                            logical_Rows += frow.str();
                        }
                        break;
                    }

                    file.seekg(ebr.Partition_next, std::ios::beg);
                    file.read(reinterpret_cast<char *>(&ebr), sizeof(Structs::EBR));
                    if (!file)
                    {
                        errMsg = "error al leer el siguiente EBR";
                        return false;
                    }
                }
            }
            else
            {
                if (part_Type != '0')
                {
                    std::ostringstream row;
                    row << "\n\t\t<td rowspan=\"2\" bgcolor=\"#DAA06D\" border=\"1\" color=\"black\"><b>"
                        << part_Name << "</b><br/>" << static_cast<long long>(part_Size)
                        << " bytes<br/>" << fmt2(part_Percentage) << "% del Disco</td>";
                    partition_Rows += row.str();
                }
            }
        }

        // Una vez agregado todo el espacio utilizado, por ultimo se coloca
        // el espacio libre dentro del disco
        if (free_Space > 0)
        {
            double free_Space_Percent = (free_Space / total_Size) * 100.0;
            std::ostringstream row;
            row << "\n\t\t<td rowspan=\"2\" bgcolor=\"#E0E0E0\" border=\"1\" color=\"black\">Espacio Libre <br/>"
                << fmt2(free_Space_Percent) << "% del Disco</td>";
            partition_Rows += row.str();
        }

        if (!extendend_Found)
        {
            dot << partition_Rows << "</tr></table>>]; }";
        }
        else
        {
            dot << partition_Rows << "</tr><tr>" << logical_Rows << "</tr></table>>]; }";
        }

        // GUARDAR EL CONTENIDO DEL DOT EN UN ARCHIVO
        std::ofstream dotFile(dotFileName, std::ios::binary | std::ios::trunc);
        if (!dotFile.is_open())
        {
            errMsg = "error al crear el archivo .dot";
            return false;
        }

        dotFile << dot.str();
        dotFile.close();
        if (!dotFile)
        {
            errMsg = "error al escribir en el archivo .dot";
            return false;
        }

        // Se ejecuta el comando de Graphviz para generar la imagen
        std::string cmd = "dot -Tpng \"" + dotFileName + "\" -o \"" + outputImage + "\"";
        int ret = std::system(cmd.c_str());
        if (ret != 0)
        {
            errMsg = "error al ejecutar el comando Graphviz";
            return false;
        }

        return true;
    }

}