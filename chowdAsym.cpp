#include "stdafx.h"
#include "chowdAsym.h"
#include <fstream>
//hgy
using namespace std;

class automat {
	class pojazd;
	double W;	//Dlugosc drogi w metrach
	double ceil_len; //Długość komórki w metrach
	int numb_ceil; //ilość komórek 
	int cars;	//liczba samochodow na drodze
	bool isChecked; //flaga , sprawdzamy czy dany pojazd byl aktualizowany podczas zmiany pasa
	int sumaV; //suma predkosci
	double sredniaV; // srednia
	double density;
	int iterations;
	pojazd **vehicles;
public:
	class pojazd
	{
	public:
		class element {
		public:
			pojazd* parent;
			int index;
		
			element(pojazd *p, int i) {
				parent = p;
				index = i;
			}
			void wyswietl()
			{
				cout << parent->V; //edit !
			}
		};

	public:
		int V; //aktualna predkosc pojazdu
		int Vmax;// maksymalna predkosc pojazdu 
		bool checked;// flaga sprawdzamy czy dany pojazd byl aktualizowany podczas zmiany pasa 
		bool is;		// czy wszystkie elementy pojazdu są już na drodze
		int carLen; //dlugosc pojazdu
		element** children;

		pojazd(double ceil_len)
		{
			Vmax = rand() % 3 + 1; // losowanie max predkosc w przedziale od 1 - 2
			carLen = static_cast<int>(ceil(static_cast<double>(rand() % 3 + 2) / ceil_len)); // losowanie dlugosci pojazu w metrach zamienione na komorki
			children = new element*[carLen];
			for (int i = 0; i < carLen; i++) {
				children[i] = new element(this, i);
			}
			this->V = rand() % Vmax + 1; //losowanie aktualnej predkosci
			//this->checked = !isChecked;
			this->is = false;

			//road = children[0];

		}
		void speedup() {
			if (V < Vmax) {
				V++;
			}
		}
		void speeddown() {
			if (V > 1) {
				V--;
			}
		}
	};
	typedef pojazd::element element;
	automat(double dens = 0.5, double nCL = 7.5, int nCars=50) {
		ceil_len = nCL;
		cars = nCars;
		density = dens;
		vehicles = new pojazd*[cars];

		int capacity = 0;
		for (int i = 0; i < cars; i++) {
			vehicles[i] = new pojazd(ceil_len);
			capacity += vehicles[i]->carLen;
		}
		//W = static_cast<double>(capacity) / density;

		//numb_ceil = static_cast<int>(ceil(W / ceil_len));
		numb_ceil = ceil(static_cast<double>(capacity) / density);
		W = numb_ceil * ceil_len;
		/*cout << W << " W" << endl;
		cout << numb_ceil << " numb-ceil" << endl;
		cout << ceil_len << " ceil_len" << endl;
		cout << density << " density" << endl;
		*/
		sumaV = 0;
		sredniaV = 0.0;
		iterations = 0;
		isChecked = false;
	}

	void automat_start() {
		srand(static_cast<unsigned int>(time(NULL)));
		
		element ***road = new element**[2];
		road[0] = new element*[numb_ceil];
		road[1] = new element*[numb_ceil];
		for (int i = 0; i < 2; i++) { for (int j = 0; j < numb_ceil; j++) { road[i][j] = NULL; } }
		
		int nr = 0;	// numer pojazdu
		int flaga = 0;// odstęp między wypuszczeniem kolejnego pojazdu
		//
		int pos = 0;
		for (int i = 0; i < cars; i++) {
			vehicles[i]->checked = !isChecked;
			for (int posx = vehicles[i]->carLen-1; posx >= 0; posx--) {
				road[1][pos] = vehicles[i]->children[posx];
				pos++;
			}
			pos++;
		}
		int maxIterations = 30;
		while (maxIterations > 0) {
			//wyswietlanie(road);
			odswiezanie(road);
			//Sleep(300);
			maxIterations--;
		}

		/*
		while (nr<cars) {
			/*
			// jeżeli jest wolne miejsce na początku 2-go pasa
			if (road[1][0] == NULL) {
				if (flaga == 0)
				{
					flaga = 4;
					generator(road, nr);
					nr++;
				}
				else flaga--;
			}

			wyswietlanie(road);
			odswiezanie(road);
			Sleep(300);
			
		}
		*/
		delete[]road;
		return;
	}
	void srednia()
	{
		ofstream wyniki;
		wyniki.open("wyniki.txt", ios::out | ios::app);
		double flow = 0.0;
		sredniaV = static_cast<double>(sumaV) / (30.0*cars);
		cout << sredniaV << " " << numb_ceil << endl;
		flow = density * sredniaV;
		wyniki << density << ";" << flow << endl;
		wyniki.close();
	}
	void generator(element ***&road, int id)
	{
		//new pojazd(isChecked, ceil_len, road[1][0]);
		vehicles[id]->checked = !isChecked;
		road[1][0] = vehicles[id]->children[0];
	}

void wyswietlanie(element ***&d) {
	system("cls");
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < numb_ceil; j++) {
			if (d[i][j] == NULL)
			{
				cout << '-';
			}
			else {
				d[i][j]->wyswietl();
			}
		}
		cout << endl;
	}

}
// Zlicza gapy - odleglosc luki przed pojazdem
int gap(element ***d, int i, int j) {
	element *t = d[i][j];
	int g = 0;
	//while (j + g + 1 < numb_ceil && d[i][j + 1 + g] == NULL) { g++; }
	int pos = j + 1;
	if (pos > numb_ceil) pos = 0;
	while (d[i][pos] == NULL) {
		g++;
		pos++;
		if (pos > numb_ceil) pos = 0;
	}
	//
	return g;
}
// Zlicza gapy na drugim pasie - odleglosc luki do tylu
bool gap_back(element ***d, int i, int j) {
	element *t = d[i][j];
	int g = 0;
	//
	int pos = j - g - 1;
	if (pos < 0) pos = numb_ceil - 1;
	//while (j - g - 1 >= 0 && d[i][j - g - 1] == NULL) { g++; }
	while (d[i][pos] == NULL) {
		g++;
		pos--;
		if (pos < 0) {
			pos = numb_ceil - 1;
		}
	}
	//if (j == g) return true;// brak pojazdow na pasie
	//if (d[i][j - g - 1]->parent->Vmax < g) return true;
	if (d[i][pos]->parent->Vmax < g) return true;
	return false;
}

