#include "Mount.h"

#include <algorithm>
#include <cctype>
#include <regex>
#include <sstream>

#include "../../Global/MountedPartitions.h"
#include "../../Structs/Str_Mbr/MBR.h"
#include "../../Utils/Utilities.h"

namespace Commands
{

    // ---- Helpers locales a este archivo --------------------------------

    // Convierte un string a minusculas, para poder comparar las "keys" de
    // los parametros (-path, -name) sin importar como las escribio el
    // usuario (-PATH=, -Path=, -path=, etc)
    static std::string toLowerStr(std::string s)
    {
        std::transform(s.begin(), s.end(), s.begin(),
                       [](unsigned char c)
                       { return std::tolower(c); });
        return s;
    }

    // Reconstruye el comando completo pegando todos los tokens con un
    // espacio entre ellos, para poder correr el regex sobre un solo
    // string (ej: ["-path=/a.dsk", "-name=Part1"] -> "-path=/a.dsk -name=Part1").
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

    // ------------------------------------------------------------------
    // Genera el "id" unico que identifica a una particion montada, con el
    // formato: <Carnet><numero de particion><letra del disco>
    // Ejemplo: "61" + "1" + "A" -> "611A"
    // ------------------------------------------------------------------
    static bool GetPartitionId(const MOUNT &mount, int indexPartition, std::string &idOut, std::string &errMsg)
    {
        std::string letra;
        if (!Utilities::GetLetra(mount.Path, letra, errMsg))
        {
            errMsg = "ERROR: Error obteniendo letra: " + errMsg;
            return false;
        }

        std::ostringstream oss;
        oss << Utilities::Carnet << indexPartition << letra;
        idOut = oss.str();

        errMsg.clear();
        return true;
    }

    // ---- MountP -------------------------------------------------------
    // Aqui ocurre el montaje "real": se busca la particion dentro del MBR
    // del disco, se valida que se pueda montar, se le genera un id unico
    // y se marca como montada tanto en el MBR (en el disco) como en la
    // tabla global de particiones montadas (en memoria)
    // ------------------------------------------------------------------
    static bool MountP(const MOUNT &mount, std::string &errMsg)
    {
        // 1) Se lee el MBR del disco indicado en -path.
        Structs::MBR mbr{};
        if (!mbr.DeserializeMBR(mount.Path, errMsg))
        {
            errMsg = "error leyendo el MBR del disco: " + errMsg;
            return false;
        }

        // 2) Se busca, dentro de ese MBR, la particion cuyo nombre
        //    coincide con -name. Devuelve un puntero directo al slot
        //    dentro del arreglo Mbr_partitions, y su indice (0-3).
        int indexPartition = -1;
        std::string gerr;
        Structs::PARTITION *partition = mbr.GetPartitionByName(mount.Name, indexPartition, gerr);

        if (partition == nullptr)
        {
            errMsg = "no se encontro la particion con el nombre: " + mount.Name;
            return false;
        }

        // 3) Solo se pueden montar particiones PRIMARIAS. Las extendidas
        //    y logicas no son montables directamente.
        if (partition->Partition_type[0] == 'E' || partition->Partition_type[0] == 'L')
        {
            errMsg = "ERROR: No se puede montar una particion extendida o logica";
            return false;
        }

        // 4) Si ya esta montada (status == '1'), no se vuelve a montar.
        if (partition->Partition_status[0] == '1')
        {
            errMsg = "ERROR: La particion ya esta montada";
            return false;
        }

        // 5) Se recalculan los numeros de particion (1, 2, 3... para las
        //    primarias en uso, 0 para la extendida) antes de montar, para
        //    que el numero este actualizado con el estado mas reciente
        //    del MBR.
        mbr.UpdatePartitionNumber();

        // Se reobtiene el puntero al mismo slot despues de
        // UpdatePartitionNumber (por si acaso, sigue siendo el mismo
        // elemento del arreglo, pero se reobtiene para mayor claridad)
        partition = &mbr.Mbr_partitions[indexPartition];

        // 6) Se genera el id unico de esta particion montada, usando el
        //    numero de particion recien calculado
        std::string id;
        if (!GetPartitionId(mount, static_cast<int>(partition->Partition_number), id, errMsg))
        {
            errMsg = "error obteniendo id de la particion: " + errMsg;
            return false;
        }

        // 7) Se registra el id en la tabla global que asocia
        //    cada id de particion montada con el path de su disco fisico
        GlobalNS::MountedPartitions[id] = mount.Path;

        // 8) Se marca la particion como montada dentro del MBR: status
        //    pasa a '1' y se le asigna el id recien generado
        partition->MountPartition(indexPartition, id);

        // 9) Se guarda el MBR actualizado de vuelta en el disco
        if (!mbr.SerializeMBR(mount.Path, errMsg))
        {
            errMsg = "error escribiendo el MBR del disco: " + errMsg;
            return false;
        }

        return true;
    }

    // ------------------------------------------------------------------
    // Punto de entrada del comando MOUNT: analiza los parametros que
    // escribio el usuario (-path, -name), valida que esten completos, y
    // si todo esta bien, delega el montaje real a MountP
    // ------------------------------------------------------------------
    CommandResult Mount_Command(const std::vector<std::string> &tokens)
    {

        MOUNT mount{};

        std::string atributos = joinTokens(tokens);

        static const std::regex lexic(
            R"(-path="[^"]+"|-path=[^\s]+|-name="[^"]+"|-name=[^\s]+)",
            std::regex::icase);

        auto begin = std::sregex_iterator(atributos.begin(), atributos.end(), lexic);
        auto end = std::sregex_iterator();

        std::vector<std::string> found;
        for (auto it = begin; it != end; ++it)
        {
            found.push_back(it->str());
        }

        // Se procesa cada parametro encontrado.
        for (const auto &fu : found)
        {

            size_t eqPos = fu.find('=');
            if (eqPos == std::string::npos)
            {
                return {false, "ERROR: Parametro invalido: " + fu};
            }

            std::string key = toLowerStr(fu.substr(0, eqPos));
            std::string value = fu.substr(eqPos + 1);

            if (value.size() >= 2 && value.front() == '"' && value.back() == '"')
            {
                value = value.substr(1, value.size() - 2);
            }

            if (key == "-path")
            {
                if (value.empty())
                {
                    return {false, "ERROR: El path de la particion no puede ser vacio"};
                }
                mount.Path = value;
            }
            else if (key == "-name")
            {
                if (value.empty())
                {
                    return {false, "ERROR: El nombre de la particion no puede ser vacio"};
                }
                mount.Name = value;
            }
            else
            {
                return {false, "ERROR: Parametro invalido: " + fu};
            }
        }

        // Ambos parametros son obligatorios
        if (mount.Path.empty())
        {
            return {false, "ERROR: El path de la particion no puede ser vacio"};
        }

        if (mount.Name.empty())
        {
            return {false, "ERROR: El nombre de la particion no puede ser vacio"};
        }

        // Ya con los parametros validados, se realiza el montaje real
        std::string errMsg;
        if (!MountP(mount, errMsg))
        {
            return {false, errMsg};
        }

        return {true, "MOUNT: Montaje de la particion realizado con exito"};
    }

}