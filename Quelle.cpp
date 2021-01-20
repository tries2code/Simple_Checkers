#include"Graph.h"
//#include"Simple_window.h"
#include <FL/fl_ask.H>
#include"Rules_window.h"
#undef vector

//Staus Spielmechanik:
//Spieler 1 und 2 können nur abwechselnd spielen
//Spielsteine können nicht rückwärts gehen
//Spielsteine können nur ein Feld weit laufen
//Mechanik zum gegenerische Steine fangen
//Mechanik für Multikills
//Wenn einer gewonnen hat kann man von vorne anfangen oder beenden
//Trasformation eines Spielsteins zur Dame
//Dame laufen
//Dame fangen
// Die Dame kann:
//					-Beliebig weit laufen, sofern keine zwei Gegener hintereinander im weg stehen
//					-In einem Zug mehrere Gegener Fangen solange zwischen den Gegnern mindestens ein freies Feld ist
//					-Einen zweiten Zug machen wenn sie beim ersten Zug mindestens einen Gegener gefangen hat und neben einem Gegner stehen beleibt
//					-Frei entscheiden ob sie angreift oder doch woanders hinläuft

//Es fehlt noch:
//Tests Tests Tests....


//Bekannte Bugs:
//					-sz muss durch 50 teilbar sein damit alles funktioniert (sprich 50 oder 100)
//					-Wenn eine Stein zur Dame transformiert und angreifen kann ist er nach wie vor am Zug. Feature?


constexpr int screen_x = 1200;
constexpr int screen_y = 1000;

constexpr int sz = 100;									//Feld Länge und Breite; vorübergehend global
constexpr int ca = sz / 2;								//Anpassung für Kreise, da die sonst oben links auf einem Feld stehen; vorübergehend global
constexpr int ra = 40;									//Radius der Spielsteine; vorübergehend global

constexpr int ls = 100;									//Linker Abstand zum Bildschirmrand
constexpr int us = 0;									//Oberer Absatnd zum Bilschirmrand

