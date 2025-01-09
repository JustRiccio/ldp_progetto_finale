#include "C:\Users\loren\OneDrive\Documenti\GitHub\ldp_progetto_finale\include\Sistema.h"

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
Sistema::Sistema(std::vector<std::unique_ptr<Dispositivo>> dispositivi) : capacita_max(4), capacita_attuale(4), orario(0) {
    this->dispositivi = std::move(dispositivi);
}

/**
 *Accensione di un dispositivo
 *ricerca il dispositivo con il nome passato come parametro 
 *se la capacita' aggiunta non sfora il max viene acceso 
 */
void Sistema::accensioneDispositivo(std::string nome) {
    bool found = false;
    for(int i = 0; i < dispositivi.size(); i++) {
        if(dispositivi[i]->getNome() == nome) {
            //controllo se il dispositivo e' gia' acceso
            if(dispositivi[i]->getStato()){
                std::cout << "Dispositivo " << nome << " già acceso" << std::endl;
            }else{
                found = true;
                //controllo che la capacita' aggiunta non sfori il max
                if(capacita_attuale + dispositivi[i]->getConsumo() <= capacita_max){
                    capacita_attuale += dispositivi[i]->getConsumo();
                    dispositivi[i]->setStato(true);
                    dispositivi[i]->setOrarioAccensione(orario);
                    std::cout << "accensioneDispositivo: " << nome << std::endl;
                }
                else{
                    std::cout << "Capacità massima raggiunta" << std::endl;
                }
            }
        }
    }
    if (!found){
        std::cout << "Dispositivo " << nome << " non trovato" << std::endl;
    }
}

void Sistema::spegnimentoDispositivo(std::string nome) {
    bool found = false;
    for(int i = 0; i < dispositivi.size(); i++) {
        //controllo se il dispositivo e' gia' spento
        if(dispositivi[i]->getNome() == nome) {
            if(!dispositivi[i]->getStato()){
                std::cout << "Dispositivo " << nome << " già spento" << std::endl;
            }else{
                //controllo se togliendo dispositivi che aggiungono kW non si vada in negativo
                if(capacita_attuale - dispositivi[i]->getConsumo() >= 0){
                    capacita_attuale -= dispositivi[i]->getConsumo();
                    dispositivi[i]->setStato(false);
                    if (dynamic_cast<Manuale *>(dispositivi[i].get())){
                        dynamic_cast<Manuale *>(dispositivi[i].get())->setOrarioSpegnimento(orario);
                    }
                    std::cout << "spegnimento Dispositivo: " << nome << std::endl;
                }
                else{
                    //se trovate qualche frase meglio da stampare
                    std::cout << "Spegnere altri dispositivi" << std::endl;
                }
            }
        }
    }
    if (!found){
        std::cout << "Dispositivo " << nome << " non trovato" << std::endl;
    }
}

void Sistema::impostaOrario(std::string nome, int orario) {
    bool found = false;
    for(int i = 0; i < dispositivi.size(); i++) {
        if(dispositivi[i]->getNome() == nome) {
            dispositivi[i]->setOrarioAccensione(orario);
            found = true;
        }
    }
    if (found){
        std::cout << "impostaOrario: " << nome << " " << orario << std::endl;
    }else{
        std::cout << "Dispositivo " << nome << " non trovato" << std::endl;
    }
}

//se un dispositivo e' gia' acceso e si imposta un orario di accensione
//viene sovrascritto l'orario di accensione e di conseguenza anche tutti i dati sull'accensi0ne precedente
void Sistema::impostaOrario(std::string nome, int orario_accensione, int orario_spegnimento) {
    bool found = false;
    for(int i = 0; i < dispositivi.size(); i++) {
        if(dispositivi[i]->getNome() == nome) {
            if (dynamic_cast<Manuale *>(dispositivi[i].get())){
                dynamic_cast<Manuale *>(dispositivi[i].get())->setOrarioSpegnimento(orario_spegnimento);
            }
            dispositivi[i]->setOrarioAccensione(orario_accensione);
            found = true;
        }
    }
    if (found){
        std::cout << "impostaOrario: " << nome << " " << orario_accensione << " " << orario_spegnimento << std::endl;
    }else{
        std::cout << "Dispositivo " << nome << " non trovato" << std::endl;
    }
}

