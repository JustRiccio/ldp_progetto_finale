#ifndef MANUALE_H
#define MANUALE_H

#include <string>
#include "Dispositivo.h"

class Manuale : public Dispositivo
{
private:
    int orario_spegnimento;

public:
    Manuale(int id, std::string nome, double consumo, int orario_accensione, int orario_spegnimento);

    int getOrarioSpegnimento();
    void setOrarioSpegnimento(int orario);
};

#endif