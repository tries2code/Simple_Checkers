#pragma once
#include"Graph.h"
#include"Simple_window.h"
#include <FL/Fl_Multiline_Output.H>
#include<fstream>

//Die Nummer war pain...

ifstream ifs{ "rules.txt" };
char c;
string text;

struct Multi_Out_box : Widget {
	Multi_Out_box(Point xy, int w, int h, const string& s/*, Window& win*/)
		:Widget(xy, w, h, s, 0)
	{
	}
	void put(const string&);

	void attach(Graph_lib::Window& win);
};

void Multi_Out_box::put(const string& s) {reference_to<Fl_Multiline_Output>(pw).value(s.c_str());}

void Multi_Out_box::attach(Graph_lib::Window& win) {
	pw = new Fl_Multiline_Output(loc.x, loc.y, width, height, label.c_str());
	own = &win;
}

struct Rules_window :public Window {
	bool button_pushed;
	Button ok_button;
	Multi_Out_box txt;
public:
	Rules_window(Point xy, int w, int h, const string& title) :Window(w, h, title),
		button_pushed(false), ok_button(Point(x_max() - 70, 30), 70, 20, "OK", cb_OK),
		txt({ 0,0 }, 1000, 800, ""){
		attach(txt);
		attach(ok_button);
		txt.put(text);
	}

	void wait_for_button() {
		while (!button_pushed) Fl::wait();
		button_pushed = false;
		Fl::redraw();
	}
private:
	static void cb_OK(Address, Address addr) { reference_to<Rules_window>(addr).quit(); }
	void quit() { hide(); button_pushed = true; }
};