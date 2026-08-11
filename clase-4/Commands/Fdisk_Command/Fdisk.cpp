#include "Fdisk.h"

#include <regex>
#include <algorithm>
#include <cctype>
#include <sstream>

namespace Commands
{

    // ---- Helpers locales a este archivo --------------------------------

    static std::string toUpper(std::string s)
    {
        std::transform(s.begin(), s.end(), s.begin(),
                       [](unsigned char c)
                       { return std::toupper(c); });
        return s;
    }

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

    // ---- Fdisk_Command -------------------------------------------------

    CommandResult Fdisk_Command(const std::vector<std::string> &tokens)
    {

        std::string atributos = joinTokens(tokens);

        static const std::regex lexic(
            R"(-size=\d+|-unit=[bBkKmM]|-fit=[bBfF]{2}|-path="[^"]+"|-path=[^\s]+|-type=[pPeElL]|-name="[^"]+"|-name=[^\s]+)",
            std::regex::icase);

        // Busca todas las coincidencias del patron dentro de "atributos"
        std::vector<std::string> found;
        auto begin = std::sregex_iterator(atributos.begin(), atributos.end(), lexic);
        auto end = std::sregex_iterator();
        for (auto it = begin; it != end; ++it)
        {
            found.push_back(it->str());
        }

        // Si la cantidad de coincidencias no calza con la cantidad de tokens,
        // significa que algun token no es un parametro valido. Se recorre
        // token por token para identificar cual es el problematico.
        if (found.size() != tokens.size())
        {
            for (const auto &token : tokens)
            {
                if (!std::regex_search(token, lexic))
                {
                    return {false, "ERROR: Parametro no reconocido: " + token + " en comando FDISK"};
                }
            }
        }

        // Variables locales donde se va guardando lo que se reconoce.
        bool hasSize = false;
        bool hasPath = false;
        bool hasName = false;
        int sizeVal = 0;
        std::string unitVal;
        std::string fitVal;
        std::string pathVal;
        std::string typeVal;
        std::string nameVal;

        // Se procesa cada parametro encontrado (ej: "-size=100")
        for (const auto &fun : found)
        {

            size_t eqPos = fun.find('=');
            if (eqPos == std::string::npos)
            {
                return {false, "ERROR: Parametro invalido: " + fun};
            }

            // key = lo que esta antes del "=", value = lo que esta despues
            std::string key = toLowerStr(fun.substr(0, eqPos));
            std::string value = fun.substr(eqPos + 1);

            // Si el valor viene entre comillas, se quitan (ej: -path="C:\a b")
            if (value.size() >= 2 && value.front() == '"' && value.back() == '"')
            {
                value = value.substr(1, value.size() - 2);
            }

            if (key == "-size")
            {
                try
                {
                    size_t charsUsados = 0;
                    int size = std::stoi(value, &charsUsados);
                    // Si stoi no consumio todo el string, habia texto invalido
                    // pegado al numero (ej: "100abc")
                    if (charsUsados != value.size() || size <= 0)
                    {
                        return {false, "ERROR: El size de la particion debe ser un numero entero positivo"};
                    }
                    sizeVal = size;
                    hasSize = true;
                }
                catch (...)
                {
                    // std::stoi lanza excepcion si el valor no es un numero valido
                    return {false, "ERROR: El size de la particion debe ser un numero entero positivo"};
                }
            }
            else if (key == "-unit")
            {
                std::string v = toUpper(value);
                if (v != "B" && v != "K" && v != "M")
                {
                    return {false, "ERROR: la unidad de la particion debe ser B, K o M"};
                }
                unitVal = v;
            }
            else if (key == "-path")
            {
                if (value.empty())
                {
                    return {false, "ERROR: El path de la particion no puede ser vacio"};
                }
                pathVal = value;
                hasPath = true;
            }
            else if (key == "-type")
            {
                std::string v = toUpper(value);
                if (v != "P" && v != "E" && v != "L")
                {
                    return {false, "ERROR: la unidad de la particion debe ser B, K o M"};
                }
                typeVal = v;
            }
            else if (key == "-fit")
            {
                std::string v = toUpper(value);
                if (v != "BF" && v != "FF" && v != "WF")
                {
                    return {false, "ERROR: El ajuste de la particion debe ser BF, FF o WF"};
                }
                fitVal = v;
            }
            else if (key == "-name")
            {
                if (value.empty())
                {
                    return {false, "ERROR: El name de la particion no puede ser vacio"};
                }
                nameVal = value;
                hasName = true;
            }
            else
            {
                return {false, "ERROR: Parametro no reconocido: " + key};
            }
        }

        // Validaciones finales
        if (!hasSize)
        {
            return {false, "ERROR: La capacidad de la particion no puede ser 0"};
        }

        if (!hasPath)
        {
            return {false, "ERROR: El path de la particion no puede ser vacio"};
        }

        if (!hasName)
        {
            return {false, "ERROR: El name de la particion no puede ser vacio"};
        }

        // Valores por defecto
        if (unitVal.empty())
        {
            unitVal = "K";
        }

        if (typeVal.empty())
        {
            typeVal = "P";
        }

        if (fitVal.empty())
        {
            fitVal = "WF";
        }

        // Se arma un mensaje mostrando lo que se reconocio, para imprimirlo
        // en consola desde el Analyzer
        std::ostringstream msg;
        msg << "FDISK: Parametros validados con exito -> "
            << "size=" << sizeVal
            << ", unit=" << unitVal
            << ", path=" << pathVal
            << ", type=" << typeVal
            << ", fit=" << fitVal
            << ", name=" << nameVal;

        return {true, msg.str()};
    }

}