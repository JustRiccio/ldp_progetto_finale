// Salamone Alessandro
#ifndef DISPOSITIVO_H
#define DISPOSITIVO_H
#include <iostream>
#include <string>

class Dispositivo
{
private:
    std::string nome;
    int id;
    double consumo;
    double consumo_totale;

    int orario_accensione;
    bool stato;

protected:
    Dispositivo(int, std::string, double, int);

public:
    Dispositivo(const Dispositivo &) = delete;
    Dispositivo &operator=(const Dispositivo &) = delete;
    virtual ~Dispositivo();

    // getters
    std::string getNome();
    int getID();
    double getConsumo();
    double getConsumoTotale();
    int getOrarioAccensione();
    bool getStato();

    // setters
    void setNome(std::string);
    void setID(int);
    void setConsumo(double);
    void setConsumoTotale(double);
    void setOrarioAccensione(int);
    void setStato(bool);
};

#endif