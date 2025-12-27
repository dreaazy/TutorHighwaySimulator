//Author: Manuel Carelli
#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>

struct tratto {
		double inizio_intervallo_;
		double velocita_media_;
		friend std::ostream& operator<<(std::ostream& os, const tratto& t) {
			os << t.velocita_media_ << " " << t.inizio_intervallo_ << ", ";
			return os;
		}
};

struct Auto {
	std::string targa_;
	int ingresso_;
	int uscita_;
	double data_orario_ingresso_;
	std::vector<tratto> profilo_velocita_;
	static bool isTargaValid(std::string targa) {
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

	Auto(const std::string& targa, int ingresso, int uscita, double data_orario_ingresso, int distanzaPercorsa) :
		targa_{ targa },
		ingresso_{ ingresso },
		uscita_{ uscita },
		data_orario_ingresso_{ data_orario_ingresso } {
		if (!isTargaValid(targa))
			throw std::invalid_argument("Targa deve essere lunga 7 e nel formato AZ000AZ.");
		if (ingresso < 0 || uscita < 0 || ingresso > uscita)
			throw std::invalid_argument("Ingresso e/o uscita non hanno senso.");
		if (data_orario_ingresso < 0)
			throw std::invalid_argument("Orario deve essere positivo.");
		double distanza = 0;
		double velocity = 80 + (std::rand() % (190 - 80 + 1));
		double time = data_orario_ingresso;
		while(distanza <= distanzaPercorsa) {
			profilo_velocita_.push_back({ time , velocity });
			velocity = 80 + (std::rand() % (190 - 80 + 1));
			time += (5 + (std::rand() % (15 - 5 + 1))) * 60;
			double delta = time - profilo_velocita_.end()->inizio_intervallo_;
			distanza += (profilo_velocita_.end()->velocita_media_ * (delta / 3600.0));
		}
	};
	
	friend std::ostream& operator<<(std::ostream& os, const Auto& a) {
		os << a.targa_ << " " << a.ingresso_ << " " << a.uscita_ << " " << a.data_orario_ingresso_ << " ";
		for (tratto t : a.profilo_velocita_)
			os << t;
		return os;
	}
};

int main() {
	std::srand(std::time(0));
	Auto a{ "AB123CD", 1, 5, 3600.0, 100 };
	std::cout << a << std::endl;
	
	return 0;
}