void odswiezanie(element ***&d)
{
	element *t;
	pojazd *p;
	int x, y;
	/*zmiana pasa*/
	
	for (int i = 0, l = 1; i < 2; i++, l--) {
		for (int j = 0; j < numb_ceil; j++) {
			t = d[i][j];
			x = y = 0;
			if (t != NULL && t->parent->checked != isChecked)
			{
				p = t->parent;
				p->checked = isChecked; //zmiana co cykl flagi
				x = gap(d, i, j + t->index); // x - dlugosc luki
				bool empty = true;

				if (x < p->V + 1 || i==0) {
					for (int k = 0; k < t->index + 1; k++) {
						if (d[l][j + k] != NULL) {
							empty = false;
						}
					}
					if (empty)
					{
						d[l][j] = t; //przeniesienie ogona
						if (gap_back(d, l, j)) {
							d[i][j] = NULL;
							for (int k = 1; k < t->index + 1; k++) {
								d[l][j + k] = d[i][j + k];
								d[i][j + k] = NULL;
							}
						}
						else {
							d[l][j] = NULL;
						}
					}
				}
			}
		}
	}
	isChecked = !isChecked;
	
	element ***tmp = new element**[2];
	tmp[0] = new element*[numb_ceil];
	tmp[1] = new element*[numb_ceil];
	for (int i = 0; i < 2; i++) { for (int j = 0; j < numb_ceil; j++) { tmp[i][j] = NULL; } }

	//implementacja asymetryczny chowdhury

	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < numb_ceil; j++)
		{
			t = d[i][j];
			if (t != NULL)
			{
				p = t->parent;
				if (p->checked != isChecked) {
					p->checked = isChecked;
					y = gap(d, i, j + t->index);// y - dlugosc luki
					if (y > p->V) { p->speedup(); }
					else if (y > 0) { p->V = y; }
					else { p->V = 1; }
					if (rand() % 100 < 25) { p->speeddown(); }//prawdopodobienstwo hamowania
					sumaV += p->V;
				}
				x = j + p->V; //przyszla pozycja
				//if (x < numb_ceil)
				if (x >= numb_ceil) x = x - numb_ceil;

				//{
					tmp[i][x] = t;
					//wstawiamy pominiete wczesniej 'elementy' pojazdu
					/*
					if (p->is == false)
					{
						for (int k = 1; t->index + k < p->carLen; k++) {
							if (x - k >= 0) {
								tmp[i][x - k] = p->children[t->index + k];
								if (tmp[1][x - k] == p->children[p->carLen - 1]) {
									p->is = true;
								}
							}
						}
						
					}
					*/
				//}
				//else { delete t; }
				//d[i][j] = NULL;
			}
		}
	}
	delete[]d;
	d = tmp;
	tmp = NULL;
	isChecked = !isChecked;
}

};

int main(int argc, char *argv[])
{
	//cout << argc << endl;
	if (argc < 4) {
		cout << "brak argumentow" << endl;
		return 0;
	}
	automat* automat1 = new automat(atof(argv[1]), atof(argv[2]), atoi(argv[3]));
	automat1->automat_start();
	automat1->srednia();
	delete automat1;
	cout << endl;
	//system("PAUSE");
	return 0;
}
