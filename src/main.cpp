// Riccardo Zamuner
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <memory>
#include <unordered_map>
#include <functional>
#include "Sistema.h"

// Funzioni di inizializzazione del sistema

std::vector<std::unique_ptr<Dispositivo>> carica_dispositivi();
void esegui_comandi_default(Sistema &);

// Funzioni che gestiscono i comandi del sistema

void set(Sistema &, std::vector<std::string> &);
void rm(Sistema &, std::vector<std::string> &);
void show(Sistema &, std::vector<std::string> &);
void reset(Sistema &, std::vector<std::string> &);

// Funzioni di utilita' per una gestione piu' efficace
// dell'input

std::vector<std::string> parse_input(std::string);
std::vector<std::string> tokenize(std::string, char);
std::vector<std::string> parse_multi_words(const std::vector<std::string> &);
int format_time(std::string);

int main(int argc, char *argv[])
{
    // Imposto un valore di default per la potenza massima
    // Se c'è un secondo argomento valido sovrascrivo il valore di default
    double potenza_massima = Sistema::POTENZA_MASSIMA_DEFAULT;
    if (argc >= 2)
    {
        try
        {
            potenza_massima = std::stod(argv[1]);
        }
        catch (const std::invalid_argument &e)
        {
            std::cout << "Errore: potenza massima non valida. Utilizzo il valore di default." << std::endl;
        }
    }
    else
    {
        std::cout << "Utilizzo con parametri personalizzati: " << argv[0] << " [potenza_massima] [log_file] " << std::endl;
    }

    // Redirect dell'output su file
    std::string log_file = "";
    std::ofstream out_file("");
    if (argc >= 3) // Se non viene passato alcun argomento come file di log, stampo normalmente a schermo
    {
        log_file = argv[2];
        // ATTENZIONE al percorso: tra windows e linux il carattere separatore di directory cambia,
        // quindi potrebbe essere necessario dover riscrivere questa riga
        // sostituendo / in \\ e viceversa
        out_file = std::ofstream("../logs/" + log_file);
        if (!out_file)
        {
            std::cout << "Errore nell'apertura del file di salvataggio log." << std::endl;
            return 1;
        }

        std::cout.rdbuf(out_file.rdbuf());
    }

    // Soluzione più o meno elegante
    // per gestire molteplici comandi senza avere uno switch case oppure una catena di if-else.
    // Ad ogni key nella hash_map (set, rm, ...) viene associata una funzione che ha sempre lo stesso tipo di firma
    std::unordered_map<std::string, std::function<void(Sistema &, std::vector<std::string> &)>> comandi = {
        {"set", set},
        {"rm", rm},
        {"show", show},
        {"reset", reset}};

    Sistema sistema(carica_dispositivi(), potenza_massima);
    esegui_comandi_default(sistema);

    std::string input;
    while (true)
    {
        std::getline(std::cin, input);
        std::cout << input << std::endl;

        std::vector<std::string> tokens = parse_input(input);

        // Controllo necessario altrimenti il programma segfaulta
        // in caso di input = a capo
        if (tokens.empty() || tokens[0].empty())
        {
            continue;
        }

        std::string command = tokens[0];
        if (command == "exit")
        {
            break;
        }

        auto it = comandi.find(command);
        if (it != comandi.end())
        {
            // eseguo la funzione associata al comando
            it->second(sistema, tokens);
        }
        else
        {
            std::cout << "Errore: comando sconosciuto" << std::endl;
        }
    }
    return 0;
}

/**
 * @brief Carica i dispositivi da un file di testo e li restituisce come un vettore di puntatori unici.
 *
 * Questa funzione legge i dati dei dispositivi da un file di testo situato in
 * "../src/utilities/dispositivi.txt". Ogni riga del file rappresenta un dispositivo e i campi
 * sono separati da virgole. La funzione crea oggetti di tipo Ciclo fisso o Manuale a seconda del numero
 * di campi presenti in ogni riga e li aggiunge a un vettore che verra' utilizzato per
 * definire la classe sistema.
 *
 * @return std::vector<std::unique_ptr<Dispositivo>> Un vettore di smart pointers ai dispositivi caricati.
 *
 * @throws std::runtime_error Se il file non può essere aperto.
 */
