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
		while (distanza <= distanzaPercorsa) {
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
static std::string generateTarga(int index = -1) {
	std::string targa;
	if (index == -1) {
		targa += static_cast<char>('A' + (std::rand() % 26));
		targa += static_cast<char>('A' + (std::rand() % 26));
		targa += std::to_string(std::rand() % 10);
		targa += std::to_string(std::rand() % 10);
		targa += std::to_string(std::rand() % 10);
		targa += static_cast<char>('A' + (std::rand() % 26));
		targa += static_cast<char>('A' + (std::rand() % 26));
	}
	else {
		for (int i = 0;i < 7;i++) {
			if (i >= 2 && i <= 4) {
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