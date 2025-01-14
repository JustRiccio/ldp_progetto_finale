// Salamone Alessandro
#ifndef CICLO_H
#define CICLO_H

#include <string>
#include "Dispositivo.h"

class Ciclo : public Dispositivo {

private:
    int durata;

public:
    Ciclo(int id, std::string nome, double consumo, int orario_accensione, int durata);

    int getDurata();
    void setDurata(int durata);
};

#endif