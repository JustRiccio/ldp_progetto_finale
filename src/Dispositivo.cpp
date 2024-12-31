#include "Dispositivo.h"

// Constructor
Dispositivo::Dispositivo(int id, std::string nome, double consumo, int orario_accensione)
    : id(id), nome(nome), consumo(consumo), orario_accensione(orario_accensione), stato(false) {}

// Destructor
Dispositivo::~Dispositivo() {}

// Getters
std::string Dispositivo::getNome()
{
    return nome;
}

int Dispositivo::getID()
{
    return id;
}

double Dispositivo::getConsumo()
{
    return consumo;
}

int Dispositivo::getOrarioAccensione()
{
    return orario_accensione;
}

bool Dispositivo::getStato()
{
    return stato;
}

// Setters
void Dispositivo::setNome(std::string nome)
{
    this->nome = nome;
}

void Dispositivo::setID(int id)
{
    this->id = id;
}

void Dispositivo::setConsumo(double consumo)
{
    this->consumo = consumo;
}

void Dispositivo::setOrarioAccensione(int orario_accensione)
{
    this->orario_accensione = orario_accensione;
}

void Dispositivo::setStato(bool stato)
{
    this->stato = stato;
}
