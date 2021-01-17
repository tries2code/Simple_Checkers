#include"Graph.h"
#include"Simple_window.h"
#include <FL/fl_ask.H>
#undef vector


//Button unter schwarzen Felder eingefügt, die geben Position wieder
//Spielsteine richtig angeordnet
//Spielsteine können ausgewählt und manipulirt werden 
//tile_pressed ist für die Spielmechanik, zurzeit im Aufbau
//Group Klasse mit ls und us ersetzt


//Staus Spielmechanik:
//Spieler 1 und 2 können nur abwechselnd spielen
//Spielsteine können nicht rückwärts gehen
//Spielsteine können nur ein Feld weit laufen
//oben genanntes kann natürlich auf Grund von bisher nicht gefundenen Bugs gelogen sein
//Mechanik zum gegenerische Steine fangen
//Mechanik für Multikills

//Es fehlt noch:
//Trasformation eines Spielsteins zur Dame mit entsprechenden Attributen
//Tests Tests Tests....



constexpr int sz = 50;									//Feld Länge und Breite; vorübergehend global
constexpr int ca = sz / 2;								//Anpassung für Kreise, da die sonst oben links auf einem Feld stehen; vorübergehend global
constexpr int ra = 20;									//Radius der Spielsteine; vorübergehend global

constexpr int ls = 100;									//Linker Abstand zum Bildschirmrand
constexpr int us = 100;									//Oberer Absatnd zum Bilschirmrand

const Color c_player1 = Color::red;
const Color c_player2 = Color::yellow;
Color c_turn = c_player1;								//Der Spieler der grade dran ist

Graph_lib::Circle* curr_stone = nullptr;

//Helfer Funktionen
Point get_point(int x, int y) {							//Ermittelt Koordinaten für Spielfeld
	Point res{ x - x % sz,y - y % sz };
	return res;
}
bool operator==(const Graph_lib::Color& a, const Graph_lib::Color& b) {				//Vergleicht Farben
	if (a.as_int() == b.as_int())return true;
	return false;
}
bool operator!=(const Graph_lib::Color& a, const Graph_lib::Color& b) {				//Vergleicht Farben
	if (!operator==(a, b))return true;
	return false;
}

//Fenster Klasse; ist noch ein ziemliches Chaos...
class My_window : public Window {

	bool button_pushed;
	bool stone_selected;								//Wurde ein Stein ausgewählt?
	Button next_button;
	Button quit_button;

	int countRed = 8;
	int countYellow = 8;

	Out_box current_turn;								//Sagt einem wer grad dran ist, ja, ist hässlich, kann man überarbeiten

