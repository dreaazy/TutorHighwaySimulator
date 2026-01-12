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
#include "simulatore.h"



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