std::vector<std::unique_ptr<Dispositivo>> carica_dispositivi()
{
    // ATTENZIONE al percorso: tra windows e linux il carattere separatore di directory cambia,
    // quindi potrebbe essere necessario dover riscrivere questa riga
    // sostituendo / in \\ e viceversa
    std::ifstream file("../src/utilities/dispositivi.txt");
    if (!file.is_open())
    {
        std::cout << "Errore nell'apertura del file dispositivi.txt" << std::endl;
        // Devo necessariamente uscire dal programma dato che non posso fare nulla
        // senza avere i dispositivi
        exit(EXIT_FAILURE);
    }

    std::string line;
    std::vector<std::unique_ptr<Dispositivo>> dispositivi;
    int id = 0; // Identificatore dei dispositivi auto incrementale
    while (std::getline(file, line))
    {
        std::vector<std::string> tokens = tokenize(line, ',');
        std::string nome = tokens[0];
        double consumo = std::stod(tokens[1]);
        if (tokens.size() == 4)
        {
            int durata = std::stoi(tokens[3]);

            // Questa riga e la riga commentata nel blocco "else"
            // vanno sostituite alle altre due righe di dispositivi.push_back()
            // nel caso si voglia fare utilizzo del piu' sicuro make_unique,
            // che pero' richiede c++14

            // dispositivi.push_back(std::make_unique<Ciclo>(id, nome, consumo, -1, durata));
            dispositivi.push_back(std::unique_ptr<Ciclo>(new Ciclo(id, nome, consumo, -1, durata)));
        }
        else
        {
            // dispositivi.push_back(std::make_unique<Manuale>(id, nome, consumo, -1, -1));
            dispositivi.push_back(std::unique_ptr<Manuale>(new Manuale(id, nome, consumo, -1, -1)));
        }
        id++;
    }
    file.close();
    return dispositivi;
}

/**
 * @brief Esegue una serie di comandi predefiniti letti da un file di testo.
 *
 * Questa funzione legge i comandi da un file di testo situati in
 * "../src/utilities/comandi.txt" e li esegue.
 *
 * @param s Reference al sistema su cui eseguire i comandi.
 *
 * @throws std::runtime_error Se il file non può essere aperto.
 */
