#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <random>
#include <memory>
#include <unordered_map>
#include <functional>
#include "Sistema.h"

// TODO: spostare un po' di sta roba in un file più adatto (se ritenuto opportuno)
Sistema init();
std::vector<std::unique_ptr<Dispositivo>> read_dispositivi();
std::ofstream redirect_output(std::string);
std::vector<std::string> tokenize(std::string, char);
void set(Sistema &, std::vector<std::string> &);
void rm(Sistema &, std::vector<std::string> &);
void show(Sistema &, std::vector<std::string> &);
void reset(Sistema &, std::vector<std::string> &);
int format_time(std::string);
std::vector<std::string> parse_input(std::string);
std::vector<std::string> parse_multi_words(const std::vector<std::string> &);
std::string extractDeviceName(const std::vector<std::string> &);
void execute_default_commands(Sistema &);

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
    std::ofstream outFile("");
    if (argc >= 2) // se non viene passato alcun argomento, stampo normalmente a schermo
    {
        log_file = argv[1];
        outFile = std::ofstream("logs\\" + log_file);
        if (!outFile)
        {
            std::cerr << "Error opening file for output.\n";
            return 1;
        }

        std::streambuf *originalCoutBuffer = std::cout.rdbuf();
        std::cout.rdbuf(outFile.rdbuf());
    }

    // soluzione più o meno elegante
    // per gestire molteplici comandi senza avere uno switch case oppure una catena di if-else.
    // ad ogni key nella hash_map (set, rm, ...) viene associata una funzione che ha sempre lo stesso tipo di firma
    std::unordered_map<std::string, std::function<void(Sistema &, std::vector<std::string> &)>> commands = {
        {"set", set},
        {"rm", rm},
        {"show", show},
        {"reset", reset}};

    Sistema sistema(read_dispositivi());

    execute_default_commands(sistema);

    // sarebbe carino spostare tutto da inizio main fino a qui in una funzione init().
    // probabilmente sorgerebbero problemi di visibilità, quindi se si riesce 
    // a trovare una soluzione elegante bene, altrimenti non importa

    std::string input;
    while (true)
    {
        std::getline(std::cin, input);
        std::vector<std::string> tokens = parse_input(input);
        std::string command = tokens[0];

        if (command == "exit")
        {
            break;
        }

        auto it = commands.find(command);
        if (it != commands.end())
        {
            // eseguo la funzione associata al comando
            it->second(sistema, tokens);
        }
        else
        {
            std::cerr << "Errore: comando sconosciuto" << std::endl;
        }
    }

    return 0;
}

// sarebbe carino utilizzare la porzione di codice già scritta nel main per eseguire
// i comandi di default.
// per ora va bene così
void execute_default_commands(Sistema &s)
{
    // TODO: anche qui attenzione al percorso del file
    std::ifstream file("src\\utilities\\comandi.txt");
    if (!file.is_open())
    {
        std::cerr << "Errore nell'apertura del file comandi.txt" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::string line;
    while (std::getline(file, line))
    {
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
            std::cerr << "Errore: comando sconosciuto" << std::endl;
        }
    }
    file.close();
}

std::vector<std::string> parse_input(std::string input)
{
    std::vector<std::string> tokens = tokenize(input, ' ');
    return parse_multi_words(tokens);
}

