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

struct Nodo { // struttura per rappresentare un VARCO oppure uno SVINCOLO e la distanza in km dalla partenza
	double km_;
	char tipo_;

	Nodo(double km, char tipo) : km_{ km }, tipo_{ tipo } {
		if (tipo != 'V' && tipo != 'S')
			throw std::invalid_argument("Tipo deve essere V o S.");
	}

	bool operator<(const Nodo& other) const { // overload dell'operatore di confronto per la priority queue, in modo da avere un min heap
		return km_ < other.km_;
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
	const int NUM_AUTO = 1000; // numero di auto da simulare 

	std::srand(std::time(0)); // inizializzo il generatore di numeri casuali con il tempo attuale
	
	std::vector<Nodo> Svincoli;
	std::vector<Nodo> Varchi; // creo due vector per svincoli e varchi per necessito di accesso casuale agli elementi 

	std::string myText;
	std::string pathHighway = std::string(PROJECT_SOURCE_DIR) + "/data/Highway.txt"; // costruisco il path del file Highway.txt

	std::ifstream HighwayFile(pathHighway); // apro il file in lettura
	if (!HighwayFile.is_open()) { // controllo che il file sia stato aperto correttamente
		throw std::runtime_error("Impossibile aprire il file Highway.txt");
	} 
	std::cout << "File aperto correttamente... Procedo con lettura\n";
	while (std::getline(HighwayFile, myText)) {
		char type = myText.back();
		myText.pop_back(); // rimuovo l'ultimo carattere della riga per ottenere il tipo (V o S)
		myText.pop_back(); // rimuovo lo spazio prima del tipo
		double numero;
		try {
			numero = std::stod(myText);
		}
		catch (const std::invalid_argument& invalid_argument) {
			throw std::invalid_argument("Errore: Nel file è presente un argomento non valido per la conversione ad double.");
		}
		catch (const std::out_of_range& out_of_range) {
			throw std::invalid_argument("Errore: Il numero è fuori dal range di double.");
		}

		if (type == 'V') {
			Varchi.push_back(Nodo(numero, type));
		}
		else if (type == 'S') {
			Svincoli.push_back(Nodo(numero, type));
		}
		else {
			throw std::invalid_argument("Errore: Tipo non valido nel file. Deve essere V o S.");
		}
	} // leggo il file riga per riga e popolo le code di priorità

	std::cout << "File letto correttamente... Numero varchi = "<< Varchi.size() << "; Numero Svincoli = "<< Svincoli.size() <<"; ... Procedo con verifica dei dati\n";
	std::sort(Svincoli.begin(), Svincoli.end());
	std::sort(Varchi.begin(), Varchi.end()); // ordino i vettori per km in modo crescente, (la numerazione dei varchi e svincoli è implicita nella posizione nel vettore ordinato)
	int cV = 0;
	int cS = 0;
	if (Varchi.size() < 2) throw std::invalid_argument("Errore: Devono esserci almeno 2 varchi.");

	if (Varchi.at(0).km_ < Svincoli.at(0).km_) throw std::invalid_argument("Errore: Il primo varco deve essere dopo il primo svincolo.");

	if (Varchi.back().km_ > Svincoli.back().km_) throw std::invalid_argument("Errore: L'ultimo varco deve essere prima dell'ultimo svincolo.");

	while (cV < Varchi.size()) {
		if (abs(Svincoli.at(cS).km_ - Varchi.at(cV).km_) < 1) throw std::invalid_argument("Errore: Esiste uno svincolo e un varco con distanza inferiore ad 1 km.");
		if (Svincoli.at(cS).km_ < Varchi.at(cV).km_)	cS++; else cV++;
	}// verifico che non esistano svincoli e varchi con distanza inferiore ad 1 km
	
	std::cout << "Verifica completata... Procedo con la simulazione:\n";
	std::cout << "1 fase: creazione file Runs.txt...\n";
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
		double distanzaPercorsa = Svincoli[uscitaIndex].km_ - Svincoli[ingressoIndex].km_;
		Auto a{ generateTarga(autoGenerate),ingressoIndex,uscitaIndex,orario,distanzaPercorsa };
		orario += ((std::rand() % 95) + 5) / 10.0;
		RunsFile << a << std::endl;
	}
	std::cout << "Simulazione completata... Procedo con la simulazione dei passaggi nei varchi\n";

	HighwayFile.close();
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
			if (index>4) {
				double delta_km = (velocita_precedente * ((inizio_intervallo - inizio_intervallo_precedente) / 3600.0));
				distanzaPercorsa += delta_km;
				for (int v = 0; v < Varchi.size(); v++) {
					if (distanzaPercorsa + Svincoli[ingresso].km_ >= Varchi[v].km_ && distanzaPercorsa + Svincoli[ingresso].km_ - delta_km < Varchi[v].km_) { // se l'auto ha superato il varco in questo intervallo
						double istante_passaggio = inizio_intervallo_precedente + (((Varchi[v].km_ - (distanzaPercorsa - delta_km + Svincoli[ingresso].km_)) / velocita_precedente) * 3600.0);
						PassagesFile << v << " " << targa << " " << istante_passaggio << '\n';
					}	
				}
			}
			index++;
		}
	}
	
	std::cout << "Simulazione completata con successo\n";

	RunsFileRead.close();
	
	return 0;
}
