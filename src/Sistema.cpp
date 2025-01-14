//Sperandio Lorenzo 
#include <iomanip>
#include <sstream>
#include <cmath>
#include "Sistema.h"

/**
 * @brief Costruttore della classe Sistema.
 * 
 * Inizializza un oggetto Sistema con una lista di dispositivi e una capacità massima.
 * La capacità attuale viene inizializzata al valore della capacità massima e l'orario viene impostato a 0.
 * 
 * @param dispositivi Un vettore di puntatori unici a oggetti Dispositivo.
 * @param capacita_max La capacità massima del sistema.
 */

Sistema::Sistema(std::vector<std::unique_ptr<Dispositivo>> dispositivi, double capacita_max)
    : capacita_max(capacita_max), capacita_attuale(capacita_max), orario(0)
{
    this->dispositivi = std::move(dispositivi);
}

 /**
 * Ricerca il dispositivo con il nome passato come parametro e, se la capacità aggiunta non supera il massimo del sistema, accende il dispositivo.
 * 
 * @param nome Il nome del dispositivo da accendere.
 *
 * Comportamento:
 * - Se il dispositivo è già acceso, stampa un messaggio indicante che il dispositivo è già acceso.
 * - Se accendere il dispositivo supera la capacità massima del sistema, stampa un messaggio indicante che non è possibile accendere il dispositivo.
 * - Se il dispositivo può essere acceso senza superare la capacità massima, aggiorna la capacità attuale, imposta lo stato del dispositivo su acceso e stampa un messaggio indicante che il dispositivo è stato acceso.
 * - Se il dispositivo non viene trovato, stampa un messaggio indicante che il dispositivo non è stato trovato.
 */
void Sistema::accensioneDispositivo(std::string nome)
{
    bool found = false;

    for (auto &dispositivo : dispositivi)
    {
        if (caseInsensitiveStringCompare(dispositivo->getNome(), nome))
        {
            // Controllo se il dispositivo e' gia' acceso
            if (dispositivo->getStato())
            {
                std::cout << stampaOrario(this->orario) << "Dispositivo " << nome << " gia' acceso" << std::endl;
            }
            else
            {
                // Controllo che la capacita' aggiunta non sfori il max.
                // Se accendendo questi dispositivo sforo il limite di potenza,
                // allora mi basta spegnerlo (non accenderlo neanche) per ritornare
                // ad una potenza accettabile
                if (capacita_attuale + dispositivo->getConsumo() < 0)
                {
                    std::cout << stampaOrario(this->orario) << "Non si puo' accendere il dispositivo '" << nome << "' perche' sforerebbe il massimo della capacita'" << std::endl;
                    dispositivo->setOrarioAccensione(-1);

                    if (dynamic_cast<Manuale *>(dispositivo.get()))
                    {
                        dynamic_cast<Manuale *>(dispositivo.get())->setOrarioSpegnimento(-1);
                    }
                }
                else
                {
                    capacita_attuale += dispositivo->getConsumo();
                    dispositivo->setStato(true);

                    // Controllo se l'orario di accensione non e' stato settato
                    if (dispositivo->getOrarioAccensione() == -1)
                    {
                        dispositivo->setOrarioAccensione(orario);
                    }
                    std::cout << stampaOrario(this->orario) << "Il dispositivo '" << dispositivo->getNome() << "' si e' acceso" << std::endl;
                }
            }
            found = true;
            break;
        }
    }
    if (!found)
    {
        std::cout << stampaOrario(this->orario) << "Il dispositivo '" << nome << "' non e' stato trovato" << std::endl;
    }
}

/**
 * @brief Spegne un dispositivo specificato dal nome.
 * 
 * Ricerca il dispositivo con il nome passato come parametro e, se il dispositivo è acceso, lo spegne.
 * 
 * @param nome Il nome del dispositivo da spegnere.
 *
 * Comportamento:
 * - Se il dispositivo è già spento, stampa un messaggio indicante che il dispositivo è già spento.
 * - Se spegnere il dispositivo porta la capacità attuale sotto zero, spegne altri dispositivi fino a riportare la capacità a un valore accettabile.
 * - Aggiorna la capacità attuale, imposta lo stato del dispositivo su spento e stampa un messaggio indicante che il dispositivo è stato spento.
 * - Se il dispositivo non viene trovato, stampa un messaggio indicante che il dispositivo non è stato trovato.
 */
