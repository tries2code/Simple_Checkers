#include"Graph.h"
#include"Simple_window.h"

//Button unter schwarzen Felder eingef�gt, die geben Position wieder
//Spielsteine richtig angeordnet
//Spielsteine k�nnen ausgew�hlt und manipulirt werden 
//tile_pressed ist f�r die Spielmechanik, zurzeit im Aufbau
//Group Klasse mit ls und us ersetzt


//Staus Spielmechanik:
//Spieler 1 und 2 k�nnen nur abwechselnd spielen
//Spielsteine k�nnen nicht r�ckw�rts gehen
//Spielsteine k�nnen nur ein Feld weit laufen
//oben genanntes kann nat�rlich auf Grund von bisher nicht gefundenen Bugs gelogen sein

//Es fehlt noch:
//Mechanik zum gegenerische Steine fangen
//Mechanik f�r Multikills
//Trasformation eines Spielsteins zur Dame mit entsprechenden Attributen
//Tests Tests Tests....



constexpr int sz = 50;									//Feld L�nge und Breite; vor�bergehend global
constexpr int ca = sz / 2;								//Anpassung f�r Kreise, da die sonst oben links auf einem Feld stehen; vor�bergehend global
constexpr int ra = 20;									//Radius der Spielsteine; vor�bergehend global

constexpr int ls = 100;									//Linker Abstand zum Bildschirmrand
constexpr int us = 100;									//Oberer Absatnd zum Bilschirmrand

const Color c_player1 = Color::red;
const Color c_player2 = Color::yellow;
Color c_turn = c_player1;								//Der Spieler der grade dran ist

Graph_lib::Circle* curr_stone = nullptr;

//Helfer Funktionen
Point get_point(int x, int y) {							//Ermittelt Koordinaten f�r Spielfeld
	Point res{ x - x % sz,y - y % sz };
	return res;
}
bool operator==(const Graph_lib::Color& a, const Graph_lib::Color& b) {				//Vergleicht Farben
	if (a.as_int() == b.as_int())return true;
	return false;
}



//Fenster Klasse; ist noch ein ziemliches Chaos...
class My_window : public Window {

	bool button_pushed;
	bool stone_selected;								//Wurde ein Stein ausgew�hlt?
	Button next_button;
	Button quit_button;

	Out_box current_turn;								//Sagt einem wer grad dran ist, ja, ist h�sslich, kann man �berarbeiten

	Vector_ref<Button>field_buttons;					//F�r die Kn�ppe unter den schwarzen Feldern
	Vector_ref< Graph_lib::Rectangle>checkers;			//F�r die Felder
	Vector_ref< Graph_lib::Circle>stones;				//F�r die Spielsteine, sind nur ausgemalte Kreise

public:
	My_window(Point xy, int w, int h, const string& title) :Window(w, h, title),
		button_pushed(false), stone_selected(false),
		next_button(Point(x_max() - 70, 0), 70, 20, "Next", cb_next),
		quit_button(Point(x_max() - 70, 30), 70, 20, "Quit", cb_quit),
		current_turn(Point{ x_max() - 350, 100 }, 100, 30, "Current Player: ")
	{
		for (int x = 0; x < 8; x++) {					//Kn�ppe unter den Feldern
			for (int y = 0; y < 8; y++) {
				if ((y + x) % 2 == 0)field_buttons.push_back(new Button{ {ls + x * sz,us + y * sz},sz,sz ," ",cb_tile_pressed });
				attach(field_buttons[field_buttons.size() - 1]);
			}
		}
		for (int x = 0; x < 8; x++) {					//Felder
			for (int y = 0; y < 8; y++) {
				checkers.push_back(new Graph_lib::Rectangle{ {ls + x * sz,us + y * sz},sz,sz });
				if ((y + x) % 2 == 0)checkers[checkers.size() - 1].set_fill_color(FL_BLACK);
				attach(checkers[checkers.size() - 1]);
			}
		}
		for (int x = 0; x < 8; x++) {					//Steine
			for (int y = 0; y < 8; y++) {
				if ((y + x) % 2 == 0)stones.push_back(new Graph_lib::Circle{ {ls + x * sz + ca,us + y * sz + ca},ra });
				attach(stones[stones.size() - 1]);
				if (y == 1)y = 5;
			}
		}
		for (int i = 0; i < stones.size(); i++) {		//Farbe der Steine, die Kondition if ((y + x) % 2 == 0) im oberen Steine-Loop hat bei der Einf�rbung irgendwie zu Schwulit�ten gef�hrt...keine Ahnung warum
			if (i % 2 == 0)stones[i].set_fill_color(c_player2);
			else stones[i].set_fill_color(c_player1);
		}
		attach(next_button);
		attach(quit_button);
		attach(current_turn);
		current_turn.put("Red");						//rot f�ngt an
	}
	void wait_for_button() {
		while (!button_pushed) Fl::wait();
		button_pushed = false;
		Fl::redraw();
	}

private:
	bool tile_empty(Point p) {
		Point curr = { p.x + ca,p.y + ca };
		for (int i = 0; i < stones.size(); i++) {
			if (stones[i].center() == curr)return false;
		}
		return true;
	}
	Graph_lib::Circle* get_stone(Point p) {				//Ermittelt Spielstein
		Graph_lib::Circle* res;
		Point curr;
		for (int i = 0; i < stones.size(); i++) {
			curr = stones[i].center();
			if (curr == Point{ p.x + ca,p.y + ca }) {
				res = &stones[i];
				return res;
			}
		}
		return nullptr;
	}