// Unisco tutte le stringhe separate da spazi, racchiuse da '""', in un'unica stringa.
// se la stringa inizia con '"', considero come stessa stringa
// tutto quello che trovo fino a quando non trovo qualcosa che termina per '"'
std::vector<std::string> parse_multi_words(const std::vector<std::string> &tokens)
{
    std::vector<std::string> parsed_tokens;

    int i = 0;
    while (i < tokens.size())
    {
        if (tokens[i][0] == '"')
        {
            // dovrei aver gestito correttamente i seguenti casi (controllare pls):
            // se la stringa è composta da più parole, devo togliere il primo e l'ultimo carattere;
            // se la stringa è composta da una sola parola, devo togliere solo il primo e l'ultimo carattere;
            // errori di off-by-one
            std::string multi_word = tokens[i];
            ++i;
            while (i < tokens.size())
            {
                if (multi_word.back() == '"')
                {
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

void set(Sistema &s, std::vector<std::string> &tokens)
{
    if (tokens.size() < 2)
    {
        std::cerr << "Utilizzo: set {time|<devicename>}" << std::endl;
        return;
    }

    if (tokens[1] == "time")
    {
        if (tokens.size() < 3)
        {
            std::cerr << "Utilizzo: set time <hh:mm>" << std::endl;
            return;
        }
        try
        {
            int time = format_time(tokens[2]);
            s.impostaOrarioSistema(time);
        }
        catch (const std::invalid_argument &e)
        {
            std::cerr << e.what() << std::endl;
        }
        return;
    }

    if (tokens.size() < 3)
    {
        std::cerr << "Utilizzo: set <devicename> {on|off|<start>} [<end>]" << std::endl;
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
        std::cerr << e.what() << std::endl;
    }
}

void rm(Sistema &s, std::vector<std::string> &tokens)
{
    if (tokens.size() < 2)
    {
        std::cerr << "Utilizzo: rm <devicename>" << std::endl;
        return;
    }

    std::string device_name = tokens[1];

    s.rimuoviOrario(device_name);
}

void show(Sistema &s, std::vector<std::string> &tokens)
{
    if (tokens.size() < 2)
    {
        s.stampaDispositivi();
        return;
    }

    std::string device_name = tokens[1];

    s.stampaDispositivo(device_name);
}

void reset(Sistema &s, std::vector<std::string> &tokens)
{
    if (tokens.size() < 2)
    {
        std::cerr << "Utilizzo: reset {time|timers|all}" << std::endl;
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
        std::cerr << "Utilizzo: reset {time|timers|all}" << std::endl;
    }
}

// splitta una stringa in base a un delimitatore dato
std::vector<std::string> tokenize(std::string input, char delimiter = ' ')
{
    std::stringstream ss(input);
    std::string token;
    std::vector<std::string> tokens;

    while (std::getline(ss, token, delimiter))
    {
        tokens.push_back(token);
    }

    return tokens;
}

// lo scopo di questa funzione è preparare il sistema per l'utilizzo:
// - effettuare il redirect dello stdout su file
// - istanziare i dispositivi leggendoli da file
// - eseguire alcune operazioni predefinite, salvate su un file (es: set frigorifero on)
// Sistema init()
// {
//     std::vector<std::unique_ptr<Dispositivo>> dispositivi = read_dispositivi();

//     // istanzia sistema

//     // ritorna il sistema
//     return Sistema(dispositivi);
// }

// funzione che ritorna uno stream verso un file di log.
// il controllo che il log_file non sia una stringa vuota è da effettuare nella funzione chiamante 
// e, nel caso, utilizzare un altro stream
// std::ofstream redirect_output(std::string log_file)
// {
//     std::ofstream out("logs\\" + log_file);
//     // ATTENZIONE al percorso, tra windows e linux potrebbero esserci problemi,
//     // quindi potrebbe essere necessario dover riscrivere questa riga
//     if (!out.is_open())
//     {
//         std::cerr << "Errore nell'apertura del file di log" << std::endl;
//         return;
//     }
//     std::cout.rdbuf(out.rdbuf());
//     return out;
// }

std::vector<std::unique_ptr<Dispositivo>> read_dispositivi()
{
    // ATTENZIONE al percorso, tra windows e linux potrebbero esserci problemi,
    // quindi potrebbe essere necessario dover riscrivere questa riga
    std::ifstream file("src\\utilities\\dispositivi.txt");
    if (!file.is_open())
    {
        std::cerr << "Errore nell'apertura del file dispositivi.txt" << std::endl;
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
        }
        else
        {
            dispositivi.push_back(std::make_unique<Manuale>(id, nome, consumo, -1, -1));
        }
        id++;
    }
    file.close();
    return dispositivi;
}