void Sistema::spegnimentoDispositivo(std::string nome)
{
    bool found = false;
    for (auto &dispositivo : dispositivi)
    {
        if (caseInsensitiveStringCompare(dispositivo->getNome(), nome))
        {
            if (!dispositivo->getStato())
            {
                std::cout << stampaOrario(this->orario) << "Il dispositivo '" << nome << "' e' gia' spento" << std::endl;
            }
            else
            {
                while (capacita_attuale - dispositivo->getConsumo() < 0)
                {
                    long unsigned int idx = -1;
                    int max = -1;
                    for (long unsigned int i = 0; i < dispositivi.size(); i++)
                    {
                        if (dispositivi[i]->getStato() && dispositivi[i]->getOrarioAccensione() > max && dispositivi[i]->getConsumo() < 0)
                        {
                            max = dispositivi[i]->getOrarioAccensione();
                            idx = i;
                        }
                    }

                    // Non devo controllare che idx sia != -1 perche' non esistono casi
                    // in cui non sia possibile spegnere qualcosa, perche' al piu' si puo'
                    // spegnere ogni dispositivo per arrivare ad una potenza pari a 0
                    spegnimentoDispositivoHelper(*dispositivi[idx]);
                }

                spegnimentoDispositivoHelper(*dispositivo);
            }
            found = true;
            break;
        }
    }

    if (!found)
    {
        std::cout << stampaOrario(this->orario) << "Il dispositivo '" << nome << "' non e' stato trovato" << std::endl;
    }
}

/**
 * @brief Imposta l'orario di accensione di un dispositivo specificato dal nome.
 * 
 * Ricerca il dispositivo con il nome passato come parametro e, se il dispositivo non è attualmente acceso, imposta l'orario di accensione.
 * 
 * @param nome Il nome del dispositivo per cui impostare l'orario di accensione.
 * @param orario L'orario di accensione da impostare.
 *
 * Comportamento:
 * - Se l'orario passato è minore o uguale all'orario attuale del sistema, stampa un messaggio indicante che l'orario inserito non è valido.
 * - Se il dispositivo è attualmente acceso, stampa un messaggio indicante che il dispositivo è attualmente acceso.
 * - Se il dispositivo ha già un timer impostato, stampa un messaggio indicante che il dispositivo ha già un timer impostato.
 * - Se il dispositivo non ha un timer impostato, imposta l'orario di accensione e stampa un messaggio indicante che l'orario di accensione è stato impostato.
 * - Se il dispositivo non viene trovato, stampa un messaggio indicante che il dispositivo non è stato trovato.
 */
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
                if (dispositivo->getOrarioAccensione() < this->orario)
                {
                    std::cout << stampaOrario(this->orario) << "Il dispositivo '" << nome << "' e' attualmente acceso" << std::endl;
                }
                else
                {
                    std::cout << stampaOrario(this->orario) << "Il dispositivo '" << nome << "' ha gia' un timer impostato" << std::endl;
                }
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
        std::cout << stampaOrario(this->orario) << "Il dispositivo '" << nome << "' non e' stato trovato" << std::endl;
    }
}

/**
 * @brief Imposta l'orario di accensione e spegnimento di un dispositivo specificato dal nome.
 * 
 * Ricerca il dispositivo con il nome passato come parametro e, se il dispositivo non è attualmente acceso, imposta l'orario di accensione e spegnimento.
 * 
 * @param nome Il nome del dispositivo per cui impostare l'orario di accensione e spegnimento.
 * @param orario_accensione L'orario di accensione da impostare.
 * @param orario_spegnimento L'orario di spegnimento da impostare.
 *
 * Comportamento:
 * - Se l'orario di accensione o spegnimento passato è minore o uguale all'orario attuale del sistema, stampa un messaggio indicante che l'orario inserito non è valido.
 * - Se l'orario di accensione è maggiore o uguale all'orario di spegnimento, stampa un messaggio indicante che l'orario inserito non è valido.
 * - Se il dispositivo è attualmente acceso, stampa un messaggio indicante che il dispositivo è attualmente acceso.
 * - Se il dispositivo ha già un timer impostato, stampa un messaggio indicante che il dispositivo ha già un timer impostato.
 * - Se il dispositivo non ha un timer impostato, imposta l'orario di accensione e spegnimento (se applicabile) e stampa un messaggio indicante che l'orario di accensione e spegnimento è stato impostato.
 * - Se il dispositivo non viene trovato, stampa un messaggio indicante che il dispositivo non è stato trovato.
 */
