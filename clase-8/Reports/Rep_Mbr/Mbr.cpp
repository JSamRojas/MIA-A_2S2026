#include "Mbr.h"

#include "../../Structs/Str_Ebr/EBR.h"
#include "../../Utils/Utilities.h"

#include <cstdlib>
#include <ctime>
#include <fstream>
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

        // Formatea un timestamp Unix (guardado como float) a texto legible.
        std::string formatUnixDate(float unixTime)
        {
            std::time_t t = static_cast<std::time_t>(unixTime);
            std::tm tmResult{};
            localtime_r(&t, &tmResult);

            char buf[64];
            std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tmResult);
            return std::string(buf);
        }
    }

    bool ReporteMBR(const Structs::MBR &mbr, const std::string &path, const std::string &diskPath, std::string &errMsg)
    {
        if (!Utilities::CreateParentDir(path, errMsg))
        {
            return false;
        }

        std::string dotFileName, outputImage;
        Utilities::GetFileNames(path, dotFileName, outputImage);

        // ---- Encabezado de la tabla: datos generales del MBR ----
        std::ostringstream dot;
        dot << "digraph G {\n"
            << "\tnode [shape=plaintext]\n"
            << "\ttabla [label =<\n"
            << "\t<table border=\"0\" cellborder=\"1\" cellspacing=\"0\">\n"
            << "\t\t<tr><td colspan=\"2\" bgcolor=\"lightblue\"> REPORTE MBR </td></tr>\n"
            << "\t\t<tr><td bgcolor = \"lightblue1\"> MBR_SIZE </td><td> " << mbr.Mbr_size << " </td></tr>\n"
            << "\t\t<tr><td bgcolor = \"lightblue1\"> MBR_DATE </td><td> " << formatUnixDate(mbr.Mbr_date) << " </td></tr>\n"
            << "\t\t<tr><td bgcolor = \"lightblue1\"> MBR_DISK_SIGNATURE </td><td> " << mbr.Mbr_signature_disk << " </td></tr>";

        // ---- Se agregan las 4 particiones (todas, incluso las vacias) ----
        for (int i = 0; i < 4; ++i)
        {
            const Structs::PARTITION &part = mbr.Mbr_partitions[i];

            std::string part_Name = trimNulls(part.Partition_name, sizeof(part.Partition_name));
            char part_Status = part.Partition_status[0];
            char part_Type = part.Partition_type[0];
            char part_Fit = part.Partition_fit[0];

            dot << "\n\t\t<tr><td colspan=\"2\" bgcolor=\"royalblue\"> PARTICION " << (i + 1) << " </td></tr>\n"
                << "\t\t<tr><td bgcolor = \"dodgerblue\"> PART_STATUS </td><td> " << part_Status << " </td></tr>\n"
                << "\t\t<tr><td bgcolor = \"dodgerblue\"> PART_TYPE </td><td> " << part_Type << " </td></tr>\n"
                << "\t\t<tr><td bgcolor = \"dodgerblue\"> PART_FIT </td><td> " << part_Fit << " </td></tr>\n"
                << "\t\t<tr><td bgcolor = \"dodgerblue\"> PART_START </td><td> " << part.Partition_start << " </td></tr>\n"
                << "\t\t<tr><td bgcolor = \"dodgerblue\"> PART_SIZE </td><td> " << part.Partition_size << " </td></tr>\n"
                << "\t\t<tr><td bgcolor = \"dodgerblue\"> PART_NAME </td><td> " << part_Name << " </td></tr>";

            // Si la particion es extendida, se buscan y agregan sus
            // particiones logicas (recorriendo la cadena de EBRs)
            if (part_Type == 'E')
            {
                std::ifstream file(diskPath, std::ios::binary);
                if (!file.is_open())
                {
                    errMsg = "error al abrir el archivo del disco";
                    return false;
                }

                // Se lee el primer EBR, justo al inicio de la extendida
                Structs::EBR ebr{};
                file.seekg(part.Partition_start, std::ios::beg);
                file.read(reinterpret_cast<char *>(&ebr), sizeof(Structs::EBR));
                if (!file)
                {
                    errMsg = "error al leer el primer EBR";
                    return false;
                }

                // Si el EBR no existe todavia, no hay particiones logicas
                // que reportar; se pasa a la siguiente particion del MBR
                if (ebr.Partition_size == 0)
                {
                    continue;
                }

                // Se recorre toda la cadena de EBRs
                while (true)
                {
                    std::string ebr_Name = trimNulls(ebr.Partition_name, sizeof(ebr.Partition_name));
                    char ebr_Status = ebr.Partition_mount[0];
                    char ebr_Fit = ebr.Partition_fit[0];

                    dot << "\n\t\t<tr><td colspan=\"2\" bgcolor=\"forestgreen\"> PARTICION LOGICA </td></tr>\n"
                        << "\t\t<tr><td bgcolor=\"chartreuse2\"> EBR_STATUS </td><td> " << ebr_Status << " </td></tr>\n"
                        << "\t\t<tr><td bgcolor=\"chartreuse2\"> EBR_FIT </td><td> " << ebr_Fit << " </td></tr>\n"
                        << "\t\t<tr><td bgcolor=\"chartreuse2\"> EBR_START </td><td> " << ebr.Partition_start << " </td></tr>\n"
                        << "\t\t<tr><td bgcolor=\"chartreuse2\"> EBR_SIZE </td><td> " << ebr.Partition_size << " </td></tr>\n"
                        << "\t\t<tr><td bgcolor=\"chartreuse2\"> EBR_NEXT </td><td> " << ebr.Partition_next << " </td></tr>\n"
                        << "\t\t<tr><td bgcolor=\"chartreuse2\"> EBR_NAME </td><td> " << ebr_Name << " </td></tr>";

                    // Si no hay siguiente EBR, se termina la cadena
                    if (ebr.Partition_next == -1)
                    {
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
        }

        dot << "\n\t</table>>] }";

        // ---- Se guarda el .dot y se genera la imagen con Graphviz ----
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
            errMsg = "error al escribir el archivo .dot";
            return false;
        }

        std::string cmd = "dot -Tpng \"" + dotFileName + "\" -o \"" + outputImage + "\"";
        int ret = std::system(cmd.c_str());
        if (ret != 0)
        {
            errMsg = "error al ejecutar el comando dot";
            return false;
        }

        return true;
    }

}