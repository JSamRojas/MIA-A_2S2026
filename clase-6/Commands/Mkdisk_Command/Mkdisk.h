#ifndef MKDISK_H
#define MKDISK_H

#include <string>
#include <vector>

#include "../CommandResult.h"

namespace Commands
{

    // Valida los tokens del comando "mkdisk" (ej: {"-size=100", "-unit=M"}).
    // Solo reconoce y valida los parametros; no crea ninguna estructura ni
    // archivo. El resultado (exito o error) viene en CommandResult.message.
    CommandResult Mkdisk_Command(const std::vector<std::string> &tokens);

}

#endif