void Sistema::impostaOrario(std::string nome, int orario_accensione, int orario_spegnimento)
{
    if (orario_accensione <= this->orario || orario_spegnimento <= this->orario)
    {
        std::cout << stampaOrario(this->orario) << "L'orario non e' valido" << std::endl;
        return;
    }
    if (orario_accensione >= orario_spegnimento)
    {
        std::cout << stampaOrario(this->orario) << "L'orario non e' valido" << std::endl;
        return;
    }

    bool found = false;
    for (auto &dispositivo : dispositivi)
    {
        if (caseInsensitiveStringCompare(dispositivo->getNome(), nome))
        {
            if (dispositivo->getOrarioAccensione() != -1)
            {
                if (dispositivo->getOrarioAccensione() < this->orario)
                {
                    std::cout << stampaOrario(this->orario) << "Il dispositivo '" << nome << "' e' attualmente acceso" << std::endl;
                }
                else
                {
                    std::cout << stampaOrario(this->orario) << "Il dispositivo '" << nome << "' ha gia' un timer impostato" << std::endl;
                }
            }
            else
            {
                dispositivo->setOrarioAccensione(orario_accensione);
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
        std::cout << stampaOrario(this->orario) << "Il dispositivo '" << nome << "' non e' stato trovato" << std::endl;
    }
}

/**
 * @brief Rimuove l'orario di accensione di un dispositivo specificato dal nome.
 * 
 * Ricerca il dispositivo con il nome passato come parametro e, se il dispositivo non è attualmente acceso, rimuove l'orario di accensione.
 * 
 * @param nome Il nome del dispositivo per cui rimuovere l'orario di accensione.
 *
 * Comportamento:
 * - Se il dispositivo è attualmente acceso, stampa un messaggio indicante che il dispositivo è attualmente acceso.
 * - Se il dispositivo è di tipo Manuale, rimuove anche l'orario di spegnimento.
 * - Se l'orario di accensione viene rimosso con successo, stampa un messaggio indicante che il timer è stato rimosso.
 * - Se il dispositivo non viene trovato, stampa un messaggio indicante che il dispositivo non è stato trovato.
 */
void Sistema::rimuoviOrario(std::string nome)
{
    bool found = false;
    for (auto &dispositivo : dispositivi)
    {
        if (caseInsensitiveStringCompare(dispositivo->getNome(), nome))
        {
            if (dispositivo->getStato())
            {
                std::cout << stampaOrario(this->orario) << "Il dispositivo '" << nome << "' e' attualmente acceso" << std::endl;
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
        std::cout << "Rimosso il timer dal dispositivo '" << nome << "'" << std::endl;
    }
    else
    {
        std::cout << "Il dispositivo '" << nome << "' non e' stato trovato" << std::endl;
    }
}

/**
 * @brief Stampa lo stato attuale dei dispositivi nel sistema, inclusi i kWh prodotti e consumati.
 * 
 * Mostra la lista di tutti i dispositivi con la produzione/consumo energetico di ciascuno dall'ultimo orario di set al momento di invio del comando.
 * Inoltre mostra la produzione/consumo energetica totale del sistema dalle 00:00 al momento di invio del comando
 * utilizza due for, perche' il secondo stampa solo i dispostivi che hanno/prodotto/consumato dopo l'ultimo orario di set
 * 
 * @details
 * - kWProdotti: Energia totale prodotta dai dispositivi accesi.
 * - kWConsumati: Energia totale consumata dai dispositivi accesi.
 * - consumoTmp: Consumo temporaneo di un singolo dispositivo.
 * 
 * La funzione itera su tutti i dispositivi, calcolando il consumo totale per ciascuno di essi. Se il dispositivo è acceso,
 * il consumo viene aggiornato in base all'orario corrente. I dispositivi che producono energia contribuiscono a kWProdotti,
 * mentre quelli che consumano energia contribuiscono a kWConsumati.
 * 
 * Viene poi stampato l'orario corrente, l'energia totale prodotta e consumata, e una lista dettagliata dei dispositivi.
 */
void Sistema::stampaDispositivi()
{
    double kWProdotti = 0;
    double kWConsumati = 0;
    double consumoTmp = 0;

    for (auto &dispositivo : dispositivi)
    {
        consumoTmp = dispositivo->getConsumoTotale();

        int orarioAccensione = dispositivo->getOrarioAccensione();
        // aggiungo i consumi/produzione se il dispositivo e' attualmente acceso
        if (orarioAccensione != -1 && orarioAccensione < orario)
        {
            consumoTmp += (orario - orarioAccensione) * dispositivo->getConsumo() / 60;
        }

        if (consumoTmp > 0)
        {
            kWProdotti += consumoTmp;
        }
        else
        {
            kWConsumati -= consumoTmp;
        }
    }

    kWConsumati = fabs(kWConsumati);
    std::cout << stampaOrario(this->orario) << "Attualmente il sistema ha prodotto " << kWProdotti << "kWh e consumato " << kWConsumati << "kWh " << std::endl;
    std::cout << "\tNello specifico: " << std::endl;
    for (auto &dispositivo : dispositivi)
    {
        std::cout << "\t- ";
        stampaDispositivo(dispositivo->getNome(), false);
    }
}

/**
 * @brief Stampa le informazioni di un dispositivo specificato per nome.
 * 
 * Questa funzione cerca un dispositivo nella lista dei dispositivi e, se trovato,
 * stampa il consumo totale o la produzione totale del dispositivo. Se il dispositivo
 * è attualmente acceso, aggiunge il consumo/produzione corrente al totale.
 * 
 * @param nome Il nome del dispositivo da cercare.
 * @param stampaOrarioFlag Flag opzionale per indicare se stampare o meno l'orario corrente.
 *                         Il valore predefinito è true.
 */
void Sistema::stampaDispositivo(std::string nome, bool stampaOrarioFlag = true)
{
    bool found = false;
    double consumo = 0;

    for (auto &dispositivo : dispositivi)
    {
        if (caseInsensitiveStringCompare(dispositivo->getNome(), nome))
        {
            consumo = dispositivo->getConsumoTotale();
            int orarioAccensione = dispositivo->getOrarioAccensione();
            // aggiungo i consumi/produzione se il dispositivo e' attualmente acceso
            if (orarioAccensione != -1 && orarioAccensione < orario)
            {
                consumo += (orario - orarioAccensione) * dispositivo->getConsumo() / 60;
            }
            found = true;
            break;
        }
    }
    if (!found)
    {
        std::cout << "Il dispositivo '" << nome << "' non e' stato trovato" << std::endl;
    }
    else
    {
        if (stampaOrarioFlag)
        {
            std::cout << stampaOrario(this->orario);
        }
        // ci si aspetta che un dispositivo o produce o consuma
        if (consumo <= 0)
        {
            std::cout << "Il dispositivo '" << nome << "' ha attualmente consumato " << fabs(consumo) << "kWh" << std::endl;
        }
        else
        {
            std::cout << "Il dispositivo '" << nome << "' ha attualmente prodotto " << consumo << "kWh" << std::endl;
        }
    }
}

/**
 * @brief Imposta l'orario del sistema e aggiorna lo stato dei dispositivi.
 * 
 * Questa funzione aggiorna l'orario del sistema incrementandolo di un minuto alla volta fino a raggiungere l'orario specificato.
 * Durante ogni iterazione, controlla lo stato dei dispositivi e li accende o spegne in base agli orari di accensione e spegnimento configurati.
 * 
 * @param orario L'orario da impostare nel formato minuti (es. 150 per le 2:30).
 */
void Sistema::impostaOrarioSistema(int orario)
{
    std::cout << stampaOrario(this->orario) << "L'orario attuale e' " << stampaOrario(this->orario).substr(1, 5);
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
    std::cout << stampaOrario(this->orario) << "L'orario attuale e' " << stampaOrario(this->orario).substr(1, 5) << std::endl;
}

/**
 * @brief Resetta l'orario del sistema e aggiorna lo stato dei dispositivi.
 *
 * Questa funzione esegue le seguenti operazioni:
 * - Stampa l'orario corrente del sistema e un messaggio di reset.
 * - Reimposta l'orario del sistema a 00:00.
 * - Disattiva tutti i dispositivi e reimposta il loro consumo totale a 0.
 * - Reimposta la capacità attuale del sistema alla capacità massima.
 */
void Sistema::resetOrarioSistema()
{
    std::cout << stampaOrario(this->orario) << "L'orario del sistema e' stato resettato" << std::endl;
    this->orario = 0;
    for (auto &dispositivo : dispositivi)
    {
        dispositivo->setStato(false);
        dispositivo->setConsumoTotale(0);
    }
    capacita_attuale = capacita_max;
}

/**
 * @brief Resetta gli orari di accensione e spegnimento dei dispositivi.
 * 
 * Questa funzione stampa l'orario corrente e un messaggio di reset dei timer dei dispositivi.
 * Successivamente, per ogni dispositivo nella lista dei dispositivi:
 * - Se il dispositivo è spento (stato false), imposta l'orario di accensione a -1.
 * - Se il dispositivo è di tipo Manuale, imposta l'orario di accensione e di spegnimento a -1.
 */
void Sistema::resetOrariDispositivi()
{
    std::cout << stampaOrario(this->orario) << "I timer dei dispositivi sono stati resettati" << std::endl;

    for (auto &dispositivo : dispositivi)
    {
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

/**
 * @brief Ripristina il sistema alle condizioni iniziali.
 * 
 * Questa funzione esegue le seguenti operazioni:
 * - Stampa l'orario corrente e un messaggio di ripristino.
 * - L’orario viene impostato a 00:00
 * - Tutti i timer vengono rimossi
 * - Tutti i dispositivi vengono spenti
 */
void Sistema::resetSistema()
{
    std::cout << stampaOrario(this->orario) << "Il sistema e' stato portato alle condizioni iniziali" << std::endl;
    resetOrarioSistema();
    resetOrariDispositivi();
}

/**
 * @brief Confronta due stringhe in modo "insensitive".
 *
 * Questa funzione confronta due stringhe carattere per carattere senza 
 * considerare le differenze tra maiuscole e minuscole. Restituisce true 
 * se le stringhe sono uguali, altrimenti false.
 * 
 * @param str1 La prima stringa da confrontare.
 * @param str2 La seconda stringa da confrontare.
 * @return true se le stringhe sono uguali ignorando le maiuscole, false altrimenti.
 */
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

/**
 * @brief Converte un orario espresso in minuti in una stringa formattata.
 *
 * Questa funzione prende un orario espresso in minuti dall'inizio della giornata
 * e lo converte in una stringa formattata nel formato [HH:MM].
 *
 * @param orario L'orario espresso in minuti.
 * @return Una stringa che rappresenta l'orario nel formato [HH:MM].
 */
std::string Sistema::stampaOrario(int orario)
{
    int ora = orario / 60;
    int minuti = orario % 60;
    std::ostringstream oss;
    oss << "[" << std::setfill('0') << std::setw(2) << ora << ":" << std::setfill('0') << std::setw(2) << minuti << "] ";
    return oss.str();
}

/**
 * @brief Spegne un dispositivo e aggiorna lo stato del sistema.
 * 
 * Questa funzione riduce la capacità attuale del sistema in base al consumo del dispositivo,
 * imposta lo stato del dispositivo su spento e, se il dispositivo è di tipo Manuale, aggiorna
 * l'orario di spegnimento. Inoltre, stampa un messaggio di spegnimento e aggiorna il consumo
 * totale del dispositivo.
 * 
 * @param dispositivo Riferimento al dispositivo da spegnere.
 */
void Sistema::spegnimentoDispositivoHelper(Dispositivo &dispositivo)
{
    capacita_attuale -= dispositivo.getConsumo();
    dispositivo.setStato(false);
    if (dynamic_cast<Manuale *>(&dispositivo))
    {
        dynamic_cast<Manuale *>(&dispositivo)->setOrarioSpegnimento(orario);
    }
    std::cout << stampaOrario(this->orario) << "Il dispositivo '" << dispositivo.getNome() << "' si e' spento" << std::endl;
    dispositivo.setConsumoTotale(dispositivo.getConsumoTotale() + (orario - dispositivo.getOrarioAccensione()) * dispositivo.getConsumo() / 60);
    dispositivo.setOrarioAccensione(-1);
}