#include "Manuale.h"

// Constructor
Manuale::Manuale(int id, std::string nome, double consumo, int orario_accensione, int orario_spegnimento)
    : Dispositivo(id, nome, consumo, orario_accensione), orario_spegnimento(orario_spegnimento) {}

// Getter for orario_spegnimento
int Manuale::getOrarioSpegnimento()
{
    return orario_spegnimento;
}

// Setter for orario_spegnimento
void Manuale::setOrarioSpegnimento(int orario)
{
    orario_spegnimento = orario;
}
