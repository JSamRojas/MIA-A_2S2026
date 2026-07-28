#ifndef ANALYZER_H
#define ANALYZER_H

#include <string>
#include <vector>

namespace Analyzer
{

    // Analiza un solo comando
    // e imprime directamente por consola el resultado o el error.
    void Analyze(const std::vector<std::string> &inputs);

}

#endif