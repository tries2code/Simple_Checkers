#pragma once

//Dame Spielstein-Klasse 
//Ist noch ziemlich unbrauchbar. Hatte versucht hier Call-Backs einzufügen. Das geht entweder nur in der Fenster-Klasse oder ich bin doof..


extern int sz;

class checker_stone : public Widget {
	Graph_lib::Circle c;				//sichtbare Teil vom Spielstein


	//Weiss noch nicht genau wie b_one und b_two call-backs auserhalb von der Fenster-Klasse bekommen können...
	//Felder-Erweiterung für eine Dame folgen wenn das alles einigermaßen steht

	Button b_one;						//Mögliches Feld zum hinbewegen. Soll zukünftig Position wiedergeben. Muss noch irgendwie komplett unsichtbar werden
	Graph_lib::Rectangle r_one;			//Zeigt an das es sich um ein mögliches Feld zum hinbewegen handelt

	Button b_two;						//Mögliches Feld zum hinbewegen. Soll zukünftig Position wiedergeben. Muss noch irgendwie komplett unsichtbar werden
	Graph_lib::Rectangle r_two;			//Zeigt an das es sich um ein mögliches Feld zum hinbewegen handelt

public:
	checker_stone(Point xy, int ww, int hh, const string& s, Callback cb)
		:Widget(xy, ww, hh, s, cb), c({ xy.x + ww / 2,xy.y + ww / 2 }, ww / 2),
		b_one({ xy.x - sz,xy.y - sz }, sz, sz, " ", 0),
		r_one({ xy.x - sz,xy.y - sz }, sz, sz),
		b_two({ xy.x + sz,xy.y - sz }, sz, sz, " ", 0),
		r_two({ xy.x + sz,xy.y - sz }, sz, sz)
	{
		c.set_fill_color(FL_RED);
	}
	void attach(Graph_lib::Window& win);
	void move(int dx, int dy);
	void encolor_moves();
	void decolor_moves();
};

void checker_stone::attach(Graph_lib::Window& win)
{
	pw = new Fl_Button(loc.x, loc.y, width, height, label.c_str());
	pw->callback(reinterpret_cast<Fl_Callback*>(do_it), &win); // pass the window
	own = &win;
	own->attach(c);
	own->attach(b_one);
	own->attach(r_one);
	own->attach(b_two);
	own->attach(r_two);

}

//move, encolor_moves und decolor_moves werden in main() demonstriert
void checker_stone::move(int dx, int dy) {
	hide();
	pw->position(loc.x += dx, loc.y += dy);
	c.move(dx, dy);
	b_one.move(dx, dy);
	r_one.move(dx, dy);
	b_two.move(dx, dy);
	r_two.move(dx, dy);
	show();
}
void checker_stone::encolor_moves() {

	r_one.set_color(FL_BLUE);
	r_two.set_color(FL_BLUE);
}
void checker_stone::decolor_moves() {
	r_one.set_color(0);
	r_two.set_color(0);
}
