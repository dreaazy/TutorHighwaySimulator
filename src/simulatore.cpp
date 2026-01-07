//Author: Manuel Carelli
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <queue>
#include <algorithm>
#include <sstream>
#include "Highway.h"

struct tratto {// creo una struttura per rappresentare un tratto di strada con velocità media e inizio intervallo
		double inizio_intervallo_;
		double velocita_media_;
		friend std::ostream& operator<<(std::ostream& os, const tratto& t) {
			os << t.velocita_media_ << " " << t.inizio_intervallo_ << ", ";
			return os;
		}
};

struct Auto { // creo una struttura per rappresentare un'auto con targa, ingresso, uscita, orario di ingresso e profilo di velocità 
	std::string targa_;
	int ingresso_;
	int uscita_;
	double data_orario_ingresso_;
	std::vector<tratto> profilo_velocita_;

	static bool isTargaValid(std::string targa) { // funzione per controllare se la targa è valida nel fomato AZ000AZ
		if (targa.length() != 7) return false;
		for (int i = 0; i < targa.length(); ++i) {
			if (i < 2 || i > 4) {
				if (!isalpha(targa[i])) return false;
			}
			else {
				if (!isdigit(targa[i])) return false;
			}
		}
		return true;
	}

	Auto(const std::string& targa, int ingresso, int uscita, double data_orario_ingresso, double distanzaPercorsa) : // costruttore che inizializza i membri e genera il profilo di velocità
		targa_{ targa },
		ingresso_{ ingresso },
		uscita_{ uscita },
		data_orario_ingresso_{ data_orario_ingresso } {

		if (!isTargaValid(targa))
			throw std::invalid_argument("Targa deve essere lunga 7 e nel formato AZ000AZ."); // controllo validità targa

		if (ingresso < 0 || uscita < 0 || ingresso > uscita)
			throw std::invalid_argument("Ingresso e/o uscita non hanno senso."); // controllo validità ingresso e uscita

		if (data_orario_ingresso < 0)
			throw std::invalid_argument("Orario deve essere positivo."); // controllo validità orario ingresso

		double distanza = 0;
		double velocity = 80 + (std::rand() % (190 - 80 + 1));
		double time = data_orario_ingresso;
		while(distanza <= distanzaPercorsa) {
			profilo_velocita_.push_back({ time , velocity });
			time += (5 + (std::rand() % (15 - 5 + 1))) * 60;
			double delta = time - profilo_velocita_.back().inizio_intervallo_;
			distanza += (profilo_velocita_.back().velocita_media_ * (delta / 3600.0));
			velocity = 80 + (std::rand() % (190 - 80 + 1));
		}// generazione del profilo di velocità
	};
	
	friend std::ostream& operator<<(std::ostream& os, const Auto& a) { // overload dell'operatore di output per stampare i membri dell'auto, utilizzato per facilitare la scrittura sul file
		os << a.targa_ << " " << a.ingresso_ << " " << a.uscita_ << " " << a.data_orario_ingresso_ << " ";
		for (tratto t : a.profilo_velocita_)
			os << t;
		return os;
	}
};


/* funzione per generare una targa casuale se non inserito alcun parametro (oppure -1) con probabilità teorica 
	di generare 2 targhe identiche = 2,19*10^-9 %); ALtrimenti restituisce una targa basata sull'indice passato come parametro */
static std::string generateTarga(int index=-1) { 
	std::string targa;
	if (index==-1){
		targa += static_cast<char>('A' + (std::rand() % 26));
		targa += static_cast<char>('A' + (std::rand() % 26));
		targa += std::to_string(std::rand() % 10);
		targa += std::to_string(std::rand() % 10);
		targa += std::to_string(std::rand() % 10);
		targa += static_cast<char>('A' + (std::rand() % 26));
		targa += static_cast<char>('A' + (std::rand() % 26));
	}
	else {
		for (int i=0;i<7;i++){
			if ( i >= 2 && i <= 4 ) {
				targa += std::to_string(index % 10);
				index /= 10;
				continue;
			}
			else {
				targa += static_cast<char>('A' + (index % 26));
				index /= 26;
			}
		}
	}
	return targa;
}

