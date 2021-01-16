#include"Graph.h"
#include"Simple_window.h"

//Button unter schwarzen Felder eingefügt, die geben Position wieder
//Spielsteine richtig angeordnet
//Spielsteine können ausgewählt und manipulirt werden 
//field_pressed ist für die Spielmechanik, zurzeit im Aufbau
//Group Klasse mit ls und us ersetzt

int sz = 50;									//Feld Länge und Breite; vorübergehend global
int ca = sz / 2;									//Anpassung für Kreise, da die sonst oben links auf einem Feld stehen; vorübergehend global
int ra = 20;									//Radius der Spielsteine; vorübergehend global

int ls = 100;									//Linker Abstand zum Bildschirmrand
int us = 100;									//Oberer Absatnd zum Bilschirmrand

//Helper Function
Point get_point(int x, int y) {
	Point res{ x - x % sz,y - y % sz };
	return res;
}		//Ermittelt Koordinaten für Spielfeld




//Fenster Klasse; ist noch ein ziemliches Chaos...
class My_window : public Window {

	bool button_pushed;
	Button next_button;
	Button quit_button;

	Vector_ref<Button>field_buttons;
	Vector_ref< Graph_lib::Rectangle>checkers;			//Für die Felder
	Vector_ref< Graph_lib::Circle>stones;

public:
	My_window(Point xy, int w, int h, const string& title) :Window(w, h, title),
		button_pushed(false),
		next_button(Point(x_max() - 70, 0), 70, 20, "Next", cb_next),
		quit_button(Point(x_max() - 70, 30), 70, 20, "Quit", cb_quit)
	{
		for (int x = 0; x < 8; x++) {					//Knöppe unter den Feldern
			for (int y = 0; y < 8; y++) {
				if ((y + x) % 2 == 0)field_buttons.push_back(new Button{ {ls + x * sz,us + y * sz},sz,sz ," ",cb_field_pressed });
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
		for (int i = 0; i < stones.size(); i++) {			//Farbe der Steine, die Kondition if ((y + x) % 2 == 0) im oberen Loop hat bei der Einfärbung irgendwie zu Schwulitäten geführt...keine Ahnung warum
			if (i % 2 == 0)stones[i].set_fill_color(FL_RED);
			else stones[i].set_fill_color(FL_YELLOW);
		}

		attach(next_button);
		attach(quit_button);
	}
	void wait_for_button() {
		while (!button_pushed) Fl::wait();
		button_pushed = false;
		Fl::redraw();
	}

private:
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


	static void cb_next(Address, Address addr) { static_cast<My_window*>(addr)->next(); }		//Sogenannte Call Back Funktionen für Knöpfe, callen die eigentlichen Funktionen für die Knöpfe
	static void cb_quit(Address, Address addr) { reference_to<My_window>(addr).quit(); }		//Ist die Stroustrup Variante, macht auch nur 		return *static_cast<W*>(pw);
	static void cb_field_pressed(Address, Address addr) { reference_to<My_window>(addr).field_pressed(); }

	void next() { button_pushed = true; }				//Löst FL::redraw() (in void wait_for_button()) aus.
	void quit() { hide(); button_pushed = true; }

	void  field_pressed() {
		Point p = get_point(Fl::event_x(), Fl::event_y());

		Graph_lib::Circle* c = get_stone(p);

		c->set_color(Color::cyan);
		c->move(-sz, -sz);
		cout << p.x << " " << p.y << endl;
		Fl::redraw();
	}
};



int main() {

	//My_window win{ {100,100},x_max(),y_max(),"Schach oder Dame" };
	My_window win{ {100,100},1000,800,"Schach oder Dame" };




	win.wait_for_button();

}