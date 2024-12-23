#ifndef DISPOSITIVO_H
#define DISPOSITIVO_H 
#include <iostream>
#include <string>
#include <vector>
#include "Dispositivo.h"

class Sistema
{
	private:
        //richiesti nelle specifiche
        std::vector<Dispositivo> dispositivi;
        int capacita_max;
        int capacita_attuale; //ricavabile volendo
        int orario;       

        //andrebbe aggiunto un vettore che tiene traccia dei dispositivi accesi
	public:
	
        void accensioneDispositivo(std::string);
        //accende un dispositivo
        void spegnimentoDispositivo(std::string);
        //spegne un dispositivo
        void impostaOrario(std::string,int, int);
        //imposta l'orario di accensione(entrambi) e spegnimento (solo per dispositivo M)
        void rimuoviOrario(std::string);
        //rimuove il timer assocciato al dispositivo(da capire cosa intende la consegna)
        void stampaDispositivi();
        //stampa la lista di tutti i dispositivi con la produzione/consumo
        //dalle 00:00 all'invio del comando + produzione/consumo totale del sistema
        void stampaDispositivo(std::string); 
        //stampa produzione/consumo di un dispositivo
        void impostaOrarioSistema(int);
        //imposta l'orario del sistema
        void resetOrarioSistema();
        //imposta l'orario del sistema a 00:00 
        void resetOrariDispositivi();
        //Rimuove i timer di tutti i dispositivi
        void resetSistema();
        //resetOrarioSistema+resetOrariDispositivi+spegne tutti i dispositivi

};

#endif