#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <random>
#include <memory>
#include <unordered_map>
#include <functional>
#include "Sistema.h"

// funzioni di inizializzazione del sistema
std::vector<std::unique_ptr<Dispositivo>> carica_dispositivi();
void esegui_comandi_default(Sistema &);

// funzioni che gestiscono i comandi del sistema
void set(Sistema &, std::vector<std::string> &);
void rm(Sistema &, std::vector<std::string> &);
void show(Sistema &, std::vector<std::string> &);
void reset(Sistema &, std::vector<std::string> &);

// funzioni di utilita' per una gestione piu' efficace
// dell'input
std::vector<std::string> parse_input(std::string);
std::vector<std::string> tokenize(std::string, char);
std::vector<std::string> parse_multi_words(const std::vector<std::string> &);
int format_time(std::string);

int main(int argc, char *argv[])
{
    // redirect dell'output su file
    // TODO: spostarlo all'interno di una funzione
    // problema:
    /*
    Key Issue: std::ofstream Goes Out of Scope
    The std::ofstream object you use to redirect std::cout
    must remain valid and in scope for the duration of the redirection.
    If the ofstream object is created inside an if block or a function,
    it will be destroyed as soon as the block or function exits. When this happens:
    -The underlying file stream is closed.
    -The std::cout stream is left pointing to an invalid stream buffer, causing undefined behavior or no output.
    */
    std::string log_file = "";
    std::ofstream out_file("");
    if (argc >= 2) // se non viene passato alcun argomento, stampo normalmente a schermo
    {
        log_file = argv[1];
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

    // soluzione più o meno elegante
    // per gestire molteplici comandi senza avere uno switch case oppure una catena di if-else.
    // ad ogni key nella hash_map (set, rm, ...) viene associata una funzione che ha sempre lo stesso tipo di firma
    std::unordered_map<std::string, std::function<void(Sistema &, std::vector<std::string> &)>> comandi = {
        {"set", set},
        {"rm", rm},
        {"show", show},
        {"reset", reset}};

    Sistema sistema(carica_dispositivi());
    esegui_comandi_default(sistema);
    std::string input;
    while (true)
    {
        std::getline(std::cin, input);
        std::cout << input << std::endl;
        std::vector<std::string> tokens = parse_input(input);

        // controllo necessario altrimenti il programma segfaulta
        // in caso di input = a capo (non so bene il motivo)
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

std::vector<std::unique_ptr<Dispositivo>> carica_dispositivi()
{
    // ATTENZIONE al percorso: tra windows e linux il carattere separatore di directory cambia,
    // quindi potrebbe essere necessario dover riscrivere questa riga
    // sostituendo / in \\ e viceversa
    std::ifstream file("../src/utilities/dispositivi.txt");
    if (!file.is_open())
    {
        std::cout << "Errore nell'apertura del file dispositivi.txt" << std::endl;
        // devo necessariamente uscire dal programma dato che non posso fare nulla
        // senza avere i dispositivi
        exit(EXIT_FAILURE);
    }

    std::string line;
    std::vector<std::unique_ptr<Dispositivo>> dispositivi;
    int id = 0; // identificatore dei dispositivi auto incrementale
    while (std::getline(file, line))
    {
        std::vector<std::string> tokens = tokenize(line, ',');
        std::string nome = tokens[0];
        double consumo = std::stod(tokens[1]);
        if (tokens.size() == 4)
        {
            int durata = std::stoi(tokens[3]);
            dispositivi.push_back(std::make_unique<Ciclo>(id, nome, consumo, -1, durata));

            // questa riga e la riga commentata nel blocco "else"
            // vanno sostituite alle altre due righe di dispositivi.push_back()
            // nel caso si voglia fare utilizzo esclusivamente di c++11.
            // Altrimenti, l'utilizzo, piu' sicuro, di make_unique, richiede c++14
            // dispositivi.push_back(std::unique_ptr<Ciclo>(new Ciclo(id, nome, consumo, -1, durata)));
        }
        else
        {
            dispositivi.push_back(std::make_unique<Manuale>(id, nome, consumo, -1, -1));
            // dispositivi.push_back(std::unique_ptr<Manuale>(new Manuale(id, nome, consumo, -1, -1)));
        }
        id++;
    }
    file.close();
    return dispositivi;
}

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
        // non ho idea del perche', ma ogni tanto, leggendo righe da file,
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

// controlla i parametri in input ed eventualmente fa eseguire il comando set
// dal sistema
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

// controlla i parametri in input ed eventualmente fa eseguire il comando rm
// dal sistema
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

// controlla i parametri in input ed eventualmente fa eseguire il comando show
// dal sistema
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

// controlla i parametri in input ed eventualmente fa eseguire il comando reset
// dal sistema
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

std::vector<std::string> parse_input(std::string input)
{
    std::vector<std::string> tokens = tokenize(input, ' ');
    return parse_multi_words(tokens);
}

// splitta una stringa in base a un delimitatore dato
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

// Unisce tutte le stringhe separate da spazi, racchiuse da '""', in un'unica stringa.
// se la stringa inizia con '"', considera come stessa stringa
// tutto quello che trovo fino a quando non trova qualcosa che termina per '"'
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
                    // se entro qua dentro, multi_word è una stringa del tipo: "parole comprese tra virgolette"
                    // mentre i punta alla parola successiva a 'virgolette"'
                    break;
                }
                multi_word += " " + tokens[i];
                ++i;
            }
            // elimino le virgolette iniziali e finali
            multi_word = multi_word.substr(1, multi_word.size() - 2);
            parsed_tokens.push_back(multi_word);
        }
        else
        {
            // parola senza virgolette, aggiungo direttamente
            parsed_tokens.push_back(tokens[i]);
            ++i;
        }
    }

    return parsed_tokens;
}

// converte un orario in formato hh:mm in minuti
// es: 12:30 = 12*60 + 30 = 750
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