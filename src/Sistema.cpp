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
Sistema::Sistema(std::vector<std::unique_ptr<Dispositivo>> dispositivi) : capacita_max(4), capacita_attuale(capacita_max), orario(0), ultimo_orario(0) {
    this->dispositivi = std::move(dispositivi);
    this->dispositiviTmp = std::vector<std::unique_ptr<Manuale>>();
}

/**
 *Accensione di un dispositivo
 *ricerca il dispositivo con il nome passato come parametro 
 *se la capacita' aggiunta non sfora il max del sistema il dispositivo viene acceso 
 */
void Sistema::accensioneDispositivo(std::string nome) {
    int tmpID = 0;
    int tmpConsumo = 0;
    int tmpOrarioAccensione = 0;
    int tmpOrarioSpegnimento = 0; 
    bool found = false;

    for(auto& dispositivo : dispositivi) {
        if(caseInsensitiveStringCompare(dispositivo->getNome(),nome)){
            //controllo se il dispositivo e' gia' acceso
            if(dispositivo->getStato()){
                std::cout << "Dispositivo " << nome << " già acceso" << std::endl;
            }else{
                //controllo che la capacita' aggiunta non sfori il max
                if(capacita_attuale + dispositivo->getConsumo() >= 0){
                    capacita_attuale += dispositivo->getConsumo();
                    dispositivo->setStato(true);

                    //da controllare se esiste gia' l'orario di accensione
                    dispositivo->setOrarioAccensione(orario);

                    //aggiungo il dispositivo al vettore temporaneo
                    if(dynamic_cast<Manuale *>(dispositivo.get())){
                        tmpID = dispositivo->getID();
                        tmpConsumo = dispositivo->getConsumo();
                        tmpOrarioAccensione = dispositivo->getOrarioAccensione();
                        tmpOrarioSpegnimento = dynamic_cast<Manuale *>(dispositivo.get())->getOrarioSpegnimento(); 
                    }else{
                        tmpID = dispositivo->getID();
                        tmpConsumo = dispositivo->getConsumo();
                        tmpOrarioAccensione = dispositivo->getOrarioAccensione();
                        tmpOrarioSpegnimento = -1;
                    }
                    dispositivi.push_back(std::make_unique<Manuale>(tmpID, nome, tmpConsumo, tmpOrarioAccensione, tmpOrarioSpegnimento));

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
                    std::cout << "Dispositivo: " << nome << " si è spento" << std::endl;
                }
                else{
                    std::cout << "Dispositivo: " << nome << " si è spento" << std::endl;
                }
            }
        }
    }
    //imposto l'orario di spegnimento del dispositivo nel vettore temporaneo
    for(auto& dispositivo : dispositiviTmp) {
        if(caseInsensitiveStringCompare(dispositivo->getNome(),nome) && dynamic_cast<Manuale *>(dispositivo.get())->getOrarioSpegnimento() == -1){
            dynamic_cast<Manuale *>(dispositivo.get())->setOrarioSpegnimento(orario);
        }
    }

    if (!found){
        std::cout << "Dispositivo " << nome << " non trovato" << std::endl;
    }
}

