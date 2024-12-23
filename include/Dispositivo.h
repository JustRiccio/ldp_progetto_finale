#ifndef SISTEMA_H
#define SISTEMA_H 
#include <iostream>
#include <string>

class Dispositivo
{
	private:
        //richiesti nelle specifiche
        std::string nome;
        int ID;
        double consumo;
        int durata;     //pensavo di tenere tutto in minuti

        //per comodita' (si potrebbe gestire tutto su durata)
        bool manual;    //da scegliere come gestire
		int CP;
        int orario_accensione;
        int orario_spegnimento;

	public:
		Dispositivo(int, std::string, double, int);

        //getters
        std::string getNome();
        int getID();
        double getConsumo();
        int getDurata();
        bool getManual();
        int getCP();
        int getOrarioAccensione();
        int getOrarioSpegnimento();

        //setters
        void setNome(std::string);
        void setID(int);
        void setConsumo(double);
        void setDurata(int);
        void setManual(bool);
        void setCP(int);
        void setOrarioAccensione(int);
        void setOrarioSpegnimento(int);

};

#endif