void Sistema::rimuoviOrario(std::string nome) {
    bool found = false;
    for(int i = 0; i < dispositivi.size(); i++) {
        if(dispositivi[i]->getNome() == nome) {
            if (dynamic_cast<Manuale *>(dispositivi[i].get())){
                dynamic_cast<Manuale *>(dispositivi[i].get())->setOrarioSpegnimento(-1);
            }
            dispositivi[i]->setOrarioAccensione(-1);
            found = true;
        }
    }
    if (found){
        std::cout << "rimuoviOrario: " << nome << std::endl;
    }else{
        std::cout << "Dispositivo " << nome << " non trovato" << std::endl;
    }
}

/**
 * Mostra la listadi tutti i dispositivi con la produzione/consumo energetico di ciascuno dalle 00:00 al momentodi inviodelcomando. 
 * Inoltre mostra la produzione/consumo energetica totale del sistema dalle 00:00 al momento di invio del comando
 */
void Sistema::stampaDispositivi() {
    std::cout << "stampaDispositivi \n" << std::endl;
    int kWProdotti = 0;
    int kWConsumati = 0;
    int consumoTmp = 0;
    
    for(int i = 0; i < dispositivi.size(); i++) {
        //se dispositivo e' di tipo manuale
        if(dynamic_cast<Manuale *>(dispositivi[i].get())){
            //se il dispositivo e' acceso dal giorno prima (es. dalle 23.00 alle 8.00)
            if(dispositivi[i]->getOrarioAccensione() > dynamic_cast<Manuale *>(dispositivi[i].get())->getOrarioSpegnimento()){

                //se il dispositivo e' stato spento prima della chiamata del comando
                if(dynamic_cast<Manuale *>(dispositivi[i].get())->getOrarioSpegnimento() < orario){
                    //kW consumati dalle 00.00 allo spegnimento
                    consumoTmp += (dispositivi[i]->getConsumo() / 60) * dynamic_cast<Manuale *>(dispositivi[i].get())->getOrarioSpegnimento(); 
                }
                else{
                    //kW consumati dalle 00.00 all'orario corrente
                    consumoTmp += (dispositivi[i]->getConsumo() / 60) * orario;
                }

                //se il dispositivo e' stato acceso prima della chiamata del comando
                if(orario > dispositivi[i]->getOrarioAccensione()){
                    //kW consumati dal momento dell'accensione all'orario corrente
                    consumoTmp += (dispositivi[i]->getConsumo() / 60) * (orario - dispositivi[i]->getOrarioAccensione());
                }
            }
            //se il dispositivo e' stato acceso in giornata (es. dalle 8.15 alle 14.00)
            else{
                //se il dispositivo e' stato spento prima della chiamata del comando
                if(orario > dynamic_cast<Manuale *>(dispositivi[i].get())->getOrarioSpegnimento()){
                    //kW consumati dall'accensione allo spegnimento
                    consumoTmp += (dispositivi[i]->getConsumo() / 60) * (dynamic_cast<Manuale *>(dispositivi[i].get())->getOrarioSpegnimento() - dispositivi[i]->getOrarioAccensione());
                }
                //se il dispositivo e' stato acceso e non e' stato ancora spento prima della chiamata al comando
                else{
                    //kW consumati dall'accensione all'orario corrente
                    consumoTmp += (dispositivi[i]->getConsumo() / 60) * (orario - dispositivi[i]->getOrarioAccensione());
                }
            }
        }
        //se dispositivo e' di tipo ciclo
        else{
            int durataTmp = dynamic_cast<Ciclo *>(dispositivi[i].get())->getDurata(); 

            //se il dispositivo e' acceso dal giorno prima (es. dalle 23.00 alle 8.00)
            if(dispositivi[i]->getOrarioAccensione() + durataTmp > 1440){
                
                //se il dispositivo e' stato spento prima della chiamata del comando
                if(dispositivi[i].get()->getOrarioAccensione() + durataTmp - 1440 < orario){
                    //kW consumati dalle 00.00 allo spegnimento
                    consumoTmp += (dispositivi[i]->getConsumo() / 60) * (dispositivi[i].get()->getOrarioAccensione() + durataTmp - 1440); 
                }
                else{
                    //kW consumati dalle 00.00 all'orario corrente
                    consumoTmp += (dispositivi[i]->getConsumo() / 60) * orario;
                }

                //se il dispositivo e' stato acceso prima della chiamata del comando
                if(orario > dispositivi[i]->getOrarioAccensione()){
                    //kW consumati dal momento dell'accensione all'orario corrente
                    consumoTmp += (dispositivi[i]->getConsumo() / 60) * (orario - dispositivi[i]->getOrarioAccensione());
                }
            }
            //se il dispositivo e' stato acceso in giornata (es. dalle 8.15 alle 14.00)
            else{
                //se il dispositivo e' stato spento prima della chiamata del comando
                if(orario > dispositivi[i]->getOrarioAccensione() + durataTmp){
                    //kW consumati dall'accensione allo spegnimento
                    consumoTmp += (dispositivi[i]->getConsumo() / 60) * durataTmp;
                }
                //se il dispositivo e' stato acceso e non e' stato ancora spento prima della chiamata al comando
                else{
                    //kW consumati dall'accensione all'orario corrente
                    consumoTmp += (dispositivi[i]->getConsumo() / 60) * (orario - dispositivi[i]->getOrarioAccensione());
                }
            }
        }

        if(consumoTmp > 0){
            kWProdotti += consumoTmp;
            std::cout << "Dispositivo "<< dispositivi[i]->getNome() << " ha prodotto" << consumoTmp << " kW \n" << std::endl;
            consumoTmp = 0;
        }
        else{
            kWConsumati -= consumoTmp;
            std::cout << "Dispositivo "<< dispositivi[i]->getNome() << " ha consumato" << -consumoTmp << " kW \n" << std::endl;
            consumoTmp = 0;
        }
    }
    std::cout << "Il sistema ha prodotto" << kWProdotti << "kW e consumato " << kWConsumati << "kW " << std::endl;
}