int main() {
	try {
		const int NUM_AUTO = 10000; // numero di auto da simulare 

		std::srand(std::time(0)); // inizializzo il generatore di numeri casuali con il tempo attuale

		std::vector<Station> Svincoli;
		std::vector<Station> Varchi; // creo due vector per svincoli e varchi per necessito di accesso casuale agli elementi 

		std::string myText;
		std::string pathHighway = std::string(PROJECT_SOURCE_DIR) + "/data/Highway.txt"; // costruisco il path del file Highway.txt
		
		Highway highway(pathHighway); // utilizzo la classe Highway per caricare e validare i dati
		std::cout << "File Highway aperto correttamente ... Procedo con lettura\n";
		Varchi = highway.getGates();
		Svincoli = highway.getJunctions();
		
		std::cout << "lettura completata... creazione file Runs.txt\n";
		std::string pathRuns = std::string(PROJECT_SOURCE_DIR) + "/data/Runs.txt"; // costruisco il path del file Runs.txt
		std::ofstream RunsFile(pathRuns); // apro il file in scrittura
		if (!RunsFile.is_open()) { // controllo che il file sia stato aperto correttamente
			throw std::runtime_error("Impossibile aprire il file Runs.txt");
		}
		std::cout << "File creato correttamente...\ngenerazione macchine\n";
		int autoGenerate = 0;
		double orario = 0;
		while (autoGenerate++ < NUM_AUTO) {
			int ingressoIndex = std::rand() % (Svincoli.size() - 1);
			int uscitaIndex = std::rand() % (Svincoli.size() - ingressoIndex - 1) + ingressoIndex + 1;
			double distanzaPercorsa = Svincoli[uscitaIndex].position - Svincoli[ingressoIndex].position;
			Auto a{ generateTarga(autoGenerate),ingressoIndex,uscitaIndex,orario,distanzaPercorsa };
			orario += ((std::rand() % 95) + 5) / 10.0;
			RunsFile << a << std::endl;
		}
		std::cout << "Simulazione completata... Procedo con la simulazione dei passaggi nei varchi\n";

		//HighwayFile.close();
		RunsFile.close();

		std::ifstream RunsFileRead(pathRuns);
		if (!RunsFileRead.is_open()) { // controllo che il file sia stato aperto correttamente
			throw std::runtime_error("Impossibile aprire il file Runs.txt");
		}

		std::string pathPassages = std::string(PROJECT_SOURCE_DIR) + "/data/Passages.txt"; // costruisco il path del file Passages.txt
		std::ofstream PassagesFile(pathPassages);
		if (!PassagesFile.is_open()) { // controllo che il file sia stato aperto correttamente
			throw std::runtime_error("Impossibile aprire il file Runs.txt");
		}
		while (std::getline(RunsFileRead, myText)) {
			std::stringstream run(myText);
			std::string word;
			int index = 0;
			double distanzaPercorsa = 0;
			double velocita_media = 0;
			double inizio_intervallo = 0;
			double velocita_precedente = 0;
			double inizio_intervallo_precedente = 0;
			std::string targa;
			int ingresso;
			while (run >> word) {
				if (index == 0)
					targa = word;
				if (index == 1)
					ingresso = std::stoi(word);
				if (index == 2)
					int uscita = std::stoi(word);
				if (index == 3)
					double orario_ingresso = std::stod(word);
				if (index > 4) {
					velocita_precedente = velocita_media;
					inizio_intervallo_precedente = inizio_intervallo;
				}
				if (index > 3) {
					velocita_media = std::stod(word);
					run >> word;
					word = word.substr(0, word.length() - 1);
					inizio_intervallo = std::stod(word);
				}
				if (index > 4) {
					double delta_km = (velocita_precedente * ((inizio_intervallo - inizio_intervallo_precedente) / 3600.0));
					distanzaPercorsa += delta_km;
					for (int v = 0; v < Varchi.size(); v++) {
						if (distanzaPercorsa + Svincoli[ingresso].position >= Varchi[v].position && distanzaPercorsa + Svincoli[ingresso].position - delta_km < Varchi[v].position) { // se l'auto ha superato il varco in questo intervallo
							double istante_passaggio = inizio_intervallo_precedente + (((Varchi[v].position - (distanzaPercorsa - delta_km + Svincoli[ingresso].position)) / velocita_precedente) * 3600.0);
							PassagesFile << v << " " << targa << " " << istante_passaggio << '\n';
						}
					}
				}
				index++;
			}
		}

		std::cout << "Simulazione completata con successo\n";

		RunsFileRead.close();
	}
	catch (const std::exception& e) {
		std::cerr << "Errore: " << e.what() << std::endl;
		return -1;
	}
	return 0;
}