//Helfer Funktionen
Point get_point(int x, int y) {							//Ermittelt Koordinaten für Spielfeld
	Point res{ x - x % sz,y - y % sz };
	return res;
}
bool operator==(const Line_style& a, const Line_style& b) {
	int x = a.style();
	int y = b.style();
	if (x == y)return true;
	return false;
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

	const Line_style king_style = Line_style(Line_style::dash, 8);	//Der Stil einer Dame
	const Line_style piece_style = Line_style(Line_style::solid, 2);	//Der einfache Stil

	const Color c_player1 = Color::red;
	const Color c_player2 = Color::yellow;
	Color c_turn = c_player1;								//Der Spieler der grade dran ist

	Graph_lib::Circle* curr_stone = nullptr;

	char countRed = 8;
	char countYellow = 8;

	bool button_pushed;
	bool stone_selected;								//Wurde ein Stein ausgewählt?

	Button rules_button;
	Button restart_button;
	Button quit_button;

	Out_box current_turn;								//Sagt einem wer grad dran ist, ja, ist hässlich, kann man überarbeiten

	Vector_ref<Button>field_buttons;					//Für die Knöppe unter den schwarzen Feldern
	Vector_ref< Graph_lib::Rectangle>checkers;			//Für die Felder
	Vector_ref< Graph_lib::Circle>stones;				//Für die Spielsteine, sind nur ausgemalte Kreise

public:
	My_window(Point xy, int w, int h, const string& title) :Window(w, h, title),
		button_pushed(false), stone_selected(false),
		rules_button(Point(x_max() - 70, 0), 70, 20, "Rules", cb_rules),
		restart_button(Point(x_max() - 70, 30), 70, 20, "Restart", cb_restart),
		quit_button(Point(x_max() - 70, 60), 70, 20, "Quit", cb_quit),
		current_turn(Point{ x_max() - 150, 100 }, 100, 30, "Current Player: ")
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
				stones[stones.size() - 1].set_color(Color::white);
				stones[stones.size() - 1].set_style(piece_style);
				attach(stones[stones.size() - 1]);
				if (y == 1)y = 5;
			}
		}
		for (int i = 0; i < stones.size(); i++) {		//Farbe der Steine, die Kondition if ((y + x) % 2 == 0) im oberen Steine-Loop hat bei der Einfärbung irgendwie zu Schwulitäten geführt...keine Ahnung warum
			if (i % 2 == 0)stones[i].set_fill_color(c_player2);
			else stones[i].set_fill_color(c_player1);
		}
		attach(rules_button);
		attach(restart_button);
		attach(quit_button);
		attach(current_turn);
		current_turn.put("Red");						//rot fängt an
		size_range(screen_x, screen_y, screen_x, screen_y);
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
	void make_king(Graph_lib::Circle* c) {					//Spielstein wird zur Dame wenn er das gegenüberliegende Ende erreicht
		int y = c->center().y - ca;
		if (c->fill_color() == c_player1 && y == us)c->set_style(king_style);
		if (c->fill_color() == c_player2 && y == us + sz * 7)c->set_style(king_style);
	}
	bool is_king(Graph_lib::Circle* c) {					//Ist der Speilstein eine Dame?
		if (c->style() == Line_style(Line_style::dash, 8))return true;
		return false;
	}

	static void cb_rules(Address, Address addr) { static_cast<My_window*>(addr)->rules(); }		//Sogenannte Call Back Funktionen für Knöpfe, callen die eigentlichen Funktionen für die Knöpfe
	static void cb_restart(Address, Address addr) { static_cast<My_window*>(addr)->restart_game(); }
	static void cb_quit(Address, Address addr) { reference_to<My_window>(addr).quit(); }		//Ist die Stroustrup Variante, macht auch nur 		return *static_cast<W*>(pw);
	static void cb_tile_pressed(Address, Address addr) { reference_to<My_window>(addr).tile_pressed(); }

	void rules() { Rules_window* rls = new Rules_window({ 100,100 }, screen_x, screen_y, "Rules"); }
	void restart_game() {

		c_turn = c_player1;
		current_turn.put("Red");
		curr_stone = nullptr;
		stone_selected = false;
		countRed = 8;
		countYellow = 8;

		for (int i = 0; i < stones.size(); i++) {
			detach(stones[i]);
		}
		stones.clear();								//Beim Anwenden von Vector_ref::erase() bleibt ein Stein übrig, keine Ahnung warum...
		for (int x = 0; x < 8; x++) {
			for (int y = 0; y < 8; y++) {
				if ((y + x) % 2 == 0)stones.push_back(new Graph_lib::Circle{ {ls + x * sz + ca,us + y * sz + ca},ra });
				attach(stones[stones.size() - 1]);
				if (y == 1)y = 5;
			}
		}
		for (int i = 0; i < stones.size(); i++) {
			if (i % 2 == 0)stones[i].set_fill_color(c_player2);
			else stones[i].set_fill_color(c_player1);
		}
		Fl::redraw();
	}
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
			if (!tile_empty(p) && stone_selected /*&& !must_attack({ temp_x - ca, temp_y - ca })*/) {		//Spielstein abwählen, auch bei Steinen die angreifen müssen
				curr_stone->set_color(Color::white);
				curr_stone = get_stone(p);
				Color c_curr = curr_stone->fill_color();
				if (c_curr == c_turn) {
					curr_stone->set_color(Color::white);
					stone_selected = false;
				}
			}
		}
		if (tile_empty(p) && stone_selected && !must_attack({ temp_x - ca, temp_y - ca }) && !is_king(curr_stone)) {

			Point pp = curr_stone->center();
			Point diff = { p.x - pp.x,p.y - pp.y };

			if (c_turn == c_player1 && p.y < pp.y && abs(diff.x) <= sz + ca && abs(diff.y) <= sz + ca && diff.y + ca != 0) {					//Bewegung rot
				curr_stone->move(diff.x + ca, diff.y + ca);
				curr_stone->set_color(Color::white);
				make_king(curr_stone);
				curr_stone = nullptr;
				stone_selected = false;
				c_turn = c_player2;
				current_turn.put("Yellow");
			}

			if (c_turn == c_player2 && p.y > pp.y && abs(diff.x) <= sz + ca && abs(diff.y) <= sz - ca) {					//Bewegung gelb
				curr_stone->move(diff.x + ca, diff.y + ca);
				curr_stone->set_color(Color::white);
				make_king(curr_stone);
				curr_stone = nullptr;
				stone_selected = false;
				c_turn = c_player1;
				current_turn.put("Red");
			}
		}
		if (tile_empty(p) && stone_selected && must_attack({ temp_x - ca, temp_y - ca }) && !is_king(curr_stone)) {		//Fangen (auch Multikill)

			vector<Point> possible_moves = strike();
			Point lost_stone;																	//Position vom Stein der gefangen wird														

			for (auto& k : possible_moves)if (p == k) {											//Mann muss fangen wenn der gewählte Stein fangen kann

				curr_stone->move((k.x + ca) - temp_x, (k.y + ca) - temp_y);
				make_king(curr_stone);

				if (temp_x > p.x && temp_y > p.y) lost_stone = { temp_x - sz,temp_y - sz };
				if (temp_x < p.x && temp_y < p.y) lost_stone = { temp_x + sz,temp_y + sz };
				if (temp_x < p.x && temp_y > p.y) lost_stone = { temp_x + sz,temp_y - sz };
				if (temp_x > p.x && temp_y < p.y) lost_stone = { temp_x - sz,temp_y + sz };

				for (int i = 0; i < stones.size(); i++) {
					if (stones[i].center() == lost_stone) {
						detach(stones[i]);
						stones.erase(i);
						if (c_turn == c_player1)countRed--;
						if (c_turn == c_player2)countYellow--;
					}
				}
				temp_x = curr_stone->center().x - ca;
				temp_y = curr_stone->center().y - ca;

				if (!must_attack({ temp_x, temp_y })) {											//Man muss fangen solange man kann
					curr_stone->set_color(Color::white);
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
		if (tile_empty(p) && stone_selected && is_king(curr_stone)) {					//Dame bewegen und fangen

			temp_x = curr_stone->center().x - ca;
			temp_y = curr_stone->center().y - ca;

			Point start{ temp_x,temp_y };
			Point move_to;

			vector<Point>lost_stones;

			bool double_hostiles = false;

			if (temp_x > p.x && temp_y > p.y && !double_hostiles) {					// von unten rechts nach oben links
				while (temp_x != p.x) {
					temp_x -= sz;
					temp_y -= sz;
					if (temp_x < ls || temp_y < us)break;
					if (hostile_present({ temp_x, temp_y }) && hostile_present({ temp_x - sz, temp_y - sz })) double_hostiles = true;
					else if (hostile_present({ temp_x, temp_y }) && tile_empty({ temp_x - sz, temp_y - sz }))lost_stones.push_back({ temp_x + ca, temp_y + ca });
				}
				if (temp_x >= ls && temp_y >= us && tile_empty({ temp_x,temp_y }))move_to = { temp_x,temp_y };
			}
			else if (temp_x < p.x && temp_y < p.y && !double_hostiles) {					// von oben links nach unten rechts
				while (temp_x != p.x) {
					temp_x += sz;
					temp_y += sz;
					if (temp_x > ls + sz * 7 || temp_y > us + sz * 7)break;
					if (hostile_present({ temp_x, temp_y }) && hostile_present({ temp_x + sz, temp_y + sz })) double_hostiles = true;
					else if (hostile_present({ temp_x, temp_y }) && tile_empty({ temp_x + sz, temp_y + sz }))lost_stones.push_back({ temp_x + ca,temp_y + ca });
				}
				if (temp_x <= ls + sz * 7 && temp_y <= us + sz * 7 && tile_empty({ temp_x,temp_y }))move_to = { temp_x,temp_y };
			}
			else if (temp_x < p.x && temp_y > p.y && !double_hostiles) {					// von unten links nach oben rechts
				while (temp_x != p.x) {
					temp_x += sz;
					temp_y -= sz;
					if (temp_x > ls + sz * 7 || temp_y < us)break;
					if (hostile_present({ temp_x, temp_y }) && hostile_present({ temp_x + sz, temp_y - sz })) double_hostiles = true;
					else if (hostile_present({ temp_x, temp_y }) && tile_empty({ temp_x + sz, temp_y - sz }))lost_stones.push_back({ temp_x + ca,temp_y + ca });
				}
				if (temp_x <= ls + sz * 7 && temp_y >= us && tile_empty({ temp_x,temp_y }))move_to = { temp_x,temp_y };
			}
			else if (temp_x > p.x && temp_y < p.y && !double_hostiles) {					// von oben rechts nach unten links
				while (temp_x != p.x) {
					temp_x -= sz;
					temp_y += sz;
					if (temp_x < ls || temp_y > us + sz * 7)break;
					if (hostile_present({ temp_x, temp_y }) && hostile_present({ temp_x - sz, temp_y + sz })) double_hostiles = true;
					else if (hostile_present({ temp_x, temp_y }) && tile_empty({ temp_x - sz, temp_y + sz }))lost_stones.push_back({ temp_x + ca,temp_y + ca });
				}
				if (temp_x >= ls && temp_y <= us + sz * 7 && tile_empty({ temp_x,temp_y }))move_to = { temp_x,temp_y };
			}
			if (!double_hostiles) {														//Dame kann nicht über zwei nebeneinander stehende Gegener laufen
				if (p == move_to)curr_stone->move((p.x - start.x), (p.y - start.y));

				for (int x = 0; x < stones.size(); x++) {
					for (unsigned int j = 0; j < lost_stones.size(); j++) {
						if (stones[x].center() == lost_stones[j]) {
							detach(stones[x]);
							stones.erase(x);
							if (c_turn == c_player1)countRed--;
							if (c_turn == c_player2)countYellow--;
						}
					}
				}
				if (!must_attack({ temp_x, temp_y })) {											//Dame kann weiter fangen
					curr_stone->set_color(Color::white);
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
				else if (lost_stones.size() == 0 && must_attack({ temp_x, temp_y })) {				//es geht nur weiter wenn die Dame beim ersten Zug etwas gefangen hat
					curr_stone->set_color(Color::white);
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
		if (countRed == 0) {					// Spielende 
			switch (fl_choice("Gelb hat gewonnen! Neues Spiel starten?", "Ja", "Nein", 0)) {
			case 0:
				restart_game();						// Spielbrett neu aufbauen
				break;
			case 1:
				quit();
				break;
			}
		}

		if (countYellow == 0) {					// Spielende 
			switch (fl_choice("Rot hat gewonnen! Neues Spiel starten?", "Ja", "Nein", 0)) {
			case 0:
				restart_game();						// Spielbrett neu aufbauen
				break;
			case 1:
				quit();
				break;
			}
		}
		Fl::redraw();
	}
};

int main() {

	while (c != '#') {
		ifs.get(c);
		if (c == '\n')text += "\n";
		if (c != '#')text += c;
	}

	My_window win{ {100,100},screen_x,screen_y,"Schach oder Dame" };						//Fenster zum Debuggen verkleinert um das Konsolenfenster zu sehen

	win.wait_for_button();


}
