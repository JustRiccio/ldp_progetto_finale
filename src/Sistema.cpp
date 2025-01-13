#include <iomanip>
#include <sstream>
#include "Sistema.h"

// Constructor
Sistema::Sistema(std::vector<std::unique_ptr<Dispositivo>> dispositivi) : capacita_max(4), capacita_attuale(0), orario(0)
{
    this->dispositivi = std::move(dispositivi);
}

/**
 *Accensione di un dispositivo
 *ricerca il dispositivo con il nome passato come parametro
 *se la capacita' aggiunta non sfora il max del sistema il dispositivo viene acceso
 */
void Sistema::accensioneDispositivo(std::string nome)
{
    bool found = false;

    for (auto &dispositivo : dispositivi)
    {
        if (caseInsensitiveStringCompare(dispositivo->getNome(), nome))
        {
            // Controllo se il dispositivo è gia' acceso
            if (dispositivo->getStato())
            {
                std::cout << stampaOrario(this->orario) << "Dispositivo " << nome << " gia' acceso" << std::endl;
            }
            else
            {
                // TODO: accendi comunque e in caso spegni i dispositivi se superano la potenza max
                // eventualmente aggiungendo regole specifiche (esempi nel pdf)

                // Controllo che la capacita' aggiunta non sfori il max
                capacita_attuale += dispositivo->getConsumo();
                dispositivo->setStato(true);

                // Controllo se l'orario di accensione non è stato settato
                if(dispositivo->getOrarioAccensione() == -1) {
                    dispositivo->setOrarioAccensione(orario);
                }
                std::cout << stampaOrario(this->orario) << "Il dispositivo '" << dispositivo->getNome() << "' si è acceso" << std::endl;
            }
            found = true;
            break;
        }
    }
    if (!found)
    {
        std::cout << stampaOrario(this->orario) << "Il dispositivo '" << nome << "' non è stato trovato" << std::endl;
    }
}

void Sistema::spegnimentoDispositivo(std::string nome)
{
    bool found = false;
    for (auto &dispositivo : dispositivi)
    {
        if (caseInsensitiveStringCompare(dispositivo->getNome(), nome))
        {
            if (!dispositivo->getStato())
            {
                std::cout << stampaOrario(this->orario) << "Il dispositivo '" << nome << "' è gia' spento" << std::endl;
            }
            else
            {
                // TODO: gestione del caso "spegnimento fotovoltaico" che potrebbe
                //  causare problemi se la potenza senza fotovoltaico e' troppo alta
                //  per il sistema (bisogna spegnere il fotovoltaico, e poi spegnere dispositivi
                //  secondo le logiche scelte (ricordarsi di aggiornare consumi di questi dispositivi))

                capacita_attuale -= dispositivo->getConsumo();
                dispositivo->setStato(false);
                if (dynamic_cast<Manuale *>(dispositivo.get()))
                {
                    dynamic_cast<Manuale *>(dispositivo.get())->setOrarioSpegnimento(orario);
                }
                std::cout << stampaOrario(this->orario) << "Il dispositivo '" << nome << "' si e' spento" << std::endl;
                dispositivo->setConsumoTotale(dispositivo->getConsumoTotale() + (orario - dispositivo->getOrarioAccensione()) * dispositivo->getConsumo() / 60);
                dispositivo->setOrarioAccensione(-1);
            }
            found = true;
            break;
        }
    }

    if (!found)
    {
        std::cout << stampaOrario(this->orario) << "Il dispositivo '" << nome << "' non è stato trovato" << std::endl;
    }
}

void Sistema::impostaOrario(std::string nome, int orario)
{
    if (orario <= this->orario)
    {
        std::cout << stampaOrario(this->orario) << "Orario inserito non valido" << std::endl;
        return;
    }
    bool found = false;
    for (auto &dispositivo : dispositivi)
    {
        if (caseInsensitiveStringCompare(dispositivo->getNome(), nome))
        {
            if (dispositivo->getOrarioAccensione() != -1)
            {
                std::cout << stampaOrario(this->orario) << "Il dispositivo '" << nome << "' ha gia' un timer impostato" << std::endl;
            }
            else
            {
                dispositivo->setOrarioAccensione(orario);
                std::cout << stampaOrario(this->orario) << "Impostato l'orario di accensione del dispositivo '" << nome << "' alle ore " << stampaOrario(orario) << std::endl;
            }
            found = true;
            break;
        }
    }
    if (!found)
    {
        std::cout << stampaOrario(this->orario) << "il dispositivo '" << nome << "' non è stato trovato" << std::endl;
    }
}