	static void cb_next(Address, Address addr) { static_cast<My_window*>(addr)->next(); }		//Sogenannte Call Back Funktionen f�r Kn�pfe, callen die eigentlichen Funktionen f�r die Kn�pfe
	static void cb_quit(Address, Address addr) { reference_to<My_window>(addr).quit(); }		//Ist die Stroustrup Variante, macht auch nur 		return *static_cast<W*>(pw);
	static void cb_tile_pressed(Address, Address addr) { reference_to<My_window>(addr).tile_pressed(); }

	void next() { button_pushed = true; }								//L�st FL::redraw() (in void wait_for_button()) aus.
	void quit() { hide(); button_pushed = true; }

	void  tile_pressed() {
		Point p = get_point(Fl::event_x(), Fl::event_y());				//p=Koordinaten der aktuell gedr�cketen Feldes

		if (!tile_empty(p) && !stone_selected) {
			curr_stone = get_stone(p);
			Color c_curr = curr_stone->fill_color();
			if (c_curr == c_turn) {
				curr_stone->set_color(Color::cyan);
				stone_selected = true;
			}
		}

		if (tile_empty(p) && stone_selected) {
			Point pp = curr_stone->center();
			Point diff = { p.x - pp.x,p.y - pp.y };

			if (c_turn == c_player1 && p.y < pp.y && abs(diff.x) <= sz + ca && abs(diff.y) <= sz + ca && diff.y + ca != 0) {					//Bewegung rot
				curr_stone->move(diff.x + ca, diff.y + ca);
				curr_stone->set_color(Color::black);
				curr_stone = nullptr;
				stone_selected = false;
				c_turn = c_player2;
				current_turn.put("Yellow");
			}

			if (c_turn == c_player2 && p.y > pp.y && abs(diff.x) <= sz + ca && abs(diff.y) <= sz - ca) {	//Bewegung gelb
				curr_stone->move(diff.x + ca, diff.y + ca);
				curr_stone->set_color(Color::black);
				curr_stone = nullptr;
				stone_selected = false;
				c_turn = c_player1;
				current_turn.put("Red");
			}
		}
		cout << p.x << " " << p.y << endl;					//Ist nur f�rs debuggen
		Fl::redraw();
	}
};

int main() {

	//My_window win{ {100,100},x_max(),y_max(),"Schach oder Dame" };
	My_window win{ {100,100},1000,800,"Schach oder Dame" };						//Fenster zum Debuggen verkleinert um das Konsolenfenster zu sehen


	win.wait_for_button();
}