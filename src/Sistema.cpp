#include "Sistema.h"

// // test per disambiguare il tipo di dispositivo (simile a instanceof di Java)
// // sarà molto importante nel momento in cui si dovrà gestire il polimorfismo
// if (dynamic_cast<Ciclo *>(dispositivi[0].get()))
// {
//     std::cout << "Il dispositivo è di tipo Ciclo" << std::endl;
// }
// else if (dynamic_cast<Manuale *>(dispositivi[0].get()))
// {
//     std::cout << "Il dispositivo è di tipo Manuale" << std::endl;
// }

// Constructor
Sistema::Sistema(std::vector<std::unique_ptr<Dispositivo>> dispositivi) : capacita_max(0), capacita_attuale(0), orario(0) {
    this->dispositivi = std::move(dispositivi);
}

void Sistema::accensioneDispositivo(std::string nome) {
    std::cout << "accensioneDispositivo: " << nome << std::endl;
}

void Sistema::spegnimentoDispositivo(std::string nome) {
    std::cout << "spegnimentoDispositivo: " << nome << std::endl;
}

void Sistema::impostaOrario(std::string nome, int orario) {
    std::cout << "impostaOrario: " << nome << " " << orario << std::endl;
}

void Sistema::impostaOrario(std::string nome, int orario_accensione, int orario_spegnimento) {
    std::cout << "impostaOrario: " << nome << " " << orario_accensione << " " << orario_spegnimento << std::endl;
}

void Sistema::rimuoviOrario(std::string nome) {
    std::cout << "rimuoviOrario: " << nome << std::endl;
}

void Sistema::stampaDispositivi() {
    std::cout << "stampaDispositivi" << std::endl;
}

void Sistema::stampaDispositivo(std::string nome) {
    std::cout << "stampaDispositivo: " << nome << std::endl;
}

void Sistema::impostaOrarioSistema(int orario) {
    std::cout << "impostaOrarioSistema: " << orario << std::endl;
}

void Sistema::resetOrarioSistema() {
    std::cout << "resetOrarioSistema" << std::endl;
}

void Sistema::resetOrariDispositivi() {
    std::cout << "resetOrariDispositivi" << std::endl;
}

void Sistema::resetSistema() {
    std::cout << "resetSistema" << std::endl;
}