// se un dispositivo e' gia' acceso e si imposta un orario di accensione
// viene sovrascritto l'orario di accensione e di conseguenza anche tutti i dati sull'accensione precedente
void Sistema::impostaOrario(std::string nome, int orario_accensione, int orario_spegnimento)
{
    if (orario_accensione <= this->orario || orario_spegnimento <= this->orario)
    {
        std::cout << stampaOrario(this->orario) << "L'orario non è valido" << std::endl;
        return;
    }
    if (orario_accensione >= orario_spegnimento)
    {
        std::cout << stampaOrario(this->orario) << "L'orario non è valido" << std::endl;
        return;
    }

    bool found = false;
    for (auto &dispositivo : dispositivi)
    {
        if (caseInsensitiveStringCompare(dispositivo->getNome(), nome))
        {
            if (dispositivo->getOrarioAccensione() != -1)
            {
                std::cout << stampaOrario(this->orario) << "Il dispositivo '" << nome << "' ha gia' un timer impostato" << std::endl;
            }
            else
            {

                if (!dispositivo->getStato())
                {
                    dispositivo->setOrarioAccensione(orario_accensione);
                }
                // Stampa differenziata per i due tipi di dispositivi
                // Settaggio dell'orario di spegnimento per manuali
                if (dynamic_cast<Manuale *>(dispositivo.get()))
                {
                    dynamic_cast<Manuale *>(dispositivo.get())->setOrarioSpegnimento(orario_spegnimento);
                    std::cout << stampaOrario(this->orario) << "Impostato un timer per il dispositivo '" << nome << "' dalle " << stampaOrario(orario_accensione) << " alle " << stampaOrario(orario_spegnimento) << std::endl;
                }
                else if (dynamic_cast<Ciclo *>(dispositivo.get()))
                {
                    std::cout << stampaOrario(this->orario) << "Impostato un timer per il dispositivo '" << nome << "' alle " << stampaOrario(orario_accensione) << std::endl;
                }
            }
            found = true;
            break;
        }
    }
    if (!found)
    {
        std::cout << stampaOrario(this->orario) << "Il dispositivo '" << nome << "' non è stato trovato" << std::endl;
    }
}

void Sistema::rimuoviOrario(std::string nome)
{
    bool found = false;
    for (auto &dispositivo : dispositivi)
    {
        if (caseInsensitiveStringCompare(dispositivo->getNome(), nome))
        {
            if (dispositivo->getStato())
            {
                std::cout << stampaOrario(this->orario) << "Il dispositivo '" << nome << "' è attualmente acceso" << std::endl;
                return;
            }
            else
            {
                if (dynamic_cast<Manuale *>(dispositivo.get()))
                {
                    dynamic_cast<Manuale *>(dispositivo.get())->setOrarioSpegnimento(-1);
                }
                dispositivo->setOrarioAccensione(-1);
                found = true;
                break;
            }
        }
    }
    std::cout << stampaOrario(this->orario);
    if (found)
    {
        std::cout << "Rimosso il timer dal dispositivo '" << nome << "'" <<  std::endl;
    }
    else
    {
        std::cout << "Il dispositivo '" << nome << "' non è stato trovato" << std::endl;
    }
}

/**
 * Mostra la lista di tutti i dispositivi con la produzione/consumo energetico di ciascuno dall'ultimo orario di set al momento di invio del comando.
 * Inoltre mostra la produzione/consumo energetica totale del sistema dalle 00:00 al momento di invio del comando
 * utilizza due for, perche' il secondo stampa solo i dispostivi che hanno/prodotto/consumato dopo l'ultimo orario di set
 */
void Sistema::stampaDispositivi()
{
    //TODO: per ora non viene tenuto conto dei consumi dei dispositivi ancora accesi.
    // vale a dire che i consumi, sia dei singoli dispositivi, che di conseguenza totali, vengono aggiornati
    // solo quando un dispostivo viene spento, mentre qui magari sarebbe
    // da stampare anche quello che un dispositivo ancora acceso sta consumando
    double kWProdotti = 0;
    double kWConsumati = 0;
    double consumoTmp = 0;

    for (auto &dispositivo : dispositivi)
    {
        consumoTmp = dispositivo->getConsumoTotale();

        if (consumoTmp > 0)
        {
            kWProdotti += consumoTmp;
        }
        else
        {
            kWConsumati -= consumoTmp;
        }
    }
    std::cout << stampaOrario(this->orario) << "Attualmente il sistema ha prodotto " << kWProdotti << "kWh e consumato " << kWConsumati << "kWh " << std::endl;
    std::cout << "\tNello specifico: " << std::endl;
    for (auto &dispositivo : dispositivi)
    {
        std::cout << "\t- ";
        stampaDispositivo(dispositivo->getNome(), false);
    }
}

void Sistema::stampaDispositivo(std::string nome, bool stampaOrarioFlag = true)
{
    //TODO: per ora non viene tenuto conto dei consumi dei dispositivi ancora accesi.
    // vale a dire che i consumi, sia dei singoli dispositivi, che di conseguenza totali, vengono aggiornati
    // solo quando un dispostivo viene spento, mentre qui magari sarebbe
    // da stampare anche quello che un dispositivo ancora acceso sta consumando
    bool found = false;
    double kWProdotti = 0;
    double kWConsumati = 0;
    double consumoTmp = 0;

    for (auto &dispositivo : dispositivi)
    {
        if (caseInsensitiveStringCompare(dispositivo->getNome(), nome))
        {
            consumoTmp = dispositivo->getConsumoTotale();
            if (consumoTmp > 0)
            {
                kWProdotti += consumoTmp;
            }
            else
            {
                kWConsumati -= consumoTmp;
            }
            found = true;
            break;
        }
    }
    if (!found)
    {
        std::cout << "Il dispositivo '" << nome << "' non è stato trovato" << std::endl;
    }
    else
    {
        if (stampaOrarioFlag)
        {
            std::cout << stampaOrario(this->orario);
        }
        // ci si aspetta che un dispositivo o produce o consuma
        if (kWConsumati >= 0)
        {
            std::cout << "Il dispositivo '" << nome << "' ha attualmente consumato " << consumoTmp << "kWh" << std::endl;
        }
        if (kWProdotti > 0)
        {
            std::cout << "Il dispositivo '" << nome << "' ha attualmente prodotto " << consumoTmp << "kWh" << std::endl;
        }
    }
}