void Sistema::stampaDispositivo(std::string nome) {
    bool found = false;
    for(int i = 0; i < dispositivi.size(); i++) {
        if(dispositivi[i]->getNome() == nome) {
            if(dispositivi[i]->getConsumo()>0){
                std::cout << "Dispositivo " << nome << " produce " << dispositivi[i]->getConsumo() << " kW" << std::endl;
            }else{
                std::cout << "Dispositivo " << nome << " consuma " << dispositivi[i]->getConsumo() << " kW" << std::endl;
            }
            found = true;
        }
    }
    if (!found){
        std::cout << "Dispositivo " << nome << " non trovato" << std::endl;
    }
    //std::cout << "stampaDispositivo: " << nome << std::endl;
}

void Sistema::impostaOrarioSistema(int orario) {
    this->orario = orario;
    std::cout << "impostaOrarioSistema: " << orario << std::endl;
    //da aggiungere una funzione che sistemi i dispositivi in base all'orario
    //tipo void gestioneCambioOrario();
}

void Sistema::resetOrarioSistema() {
    this->orario = 0;
    std::cout << "resetOrarioSistema" << std::endl;
}

// resetta tutti i timer tranne quelli accesi al momento del comando
//manuale togli inizio e fine
//ciclico togli l'inizio
void Sistema::resetOrariDispositivi() {
    for(int i = 0; i < dispositivi.size(); i++) {
        if(!dispositivi[i]->getStato()){    
            if (dynamic_cast<Manuale *>(dispositivi[i].get())){
                //::::::::::::::::::::::::::::::::
                dynamic_cast<Manuale *>(dispositivi[i].get())->setOrarioSpegnimento(-1);
                //::::::::::::::::::::::::::::::::
            }
            dispositivi[i]->setOrarioAccensione(-1);
        }
    }
    std::cout << "resetOrariDispositivi" << std::endl;
}

void Sistema::resetSistema() {
    for(int i = 0; i < dispositivi.size(); i++) {
        dispositivi[i]->setStato(false);
        if (dynamic_cast<Manuale *>(dispositivi[i].get())){
            dynamic_cast<Manuale *>(dispositivi[i].get())->setOrarioSpegnimento(-1);
        }
        dispositivi[i]->setOrarioAccensione(-1);
    }
    std::cout << "resetSistema" << std::endl;
}