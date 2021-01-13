#pragma once

//Dame Spielstein-Klasse 
//Man drückt auf den Stein und zwei Felder werden farblich makiert. Man wählt eins der beiden aus um den Stein zu bewegen.
//Kann sich bisher nur in einer Richtung bewegen.

class checker_stone : public Widget {
	Graph_lib::Circle c;					//sichtbare Teil vom Spielstein

	//Felder-Erweiterung für eine Dame folgen wenn das alles einigermaßen steht

	Button b_left;							//Mögliches Feld zum hinbewegen. Muss noch irgendwie komplett unsichtbar werden
	Graph_lib::Rectangle r_left;			//Zeigt an das es sich um ein mögliches Feld zum hinbewegen handelt

	Button b_right;							//Mögliches Feld zum hinbewegen. Muss noch irgendwie komplett unsichtbar werden
	Graph_lib::Rectangle r_right;			//Zeigt an das es sich um ein mögliches Feld zum hinbewegen handelt

	bool moveability;

public:
	checker_stone(Point xy, int ww, int hh, const string& s, Callback cb, Callback cb2, Callback cb3, bool m_e)
		:Widget(xy, ww, hh, s, cb), c({ xy.x + ww / 2,xy.y + ww / 2 }, ww / 2),
		b_left({ xy.x - ww,xy.y - hh }, ww, hh, " ", cb2),
		r_left({ xy.x - ww,xy.y - hh }, ww, hh),
		b_right({ xy.x + ww,xy.y - hh }, ww, hh, " ", cb3),
		r_right({ xy.x + ww,xy.y - hh }, ww, hh),
		moveability(m_e)
	{
		c.set_fill_color(FL_RED);
	}
	void attach(Graph_lib::Window& win);				//Übergibt die Representation an das Fenster
	void move(int dx, int dy);
	void encolor_moves();								//Markiert Felder auf die man sich bewegen kann
	void decolor_moves();								//hebt o.g. Markierung wieder auf
	void m_enable() { moveability = true; }				//Handbremse aus
	void m_disable() { moveability = false; }			//Handbremse an
	bool is_moveable() const { return moveability; }	//Ist die Handbremse angezogen?
};

void checker_stone::attach(Graph_lib::Window& win) {
	pw = new Fl_Button(loc.x, loc.y, width, height, label.c_str());
	pw->callback(reinterpret_cast<Fl_Callback*>(do_it), &win); // pass the window
	own = &win;
	own->attach(c);
	own->attach(b_left);
	own->attach(r_left);
	own->attach(b_right);
	own->attach(r_right);
}
void checker_stone::move(int dx, int dy) {
	hide();
	pw->position(loc.x += dx, loc.y += dy);
	c.move(dx, dy);
	b_left.move(dx, dy);
	r_left.move(dx, dy);
	b_right.move(dx, dy);
	r_right.move(dx, dy);
	show();
}
void checker_stone::encolor_moves() {
	r_left.set_color(Color::cyan);
	r_right.set_color(Color::cyan);
	Fl::redraw();
}
void checker_stone::decolor_moves() {
	r_left.set_color(Color::invisible);
	r_right.set_color(Color::invisible);
	Fl::redraw();
}

