// Salamone Alessandro
#ifndef SISTEMA_H
#define SISTEMA_H
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include "Dispositivo.h"
#include "Ciclo.h"
#include "Manuale.h"
class Sistema
{
private:
        // il vettore deve essere di puntatori perchè non può contenere reference a oggetti di tipo differenti
        // top answer + risposta di Thomas Sablik del seguente link:
        // https://stackoverflow.com/questions/2394581/pure-virtual-class-and-collections-vector
        std::vector<std::unique_ptr<Dispositivo>> dispositivi;
        const double capacita_max;
        double capacita_attuale; // ricavabile volendo
        int orario;

public:
        static constexpr double POTENZA_MASSIMA_DEFAULT = 3.5;
        Sistema(std::vector<std::unique_ptr<Dispositivo>>, double potenza_massima = POTENZA_MASSIMA_DEFAULT); // aggiunto parametro potenza_massima
        // Constructor
        void accensioneDispositivo(std::string);
        // accende un dispositivo
        void spegnimentoDispositivo(std::string);
        // spegne un dispositivo
        void impostaOrario(std::string, int);
        // imposta l'orario di accensione per il dispositivo
        void impostaOrario(std::string, int, int);
        // imposta l'orario di accensione e spegnimento (solo per dispositivo M)
        void rimuoviOrario(std::string);
        // rimuove il timer assocciato al dispositivo(da capire cosa intende la consegna)
        void stampaDispositivi();
        // stampa la lista di tutti i dispositivi con la produzione/consumo
        // dalle 00:00 all'invio del comando + produzione/consumo totale del sistema
        void stampaDispositivo(std::string, bool);
        // stampa produzione/consumo di un dispositivo
        void impostaOrarioSistema(int);
        // imposta l'orario del sistema
        void resetOrarioSistema();
        // imposta l'orario del sistema a 00:00
        void resetOrariDispositivi();
        // Rimuove i timer di tutti i dispositivi
        void resetSistema();
        // resetOrarioSistema+resetOrariDispositivi+spegne tutti i dispositivi
        bool caseInsensitiveStringCompare(const std::string &, const std::string &);
        // funzione per comparare stringhe case insensitive
        std::string stampaOrario(int);
        // stampa l'orario del sistema
        void spegnimentoDispositivoHelper(Dispositivo&);
};

#endif