void Sistema::impostaOrarioSistema(int orario)
{
    std::cout << stampaOrario(this->orario) << "L'orario attuale è " << stampaOrario(this->orario).substr(1, 5);
    std::cout << std::endl;

    // se l'orario e' maggiore di quello attuale
    while (this->orario < orario)
    {
        // Ogni iterazione simula un minuto
        this->orario += 1;
        for (auto &dispositivo : dispositivi)
        {
            if (dynamic_cast<Manuale *>(dispositivo.get()))
            {
                if (dispositivo->getStato() && dynamic_cast<Manuale *>(dispositivo.get())->getOrarioSpegnimento() == this->orario)
                {
                    spegnimentoDispositivo(dispositivo->getNome());
                }
                if (!dispositivo->getStato() && dispositivo->getOrarioAccensione() == this->orario)
                {
                    accensioneDispositivo(dispositivo->getNome());
                }
            }
            else
            {
                if (dispositivo->getStato() && dispositivo->getOrarioAccensione() + dynamic_cast<Ciclo *>(dispositivo.get())->getDurata() == this->orario)
                {
                    spegnimentoDispositivo(dispositivo->getNome());
                }
                if (!dispositivo->getStato() && dispositivo->getOrarioAccensione() == this->orario)
                {
                    accensioneDispositivo(dispositivo->getNome());
                }
            }
        }
    }
    std::cout << stampaOrario(this->orario) << "L'orario attuale è " << stampaOrario(this->orario).substr(1, 5) << std::endl;
}

// TODO: controllare che faccia quello richiesto dalle specifiche che l'ho scritto di fretta
void Sistema::resetOrarioSistema()
{
    std::cout << stampaOrario(this->orario) << "L'orario del sistema è stato resettato" << std::endl;
    this->orario = 0;
    for (auto &dispositivo : dispositivi)
    {
        dispositivo->setStato(false);
        dispositivo->setConsumoTotale(0);
    }
}

// resetta tutti i timer tranne quelli accesi al momento del comando
// TODO: controllare che faccia quello richiesto dalle specifiche che l'ho scritto di fretta
void Sistema::resetOrariDispositivi()
{
    std::cout << stampaOrario(this->orario) << "I timer dei dispositivi sono stati resettati" << std::endl;

    for (auto &dispositivo : dispositivi)
    {
        // TODO: gestione dispositivo ciclico gia' acceso
        if (!dispositivo->getStato())
        {
            dispositivo->setOrarioAccensione(-1);
        }
        if (dynamic_cast<Manuale *>(dispositivo.get()))
        {
            dynamic_cast<Manuale *>(dispositivo.get())->setOrarioSpegnimento(-1);
        }
    }
}

// Riporta il sistema alle condizioni iniziali
// L’orario viene impostato a 00:00, tutti i timer vengono rimossi e tutti i dispositivi vengono spenti
void Sistema::resetSistema()
{
    std::cout << stampaOrario(this->orario) << "Il sistema è stato portato alle condizioni iniziali" << std::endl;
    resetOrarioSistema();

    for (auto &dispositivo : dispositivi)
    {
        dispositivo->setStato(false);
        if (dynamic_cast<Manuale *>(dispositivo.get()))
        {
            dynamic_cast<Manuale *>(dispositivo.get())->setOrarioSpegnimento(-1);
        }
        dispositivo->setOrarioAccensione(-1);
        dispositivo->setConsumoTotale(0);
    }    
}

// Comparazione case insensitive tra due stringhe
bool Sistema::caseInsensitiveStringCompare(const std::string &str1, const std::string &str2)
{
    if (str1.size() != str2.size())
    {
        return false;
    }
    for (std::string::const_iterator c1 = str1.begin(), c2 = str2.begin(); c1 != str1.end(); ++c1, ++c2)
    {
        if (tolower(static_cast<unsigned char>(*c1)) != tolower(static_cast<unsigned char>(*c2)))
        {
            return false;
        }
    }
    return true;
}

// Stampa orario in formato hh:mm
std::string Sistema::stampaOrario(int orario) {
    int ora = orario / 60;
    int minuti = orario % 60;
    std::ostringstream oss;
    oss << "[" << std::setfill('0') << std::setw(2) << ora << ":" << std::setfill('0') << std::setw(2) << minuti << "] ";
    return oss.str();
}