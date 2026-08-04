#ifndef COMMAND_RESULT_H
#define COMMAND_RESULT_H

#include <string>

namespace Commands
{

    // Resultado generico que devuelve la validacion de cualquier comando.
    struct CommandResult
    {
        bool success = false; // true si los parametros son validos
        std::string message;  // mensaje de exito, o de error si success == false
    };

}

#endif