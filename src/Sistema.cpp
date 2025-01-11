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
    for(auto& dispositivo : dispositivi) {
        if(caseInsensitiveStringCompare(dispositivo->getNome(),nome)){
            //controllo se il dispositivo e' gia' acceso
            if(dispositivo->getStato()){
                std::cout << "Dispositivo " << nome << " già acceso" << std::endl;
            }else{
                //controllo che la capacita' aggiunta non sfori il max
                if(capacita_attuale + dispositivo->getConsumo() <= capacita_max){
                    capacita_attuale += dispositivo->getConsumo();
                    dispositivo->setStato(true);
                    dispositivo->setOrarioAccensione(orario);
                    std::cout << "accensioneDispositivo: " << dispositivo->getNome() << std::endl;
                }
                else{
                    std::cout << "Capacità massima raggiunta" << std::endl;
                }
            }
            found = true;
        }
    }
    if (!found){
        std::cout << "Dispositivo " << nome << " non trovato" << std::endl;
    }
}

void Sistema::spegnimentoDispositivo(std::string nome) {
    bool found = false;
    for(auto& dispositivo : dispositivi) {
        //controllo se il dispositivo e' gia' spento
        if(caseInsensitiveStringCompare(dispositivo->getNome(),nome)){
            if(!dispositivo->getStato()){
                std::cout << "Dispositivo " << nome << " già spento" << std::endl;
            }else{
                //controllo se togliendo dispositivi che aggiungono kW non si vada in negativo
                if(capacita_attuale - dispositivo->getConsumo() >= 0){
                    capacita_attuale -= dispositivo->getConsumo();
                    dispositivo->setStato(false);
                    if (dynamic_cast<Manuale *>(dispositivo.get())){
                        dynamic_cast<Manuale *>(dispositivo.get())->setOrarioSpegnimento(orario);
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
    for(auto& dispositivo : dispositivi) {
        if(caseInsensitiveStringCompare(dispositivo->getNome(),nome)){
            dispositivo->setOrarioAccensione(orario);
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
    for(auto& dispositivo : dispositivi) {
        if(caseInsensitiveStringCompare(dispositivo->getNome(),nome)){
            if (dynamic_cast<Manuale *>(dispositivo.get())){
                dynamic_cast<Manuale *>(dispositivo.get())->setOrarioSpegnimento(orario_spegnimento);
            }
            dispositivo->setOrarioAccensione(orario_accensione);
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
    for(auto& dispositivo : dispositivi) {
        if(caseInsensitiveStringCompare(dispositivo->getNome(),nome)){
            if (dynamic_cast<Manuale *>(dispositivo.get())){
                dynamic_cast<Manuale *>(dispositivo.get())->setOrarioSpegnimento(-1);
            }
            dispositivo->setOrarioAccensione(-1);
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
    
    for(auto& dispositivo : dispositivi) {
        //se dispositivo e' di tipo manuale
        if(dynamic_cast<Manuale *>(dispositivo.get())){
            //se il dispositivo e' acceso dal giorno prima (es. dalle 23.00 alle 8.00)
            if(dispositivo->getOrarioAccensione() > dynamic_cast<Manuale *>(dispositivo.get())->getOrarioSpegnimento()){

                //se il dispositivo e' stato spento prima della chiamata del comando
                if(dynamic_cast<Manuale *>(dispositivo.get())->getOrarioSpegnimento() < orario){
                    //kW consumati dalle 00.00 allo spegnimento
                    consumoTmp += (dispositivo->getConsumo() / 60) * dynamic_cast<Manuale *>(dispositivo.get())->getOrarioSpegnimento(); 
                }
                else{
                    //kW consumati dalle 00.00 all'orario corrente
                    consumoTmp += (dispositivo->getConsumo() / 60) * orario;
                }

                //se il dispositivo e' stato acceso prima della chiamata del comando
                if(orario > dispositivo->getOrarioAccensione()){
                    //kW consumati dal momento dell'accensione all'orario corrente
                    consumoTmp += (dispositivo->getConsumo() / 60) * (orario - dispositivo->getOrarioAccensione());
                }
            }
            //se il dispositivo e' stato acceso in giornata (es. dalle 8.15 alle 14.00)
            else{
                //se il dispositivo e' stato spento prima della chiamata del comando
                if(orario > dynamic_cast<Manuale *>(dispositivo.get())->getOrarioSpegnimento()){
                    //kW consumati dall'accensione allo spegnimento
                    consumoTmp += (dispositivo->getConsumo() / 60) * (dynamic_cast<Manuale *>(dispositivo.get())->getOrarioSpegnimento() - dispositivo->getOrarioAccensione());
                }
                //se il dispositivo e' stato acceso e non e' stato ancora spento prima della chiamata al comando
                else{
                    //kW consumati dall'accensione all'orario corrente
                    consumoTmp += (dispositivo->getConsumo() / 60) * (orario - dispositivo->getOrarioAccensione());
                }
            }
        }
        //se dispositivo e' di tipo ciclo
        else{
            int durataTmp = dynamic_cast<Ciclo *>(dispositivo.get())->getDurata(); 

            //se il dispositivo e' acceso dal giorno prima (es. dalle 23.00 alle 8.00)
            if(dispositivo->getOrarioAccensione() + durataTmp > 1440){
                
                //se il dispositivo e' stato spento prima della chiamata del comando
                if(dispositivo.get()->getOrarioAccensione() + durataTmp - 1440 < orario){
                    //kW consumati dalle 00.00 allo spegnimento
                    consumoTmp += (dispositivo->getConsumo() / 60) * (dispositivo.get()->getOrarioAccensione() + durataTmp - 1440); 
                }
                else{
                    //kW consumati dalle 00.00 all'orario corrente
                    consumoTmp += (dispositivo->getConsumo() / 60) * orario;
                }

                //se il dispositivo e' stato acceso prima della chiamata del comando
                if(orario > dispositivo->getOrarioAccensione()){
                    //kW consumati dal momento dell'accensione all'orario corrente
                    consumoTmp += (dispositivo->getConsumo() / 60) * (orario - dispositivo->getOrarioAccensione());
                }
            }
            //se il dispositivo e' stato acceso in giornata (es. dalle 8.15 alle 14.00)
            else{
                //se il dispositivo e' stato spento prima della chiamata del comando
                if(orario > dispositivo->getOrarioAccensione() + durataTmp){
                    //kW consumati dall'accensione allo spegnimento
                    consumoTmp += (dispositivo->getConsumo() / 60) * durataTmp;
                }
                //se il dispositivo e' stato acceso e non e' stato ancora spento prima della chiamata al comando
                else{
                    //kW consumati dall'accensione all'orario corrente
                    consumoTmp += (dispositivo->getConsumo() / 60) * (orario - dispositivo->getOrarioAccensione());
                }
            }
        }

        if(consumoTmp > 0){
            kWProdotti += consumoTmp;
            std::cout << "Dispositivo "<< dispositivo->getNome() << " ha prodotto" << consumoTmp << " kW \n" << std::endl;
            consumoTmp = 0;
        }
        else{
            kWConsumati -= consumoTmp;
            std::cout << "Dispositivo "<< dispositivo->getNome() << " ha consumato" << -consumoTmp << " kW \n" << std::endl;
            consumoTmp = 0;
        }
    }
    std::cout << "Il sistema ha prodotto" << kWProdotti << "kW e consumato " << kWConsumati << "kW " << std::endl;
}

void Sistema::stampaDispositivo(std::string nome) {
    bool found = false;
    for(auto& dispositivo : dispositivi) {
        if(caseInsensitiveStringCompare(dispositivo->getNome(),nome)){
            if(dispositivo->getConsumo()>0){
                std::cout << "Dispositivo " << nome << " produce " << dispositivo->getConsumo() << " kW" << std::endl;
            }else{
                std::cout << "Dispositivo " << nome << " consuma " << dispositivo->getConsumo() << " kW" << std::endl;
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
    for(auto& dispositivo : dispositivi) {
        if(!dispositivo->getStato()){    
            if (dynamic_cast<Manuale *>(dispositivo.get())){
                //::::::::::::::::::::::::::::::::
                dynamic_cast<Manuale *>(dispositivo.get())->setOrarioSpegnimento(-1);
                //::::::::::::::::::::::::::::::::
            }
            dispositivo->setOrarioAccensione(-1);
        }
    }
    std::cout << "resetOrariDispositivi" << std::endl;
}

void Sistema::resetSistema() {
    for(auto& dispositivo : dispositivi) {
        dispositivo->setStato(false);
        if (dynamic_cast<Manuale *>(dispositivo.get())){
            dynamic_cast<Manuale *>(dispositivo.get())->setOrarioSpegnimento(-1);
        }
        dispositivo->setOrarioAccensione(-1);
    }
    std::cout << "resetSistema" << std::endl;
}

//funzione per comparare stringhe case insensitive
bool Sistema::caseInsensitiveStringCompare(const std::string& str1, const std::string& str2) {
    if (str1.size() != str2.size()) {
        return false;
    }
    for (std::string::const_iterator c1 = str1.begin(), c2 = str2.begin(); c1 != str1.end(); ++c1, ++c2) {
        if (tolower(static_cast<unsigned char>(*c1)) != tolower(static_cast<unsigned char>(*c2))) {
            return false;
        }
    }
    return true;
}