void Sistema::impostaOrario(std::string nome, int orario) {
    if(orario < this->orario){
        std::cout << "Orario non valido" << std::endl;
        return;
    }
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
//viene sovrascritto l'orario di accensione e di conseguenza anche tutti i dati sull'accensione precedente
void Sistema::impostaOrario(std::string nome, int orario_accensione, int orario_spegnimento) {
    if(orario_accensione < this->orario || orario_spegnimento < this->orario){
        std::cout << "Orario non valido" << std::endl;
        return;
    }
    if(orario_accensione >= orario_spegnimento){
        std::cout << "Orario non valido" << std::endl;
        return;
    }

    bool found = false;
    for(auto& dispositivo : dispositivi) {
        if(caseInsensitiveStringCompare(dispositivo->getNome(),nome)){
            if(!dispositivo->getStato()){
                dispositivo->setOrarioAccensione(orario_accensione);
            }
            if (dynamic_cast<Manuale *>(dispositivo.get())){
                dynamic_cast<Manuale *>(dispositivo.get())->setOrarioSpegnimento(orario_spegnimento);
            }  
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
            if(dispositivo->getStato()){
                std::cout << "Dispositivo " << nome << " attualmente acceso" << std::endl;
                return;
            }else{
                if (dynamic_cast<Manuale *>(dispositivo.get())){
                    dynamic_cast<Manuale *>(dispositivo.get())->setOrarioSpegnimento(-1);
                }
                dispositivo->setOrarioAccensione(-1);
                found = true;
            }
        }
    }
    if (found){
        std::cout << "rimuoviOrario: " << nome << std::endl;
    }else{
        std::cout << "Dispositivo " << nome << " non trovato" << std::endl;
    }
}

/**
 * Mostra la listadi tutti i dispositivi con la produzione/consumo energetico di ciascuno dall'ultimo orario di set al momento di invio del comando. 
 * Inoltre mostra la produzione/consumo energetica totale del sistema dalle 00:00 al momento di invio del comando
 * utilizza due for, perche' il secondo stampa solo i dispostivi che hanno/prodotto/consumato dopo l'ultimo orario di set
 */
void Sistema::stampaDispositivi() {
    std::cout << "stampaDispositivi \n" << std::endl;
    int kWProdotti = 0;
    int kWConsumati = 0;
    int consumoTmp = 0;
    
    for(auto& dispositivo : dispositivi) {
        stampaDispositivo(dispositivo->getNome());
    }

    for(auto& dispositivo : dispositiviTmp) {
        if(dispositivo->getOrarioAccensione() <= ultimo_orario){
            if(dispositivo->getOrarioSpegnimento() >= ultimo_orario){
                if(dispositivo->getOrarioSpegnimento() <= orario){
                    consumoTmp += (dispositivo->getConsumo() / 60) * (dispositivo->getOrarioSpegnimento() - ultimo_orario);
                }
                else{
                    consumoTmp += (dispositivo->getConsumo() / 60) * (orario - ultimo_orario);
                }
            }
        }
        else{
            if(dispositivo->getOrarioAccensione() <= orario){
                if(dispositivo->getOrarioSpegnimento() <= orario){
                    consumoTmp += (dispositivo->getConsumo() / 60) * (dispositivo->getOrarioSpegnimento() - dispositivo->getOrarioAccensione());
                }
                else{
                    consumoTmp += (dispositivo->getConsumo() / 60) * (orario - dispositivo->getOrarioAccensione());
                }
            }
        }

        if(consumoTmp > 0){
            kWProdotti += consumoTmp;
            consumoTmp = 0;
        }
        else{
            kWConsumati -= consumoTmp;
            consumoTmp = 0;
        }
    }
    std::cout << "Il sistema ha prodotto " << kWProdotti << "kW e consumato " << kWConsumati << "kW " << std::endl;
}

void Sistema::stampaDispositivo(std::string nome) {
    bool found = false;
    int kWProdotti = 0;
    int kWConsumati = 0;
    int consumoTmp = 0;
    
    for(auto& dispositivo : dispositiviTmp) {
        if(caseInsensitiveStringCompare(dispositivo->getNome(),nome)){
            if(orario >= dynamic_cast<Manuale *>(dispositivo.get())->getOrarioSpegnimento()){
                consumoTmp += (dispositivo->getConsumo() / 60) * (dynamic_cast<Manuale *>(dispositivo.get())->getOrarioSpegnimento() - dispositivo->getOrarioAccensione());
            }
            else{
                consumoTmp += (dispositivo->getConsumo() / 60) * (orario - dispositivo->getOrarioAccensione());
            }
            found = true;
        }
        if(consumoTmp > 0){
            kWProdotti += consumoTmp;           
            consumoTmp = 0;
        }
        else{
            kWConsumati -= consumoTmp;           
            consumoTmp = 0;
        }
    }
    //ci si aspetta che un dispotivo o produce o consuma
    if(kWConsumati > 0){
        std::cout << "Dispositivo "<< nome << " ha consumato" << -consumoTmp << " kW" << std::endl;
    }
    if(kWProdotti > 0){
        std::cout << "Dispositivo "<< nome << " ha prodotto" << consumoTmp << " kW" << std::endl;
    }
    if (!found){
        //se non e' stato trovato il dispositivo nel vettore temporaneo controllo la lista dei dispositivi
        for(auto& dispositivo : dispositivi) {
            if(caseInsensitiveStringCompare(dispositivo->getNome(),nome)){
                found = true;
            }
        }
        if (found){
            std::cout << "Dispositivo "<< nome << " ha prodotto/consumato 0 kW" << std::endl;
        }
        else{
            std::cout << "Dispositivo " << nome << " non trovato" << std::endl;
        }  
    }
}

void Sistema::impostaOrarioSistema(int orario) {
    //se l'orario e' maggiore di quello attuale
    if(this->orario < orario){
        //itera ogni minuto
        for(int min = this->orario; min < orario; min++){
            for(auto& dispositivo : dispositivi) {
                if(dynamic_cast<Manuale *>(dispositivo.get())){
                    
                    if(dynamic_cast<Manuale *>(dispositivo.get())->getOrarioSpegnimento() <= min && dispositivo->getStato()){
                        spegnimentoDispositivo(dispositivo->getNome());
                    }
                    if(dispositivo->getOrarioAccensione() <= min && !dispositivo->getStato()){
                        spegnimentoDispositivo(dispositivo->getNome());
                    }
                }
                else{
                    if(dispositivo->getOrarioAccensione() + dynamic_cast<Ciclo *>(dispositivo.get())->getDurata() <= min && dispositivo->getStato()){
                        spegnimentoDispositivo(dispositivo->getNome());
                    }
                    if(dispositivo->getOrarioAccensione() <= min && !dispositivo->getStato()){
                        spegnimentoDispositivo(dispositivo->getNome());
                    }
                }
            }
        }
    }
    this->ultimo_orario = this->orario;
    this->orario = orario;
    std::cout << "impostaOrarioSistema: " << orario << std::endl;
}

void Sistema::resetOrarioSistema() {
    this->orario = 0;
    this->ultimo_orario = 0;
    //svuoto il vettore temporaneo
    dispositiviTmp.clear();
    std::cout << "resetOrarioSistema" << std::endl;
}

// resetta tutti i timer tranne quelli accesi al momento del comando
void Sistema::resetOrariDispositivi() {
    for(auto& dispositivo : dispositivi) {
        if(!dispositivo->getStato()){    
            dispositivo->setOrarioAccensione(-1);
        }
        if (dynamic_cast<Manuale *>(dispositivo.get())){
            dynamic_cast<Manuale *>(dispositivo.get())->setOrarioSpegnimento(-1);
            for(auto& dispositivoTmp : dispositiviTmp) {
                if (dynamic_cast<Manuale *>(dispositivoTmp.get())->getOrarioSpegnimento() > orario
                    && dynamic_cast<Manuale *>(dispositivoTmp.get())->getNome() == dispositivo->getNome()){
                    dynamic_cast<Manuale *>(dispositivoTmp.get())->setOrarioSpegnimento(-1);
                }
            }
        }
    }
    std::cout << "resetOrariDispositivi" << std::endl;
}

void Sistema::resetSistema() {
    resetOrarioSistema();
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