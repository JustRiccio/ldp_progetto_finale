// Salamone Alessandro
#include "Dispositivo.h"

/**
 * @brief Costruttore della classe Dispositivo
 *
 * Inizializza un nuovo dispositivo con i parametri forniti
 *
 * @param id Identificativo del dispositivo
 * @param nome Nome del dispositivo
 * @param consumo Consumo istantaneo del dispositivo
 * @param stato Stato del dispositivo (acceso/spento)
 */
Dispositivo::Dispositivo(int id, std::string nome, double consumo, int stato)
    : id(id), nome(nome), consumo(consumo), stato(stato) {}

// Destructor
Dispositivo::~Dispositivo() {}

/**
 * @brief Getter per il nome del dispositivo
 *
 * @return std::string Nome del dispositivo
 */
std::string Dispositivo::getNome()
{
    return nome;
}

/**
 * @brief Getter per l'ID del dispositivo
 *
 * @return int Identificativo del dispositivo
 */
int Dispositivo::getID()
{
    return id;
}

/**
 * @brief Getter per il consumo istantaneo del dispositivo
 *
 * @return double Consumo istantaneo del dispositivo
 */
double Dispositivo::getConsumo()
{
    return consumo;
}

/**
 * @brief Getter per il consumo totale del dispositivo
 *
 * @return double Consumo totale del dispositivo
 */
double Dispositivo::getConsumoTotale()
{
    return consumo_totale;
}

/**
 * @brief Getter per l'orario di accensione del dispositivo
 *
 * @return int Orario di accensione del dispositivo
 */
int Dispositivo::getOrarioAccensione()
{
    return orario_accensione;
}

/**
 * @brief Getter per lo stato del dispositivo
 *
 * @return bool Stato del dispositivo (true=acceso, false=spento)
 */
bool Dispositivo::getStato()
{
    return stato;
}

/**
 * @brief Setter per il nome del dispositivo
 *
 * @param nome Nuovo nome del dispositivo
 */
void Dispositivo::setNome(std::string nome)
{
    this->nome = nome;
}

/**
 * @brief Setter per l'ID del dispositivo
 *
 * @param id Nuovo identificativo del dispositivo
 */
void Dispositivo::setID(int id)
{
    this->id = id;
}

/**
 * @brief Setter per il consumo istantaneo del dispositivo
 *
 * @param consumo Nuovo valore del consumo istantaneo
 */
void Dispositivo::setConsumo(double consumo)
{
    this->consumo = consumo;
}

/**
 * @brief Setter per il consumo totale del dispositivo
 *
 * @param consumo Nuovo valore del consumo totale
 */
void Dispositivo::setConsumoTotale(double consumo)
{
    this->consumo_totale = consumo;
}

/**
 * @brief Setter per l'orario di accensione del dispositivo
 *
 * @param orario_accensione Nuovo orario di accensione
 */
void Dispositivo::setOrarioAccensione(int orario_accensione)
{
    this->orario_accensione = orario_accensione;
}

/**
 * @brief Setter per lo stato del dispositivo
 * 
 * @param stato Nuovo stato del dispositivo (stato=true -> acceso)
 */
void Dispositivo::setStato(bool stato)
{
    this->stato = stato;
}