void esegui_comandi_default(Sistema &s)
{
    // ATTENZIONE al percorso: tra windows e linux il carattere separatore di directory cambia,
    // quindi potrebbe essere necessario dover riscrivere questa riga
    // sostituendo / in \\ e viceversa
    std::ifstream file("../src/utilities/comandi.txt");
    if (!file.is_open())
    {
        std::cout << "Errore nell'apertura del file comandi.txt" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::string line;
    while (std::getline(file, line))
    {
        // Non ho idea del perche', ma ogni tanto, leggendo righe da file,
        // viene messo un carriage return alla fine, e questo causa grandi problemi
        // nella tokenizzazione e nel parse dell'input per eseguire i comandi.
        // Quindi, se il carattere e' presente, lo rimuovo
        if (line.back() == '\r')
        {
            line = line.substr(0, line.length() - 1);
        }

        std::cout << line << std::endl;
        std::vector<std::string> tokens = parse_input(line);

        std::string command = tokens[0];
        if (command == "set")
        {
            set(s, tokens);
        }
        else if (command == "rm")
        {
            rm(s, tokens);
        }
        else if (command == "show")
        {
            show(s, tokens);
        }
        else if (command == "reset")
        {
            reset(s, tokens);
        }
        else
        {
            std::cout << "Errore: comando sconosciuto" << std::endl;
        }
    }
    file.close();
}

/**
 * @brief Imposta un timer ad un dispositivo oppure imposta il tempo di sistema.
 * 
 * Questa funzione imposta un timer ad un dispositivo oppure imposta il tempo di sistema a
 * seconda dei parametri passati come input.
 * A seconda che il dispositivo sia manuale o a ciclo prefissato, permette di
 * impostare anche un orario di accensione ed eventualmente un orario di spegnimento.
 * 
 * @param s Reference al sistema su cui eseguire i comandi.
 * @param tokens Vettore di stringhe contenente i parametri del comando. Gli elementi dal
 *               secondo in poi devono rispettare il formato imposto
 * 
 * Utilizzo: set time <hh:mm>
 * Utilizzo: set <devicename> {on|off|<start>} [<end>]
 */
void set(Sistema &s, std::vector<std::string> &tokens)
{
    if (tokens.size() < 2)
    {
        std::cout << "Utilizzo: set {time|<devicename>}" << std::endl;
        return;
    }

    if (tokens[1] == "time")
    {
        if (tokens.size() < 3)
        {
            std::cout << "Utilizzo: set time <hh:mm>" << std::endl;
            return;
        }
        try
        {
            int time = format_time(tokens[2]);
            s.impostaOrarioSistema(time);
        }
        catch (const std::invalid_argument &e)
        {
            std::cout << e.what() << std::endl;
        }
        return;
    }

    if (tokens.size() < 3)
    {
        std::cout << "Utilizzo: set <devicename> {on|off|<start>} [<end>]" << std::endl;
        return;
    }

    std::string device_name = tokens[1];
    std::string state = tokens[2];

    if (state == "on" || state == "off")
    {
        if (state == "on")
            s.accensioneDispositivo(device_name);
        else
            s.spegnimentoDispositivo(device_name);
        return;
    }
    try
    {
        int start = format_time(state);
        if (tokens.size() > 3)
        {
            int end = format_time(tokens[3]);
            s.impostaOrario(device_name, start, end);
        }
        else
        {
            s.impostaOrario(device_name, start);
        }
    }
    catch (const std::invalid_argument &e)
    {
        std::cout << e.what() << std::endl;
    }
}

/**
 * @brief Elimina un timer da un dispositivo.
 * 
 * Questa funzione rimuove un timer associato a un dispositivo specificato.
 * Effettua un controllo sui parametri per assicurarsi che il nome del dispositivo
 * sia fornito correttamente.
 * 
 * @param s Reference al sistema su cui eseguire i comandi.
 * @param tokens Vettore di stringhe contenente i parametri del comando. Il secondo elemento
 *               deve essere il nome del dispositivo da cui rimuovere il timer.
 * 
 * Utilizzo: rm <devicename>
 */
void rm(Sistema &s, std::vector<std::string> &tokens)
{
    if (tokens.size() < 2)
    {
        std::cout << "Utilizzo: rm <devicename>" << std::endl;
        return;
    }

    std::string device_name = tokens[1];

    s.rimuoviOrario(device_name);
}

/**
 * @brief Mostra informazioni sui dispositivi nel sistema.
 *
 * Questa funzione visualizza le informazioni sui consumi dei dispositivi gestiti dal sistema.
 * Se il vettore di token contiene meno di due elementi, vengono stampati i consumi di tutti i dispositivi.
 * Altrimenti, vengono stampati i dati di consumo del dispositivo specificato come secondo parametro
 *
 * @param s Reference al sistema su cui eseguire i comandi.
 * @param tokens Vettore di stringhe contenente i parametri del comando. Il secondo token
 *               (se presente) rappresenta il nome del dispositivo di cui visualizzare i consumi.
 * 
 * Utilizzo: show [<devicename>]
 */
void show(Sistema &s, std::vector<std::string> &tokens)
{
    if (tokens.size() < 2)
    {
        s.stampaDispositivi();
        return;
    }

    std::string device_name = tokens[1];

    s.stampaDispositivo(device_name, true);
}

/**
 * @brief Funzione di debug che resetta il sistema in base al parametro specificato.
 *
 * Questa funzione permette di resettare il sistema in base al parametro fornito.
 * I parametri accettati sono:
 * - "time": resetta l'orario del sistema.
 * - "timers": resetta gli orari dei dispositivi.
 * - "all": resetta l'intero sistema.
 *
 * @param s Reference al sistema su cui eseguire i comandi.
 * @param tokens Vettore di stringhe contenente i parametri del comando. Il secondo elemento deve essere uno dei parametri accettati ("time", "timers", "all").
 *
 * @note Se il numero di parametri è inferiore a 2 o se il parametro non è valido, viene stampato un messaggio di utilizzo.
 * 
 * Utilizzo: reset {time|timers|all}
 */
void reset(Sistema &s, std::vector<std::string> &tokens)
{
    if (tokens.size() < 2)
    {
        std::cout << "Utilizzo: reset {time|timers|all}" << std::endl;
        return;
    }

    std::string arg = tokens[1];

    if (arg == "time")
    {
        s.resetOrarioSistema();
    }
    else if (arg == "timers")
    {
        s.resetOrariDispositivi();
    }
    else if (arg == "all")
    {
        s.resetSistema();
    }
    else
    {
        std::cout << "Utilizzo: reset {time|timers|all}" << std::endl;
    }
}

/**
 * @brief Elabora una stringa data come input
 *
 * Questa funzione prende una stringa di input, la suddivide in token utilizzando
 * lo spazio (' ') come delimitatore e poi elabora ulteriormente i token per gestire
 * stringhe composte da piu' parole separate da spazi e delimitate da virgolette ("").
 *
 * @param input La stringa di input da analizzare.
 * @return std::vector<std::string> Un vettore di stringhe contenente i token parsati.
 */
std::vector<std::string> parse_input(std::string input)
{
    std::vector<std::string> tokens = tokenize(input, ' ');
    return parse_multi_words(tokens);
}

/**
 * @brief Suddivide una stringa in un vettore di sottostringhe separate da un delimitatore specificato.
 *
 * @param input La stringa da suddividere.
 * @param delimitatore Il carattere utilizzato come delimitatore per la suddivisione.
 * Di default viene utilizzato lo spazio(' ').
 * @return std::vector<std::string> Un vettore contenente le sottostringhe risultanti dalla suddivisione.
 */
std::vector<std::string> tokenize(std::string input, char delimitatore = ' ')
{
    std::stringstream ss(input);
    std::string token;
    std::vector<std::string> tokens;

    while (std::getline(ss, token, delimitatore))
    {
        tokens.push_back(token);
    }

    return tokens;
}

/**
 * @brief Questa funzione prende un vettore di stringhe (tokens) e combina le parole racchiuse tra virgolette in un'unica stringa.
 *
 * @param tokens Un vettore di stringhe che rappresenta i token da analizzare.
 * @return std::vector<std::string> Un vettore di stringhe in cui le parole racchiuse tra virgolette sono state combinate in un'unica stringa senza virgolette.
 *
 * La funzione scorre il vettore di token e, quando trova una parola che inizia con una virgoletta ("), continua ad aggiungere le parole successive fino a trovare una parola che termina con una virgoletta.
 * Le virgolette iniziali e finali vengono rimosse dalla stringa combinata prima di aggiungerla al vettore di output.
 * Le parole che non sono racchiuse tra virgolette vengono aggiunte direttamente al vettore di output.
 */
std::vector<std::string> parse_multi_words(const std::vector<std::string> &tokens)
{
    std::vector<std::string> parsed_tokens;

    long unsigned int i = 0;
    while (i < tokens.size())
    {
        if (tokens[i][0] == '"')
        {
            std::string multi_word = tokens[i];
            ++i;
            while (i < tokens.size())
            {
                if (multi_word.back() == '"')
                {
                    // Se entro qua dentro, multi_word è una stringa del tipo: "parole comprese tra virgolette"
                    // mentre i punta alla parola successiva a 'virgolette"'
                    break;
                }
                multi_word += " " + tokens[i];
                ++i;
            }
            // Elimino le virgolette iniziali e finali
            multi_word = multi_word.substr(1, multi_word.size() - 2);
            parsed_tokens.push_back(multi_word);
        }
        else
        {
            // Parola senza virgolette, aggiungo direttamente
            parsed_tokens.push_back(tokens[i]);
            ++i;
        }
    }

    return parsed_tokens;
}

/**
 * @brief Converte una stringa di orario nel formato "hh:mm" in minuti dall'inizio della giornata.
 *
 * Questa funzione prende una stringa che rappresenta un orario nel formato "hh:mm" e la converte
 * nel numero totale di minuti dall'inizio della giornata (00:00). Se il formato della stringa non è
 * valido o se l'orario non è valido, viene lanciata un'eccezione std::invalid_argument.
 *
 * @param time La stringa che rappresenta l'orario nel formato "hh:mm".
 * @return Il numero totale di minuti dall'inizio della giornata.
 * @throws std::invalid_argument Se il formato della stringa non è valido o se l'orario non è valido.
 */
int format_time(std::string time)
{
    std::stringstream time_ss(time);
    std::string hour_str, minute_str;
    if (!std::getline(time_ss, hour_str, ':') || !std::getline(time_ss, minute_str, ':'))
    {
        throw std::invalid_argument("Errore: formato non valido. Utilizza hh:mm");
    }
    int hour = std::stoi(hour_str);
    int minute = std::stoi(minute_str);
    if (hour < 0 || hour >= 24 || minute < 0 || minute >= 60)
    {
        throw std::invalid_argument("Errore: orario non valido");
    }
    return hour * 60 + minute;
}