	Vector_ref<Button>field_buttons;					//Für die Knöppe unter den schwarzen Feldern
	Vector_ref< Graph_lib::Rectangle>checkers;			//Für die Felder
	Vector_ref< Graph_lib::Circle>stones;				//Für die Spielsteine, sind nur ausgemalte Kreise

public:
	My_window(Point xy, int w, int h, const string& title) :Window(w, h, title),
		button_pushed(false), stone_selected(false),
		next_button(Point(x_max() - 70, 0), 70, 20, "Next", cb_next),
		quit_button(Point(x_max() - 70, 30), 70, 20, "Quit", cb_quit),
		current_turn(Point{ x_max() - 350, 100 }, 100, 30, "Current Player: ")
	{
		for (int x = 0; x < 8; x++) {					//Knöppe unter den Feldern
			for (int y = 0; y < 8; y++) {
				if ((y + x) % 2 == 0)field_buttons.push_back(new Button{ {ls + x * sz,us + y * sz},sz,sz ," ",cb_tile_pressed });
				attach(field_buttons[field_buttons.size() - 1]);
			}
		}
		for (int x = 0; x < 8; x++) {					//Felder
			for (int y = 0; y < 8; y++) {
				checkers.push_back(new Graph_lib::Rectangle{ {ls + x * sz,us + y * sz},sz,sz });
				if ((y + x) % 2 == 0)checkers[checkers.size() - 1].set_fill_color(Color::black);
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
		for (int i = 0; i < stones.size(); i++) {		//Farbe der Steine, die Kondition if ((y + x) % 2 == 0) im oberen Steine-Loop hat bei der Einfärbung irgendwie zu Schwulitäten geführt...keine Ahnung warum
			if (i % 2 == 0)stones[i].set_fill_color(c_player2);
			else stones[i].set_fill_color(c_player1);
		}
		attach(next_button);
		attach(quit_button);
		attach(current_turn);
		current_turn.put("Red");						//rot fängt an
	}
	void wait_for_button() {

		while (!button_pushed) Fl::wait();
		button_pushed = false;
		Fl::redraw();
	}

private:
	bool tile_empty(Point p)const {
		Point curr{ p.x + ca,p.y + ca };
		if (p.x < ls || p.x >= ls + 8 * sz || p.y < us || p.y >= us + 8 * sz)return false;
		for (int i = 0; i < stones.size(); i++) {
			if (stones[i].center() == curr)return false;
		}
		return true;
	}
	bool hostile_present(Point p) const {
		if (stone_selected && tile_empty(p))return false;
		Point curr{ p.x + ca,p.y + ca };
		for (int i = 0; i < stones.size(); i++)
			if (stones[i].center() == curr && stones[i].fill_color() != c_turn)return true;

		return false;
	}
	bool must_attack(Point p) const {
		if ((hostile_present({ p.x + sz,p.y + sz }) && tile_empty({ p.x + sz * 2,p.y + sz * 2 })) ||
			(hostile_present({ p.x - sz,p.y - sz }) && tile_empty({ p.x - sz * 2,p.y - sz * 2 })) ||
			(hostile_present({ p.x + sz,p.y - sz }) && tile_empty({ p.x + sz * 2,p.y - sz * 2 })) ||
			(hostile_present({ p.x - sz,p.y + sz }) && tile_empty({ p.x - sz * 2,p.y + sz * 2 })))return true;
		return false;
	}
	vector<Point> strike() {
		Point stone{ curr_stone->center().x - ca, curr_stone->center().y - ca };
		vector<Point> res;
		if (hostile_present({ stone.x + sz,stone.y + sz }) && tile_empty({ stone.x + sz * 2,stone.y + sz * 2 }))res.push_back({ stone.x + sz * 2,stone.y + sz * 2 });
		if (hostile_present({ stone.x - sz,stone.y - sz }) && tile_empty({ stone.x - sz * 2,stone.y - sz * 2 }))res.push_back({ stone.x - sz * 2,stone.y - sz * 2 });
		if (hostile_present({ stone.x + sz,stone.y - sz }) && tile_empty({ stone.x + sz * 2,stone.y - sz * 2 }))res.push_back({ stone.x + sz * 2,stone.y - sz * 2 });
		if (hostile_present({ stone.x - sz,stone.y + sz }) && tile_empty({ stone.x - sz * 2,stone.y + sz * 2 }))res.push_back({ stone.x - sz * 2,stone.y + sz * 2 });

		return res;
	}
	Graph_lib::Circle* get_stone(Point& p) {				//Ermittelt Spielstein
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

	static void cb_next(Address, Address addr) { static_cast<My_window*>(addr)->next(); }		//Sogenannte Call Back Funktionen für Knöpfe, callen die eigentlichen Funktionen für die Knöpfe
	static void cb_quit(Address, Address addr) { reference_to<My_window>(addr).quit(); }		//Ist die Stroustrup Variante, macht auch nur 		return *static_cast<W*>(pw);
	static void cb_tile_pressed(Address, Address addr) { reference_to<My_window>(addr).tile_pressed(); }

	void next() { button_pushed = true; }								//Löst FL::redraw() (in void wait_for_button()) aus.
	void quit() { hide(); button_pushed = true; }

	void  tile_pressed() {
		Point p = get_point(Fl::event_x(), Fl::event_y());				//p=Koordinaten des aktuell gedrücketen Feldes

		int temp_x = 0;
		int temp_y = 0;
		if (stone_selected) {
			temp_x = curr_stone->center().x;
			temp_y = curr_stone->center().y;
		}




		if (!tile_empty(p) && !stone_selected) {						//Spielstein auswählen
			curr_stone = get_stone(p);
			Color c_curr = curr_stone->fill_color();
			if (c_curr == c_turn) {
				curr_stone->set_color(Color::cyan);
				stone_selected = true;
			}
			
		}
		else {
			if (!tile_empty(p) && stone_selected) {						//Spielstein abwählen
				curr_stone = get_stone(p);							
				Color c_curr = curr_stone->fill_color();
				if (c_curr == c_turn) {
					curr_stone->set_color(Color::black);
					stone_selected = false;
					}

				}
			}




		if (tile_empty(p) && stone_selected && !must_attack({ temp_x - ca, temp_y - ca })) {
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

			if (c_turn == c_player2 && p.y > pp.y && abs(diff.x) <= sz + ca && abs(diff.y) <= sz - ca) {					//Bewegung gelb
				curr_stone->move(diff.x + ca, diff.y + ca);
				curr_stone->set_color(Color::black);
				curr_stone = nullptr;
				stone_selected = false;
				c_turn = c_player1;
				current_turn.put("Red");
			}
		}
		if (tile_empty(p) && stone_selected && must_attack({ temp_x - ca, temp_y - ca })) {		//Fangen (auch Multikill)

			vector<Point> possible_moves = strike();
			Point lost_stone;																	//Position vom Stein der gefangen wird														


			for (auto k : possible_moves)if (p == k) {											//Mann muss fangen wenn der gewählte Stein fangen kann

				curr_stone->move((k.x + ca) - temp_x, (k.y + ca) - temp_y);

				if (temp_x > p.x && temp_y > p.y) lost_stone = { temp_x - sz,temp_y - sz };
				if (temp_x < p.x && temp_y < p.y) lost_stone = { temp_x + sz,temp_y + sz };
				if (temp_x < p.x && temp_y > p.y) lost_stone = { temp_x + sz,temp_y - sz };
				if (temp_x > p.x && temp_y < p.y) lost_stone = { temp_x - sz,temp_y + sz };


				for (int i = 0; i < stones.size(); i++) {
					if (stones[i].center() == lost_stone) {
						detach(stones[i]);
						stones.erase(i);
						if (c_turn == c_player1) {
							countRed = countRed - 1;
						}
						if (c_turn == c_player2) {
							countRed = countYellow - 1;
						}
					}
				}

				temp_x = curr_stone->center().x - ca;
				temp_y = curr_stone->center().y - ca;

				if (!must_attack({ temp_x, temp_y })) {											//Man muss fangen solange man kann
					curr_stone->set_color(Color::black);
					curr_stone = nullptr;
					stone_selected = false;
					if (c_turn == c_player1) {
						c_turn = c_player2;
						current_turn.put("Yellow");
					}
					else if (c_turn == c_player2) {
						c_turn = c_player1;
						current_turn.put("Red");
					}
				}
			}
		}
		//Transformation Dame
		//Bewegen Dame
		//Fangen Dame

		cout << p.x << " " << p.y << endl;																//Ist nur fürs debuggen
		//Point test;																				//Ist nur fürs debuggen
		//if (curr_stone)test = { curr_stone->center().x - ca,curr_stone->center().y - ca };		//Ist nur fürs debuggen
		//cout << "hostile present" << hostile_present(p) << endl;									//Ist nur fürs debuggen
		//if (curr_stone)cout << "must attack" << must_attack(test) << endl;						//Ist nur fürs debuggen

		if (countRed == 0) {					// Spielende verkünden. Switchanweisung sind noch Blödsinn aber irgendwas muss drin stehen.
			switch (fl_choice("Gelb hat gewonnen! Neues Spiel starten?", "Ja", "Nein", 0)) {
			case 0: countRed = 8; // Spielbrett neu aufbauen
			case 1: countRed = 0; // No (default)
			}

		}

		if (countYellow == 0) {
			switch (fl_choice("Rot hat gewonnen! Neues Spiel starten?", "Ja", "Nein", 0)) {
			case 0:  countRed = 8;// Spielbrett neu aufbauen
			case 1:  countRed = 0;// No (default)
			}

		}


		Fl::redraw();
	}
};

int main() {

	//My_window win{ {100,100},x_max(),y_max(),"Schach oder Dame" };
	My_window win{ {100,100},1000,800,"Schach oder Dame" };						//Fenster zum Debuggen verkleinert um das Konsolenfenster zu sehen


	win.wait_for_button();
}