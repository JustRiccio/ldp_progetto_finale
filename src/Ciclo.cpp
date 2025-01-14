// Salamone Alessandro
#include "Ciclo.h"

// Constructor
/**
 * @brief Costruttore di un dispositivo a ciclo prefissato.
 * 
 * @param id Identificativo univoco del dispositivo.
 * @param nome Nome del dispositivo.
 * @param consumo Consumo energetico del dispositivo.
 * @param orario_accensione Orario di accensione del dispositivo.
 * @param durata Durata del ciclo del dispositivo.
 */
Ciclo::Ciclo(int id, std::string nome, double consumo,int orario_accensione, int durata)
    : Dispositivo(id, nome, consumo, orario_accensione), durata(durata) {}

/**
 * @brief Getter per durata del ciclo.
 * 
 * @return int La durata del ciclo in minuti.
 */
int Ciclo::getDurata()
{
    return durata;
}

/**
 * @brief Setter per durata del ciclo.
 * 
 * @param durata La durata del ciclo in minuti da impostare.
 */
void Ciclo::setDurata(int durata)
{
    this->durata = durata;
}