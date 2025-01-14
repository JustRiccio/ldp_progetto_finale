// Salamone Alessandro
#include "Manuale.h"

// Constructor
/**
 * @brief Costruttore di un dispositivo manuale.
 * 
 * @param id Identificativo univoco del dispositivo.
 * @param nome Nome del dispositivo.
 * @param consumo Consumo energetico del dispositivo.
 * @param orario_accensione Orario di accensione del dispositivo.
 * @param orario_spegnimento Orario di spegnimento del dispositivo.
 */
Manuale::Manuale(int id, std::string nome, double consumo, int orario_accensione, int orario_spegnimento)
    : Dispositivo(id, nome, consumo, orario_accensione), orario_spegnimento(orario_spegnimento) {}

/**
 * @brief Getter per orario di spegnimento del dispositivo
 * 
 * @return int L'orario di spegnimento del dispositivo in minuti
 */
int Manuale::getOrarioSpegnimento()
{
    return orario_spegnimento;
}

/**
 * @brief Setter per durata del ciclo.
 * 
 * @param durata L'orario di spegnimento in minuti da impostare
 */
void Manuale::setOrarioSpegnimento(int orario)
{
    orario_spegnimento = orario;
}
