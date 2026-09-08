#ifndef FDISK_H
#define FDISK_H

#include <string>
#include <vector>

#include "../CommandResult.h"

namespace Commands
{

    // Valida los tokens del comando "fdisk" (ej: {"-size=100", "-unit=M"}).
    // Solo reconoce y valida los parametros; no crea ninguna estructura ni
    // archivo. El resultado (exito o error) viene en CommandResult.message.
    CommandResult Fdisk_Command(const std::vector<std::string> &tokens);

}

#endif