#include "Ciclo.h"

// Constructor
Ciclo::Ciclo(int id, std::string nome, double consumo,int orario_accensione, int durata)
    : Dispositivo(id, nome, consumo, orario_accensione), durata(durata) {}

// Getter for durata
int Ciclo::getDurata()
{
    return durata;
}

// Setter for durata
void Ciclo::setDurata(int durata)
{
    this->durata = durata;
}