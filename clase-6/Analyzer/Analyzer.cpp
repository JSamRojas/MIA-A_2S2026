#include "Analyzer.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include "../Commands/Mkdisk_Command/Mkdisk.h"
#include "../Commands/Fdisk_Command/Fdisk.h"
#include "../Commands/Mount_Command/Mount.h"
#include "../Commands/Rep_Command/Rep.h"

namespace Analyzer
{

    // ---- Helpers ----------------------------------------------------------

    static std::string trim(const std::string &s)
    {
        size_t start = s.find_first_not_of(" \t\r\n");
        if (start == std::string::npos)
            return "";
        size_t end = s.find_last_not_of(" \t\r\n");
        return s.substr(start, end - start + 1);
    }

    static std::vector<std::string> fields(const std::string &s)
    {
        std::vector<std::string> tokens;
        std::istringstream iss(s);
        std::string tok;
        while (iss >> tok)
        {
            tokens.push_back(tok);
        }
        return tokens;
    }

    static std::string toLower(std::string s)
    {
        std::transform(s.begin(), s.end(), s.begin(),
                       [](unsigned char c)
                       { return std::tolower(c); });
        return s;
    }

    // ---- Comandos simulados -------------------------------------------
    // Aqui se reemplazan las llamadas reales por strings de resultado.

    // static std::string Mkdisk_Command(const std::vector<std::string> &params) { return "Comando 'mkdisk' ejecutado correctamente"; }
    static std::string Rmdisk_Command(const std::vector<std::string> &params) { return "Comando 'rmdisk' ejecutado correctamente"; }
    // static std::string Fdisk_Command(const std::vector<std::string> &params) { return "Comando 'fdisk' ejecutado correctamente"; }
    // static std::string Mount_Command(const std::vector<std::string> &params) { return "Comando 'mount' ejecutado correctamente"; }
    static std::string Mkfs_Command(const std::vector<std::string> &params) { return "Comando 'mkfs' ejecutado correctamente"; }
    static std::string Mkusr_Command(const std::vector<std::string> &params) { return "Comando 'mkusr' ejecutado correctamente"; }
    static std::string Rmusr_Command(const std::vector<std::string> &params) { return "Comando 'rmusr' ejecutado correctamente"; }
    static std::string Mkfile_Command(const std::vector<std::string> &params) { return "Comando 'mkfile' ejecutado correctamente"; }

    // ---- Analyze ------------------------------------------------------

    void Analyze(const std::vector<std::string> &inputs)
    {

        // Si no se proporciona ningun comando, se imprime un error
        if (inputs.empty())
        {
            std::cout << "[ERROR] No se proporciono ningun comando" << std::endl;
            return;
        }

        // Se confirma si el comando no es una linea en blanco
        // Si lo es, se imprime tal cual
        std::string input = trim(inputs[0]);
        if (input.empty())
        {
            std::cout << "\n"
                      << input << "\n"
                      << std::endl;
            return;
        }

        std::vector<std::string> tokens = fields(input);
        if (tokens.empty())
        {
            std::cout << "[ERROR] No se proporciono ningun comando valido" << std::endl;
            return;
        }

        tokens[0] = toLower(tokens[0]);
        std::vector<std::string> params(tokens.begin() + 1, tokens.end());

        std::string msg;
        bool hasError = false;
        std::string errorMsg;

        if (tokens[0] == "mkdisk")
        {
            Commands::CommandResult result = Commands::Mkdisk_Command(params);

            if (result.success)
            {
                msg = result.message;
            }
            else
            {
                hasError = true;
                errorMsg = result.message;
            }
        }
        else if (tokens[0] == "rmdisk")
        {
            msg = Rmdisk_Command(params);
        }
        else if (tokens[0] == "fdisk")
        {
            Commands::CommandResult result = Commands::Fdisk_Command(params);

            if (result.success)
            {
                msg = result.message;
            }
            else
            {
                hasError = true;
                errorMsg = result.message;
            }
        }
        else if (tokens[0] == "mount")
        {
            Commands::CommandResult result = Commands::Mount_Command(params);

            if (result.success)
            {
                msg = result.message;
            }
            else
            {
                hasError = true;
                errorMsg = result.message;
            }
        }
        else if (tokens[0] == "mkfs")
        {
            msg = Mkfs_Command(params);
        }
        else if (tokens[0] == "mkusr")
        {
            msg = Mkusr_Command(params);
        }
        else if (tokens[0] == "rmusr")
        {
            msg = Rmusr_Command(params);
        }
        else if (tokens[0] == "mkfile")
        {
            msg = Mkfile_Command(params);
        }
        else if (tokens[0] == "rep")
        {
            Commands::CommandResult result = Commands::Rep_Command(params);

            if (result.success)
            {
                msg = result.message;
            }
            else
            {
                hasError = true;
                errorMsg = result.message;
            }
        }
        else
        {
            hasError = true;
            errorMsg = "comando no reconocido: " + tokens[0];
        }

        if (hasError)
        {
            std::cout << "[ERROR] " << errorMsg << std::endl;
        }
        else
        {
            std::cout << msg << std::endl;
        }
    }

}