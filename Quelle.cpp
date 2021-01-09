#include"Graph.h"
#include"Simple_window.h"


class Group :public Shape {
public:
	Group() {}
	void add_shape(Shape& s);
	void move(int x, int y);
	void set_element_color(int element, Color c);
	void set_element_fill_color(int element, Color c);
	void set_group_color(int start, int end, Color c);
	void set_group_fill_color(int start, int end, Color c);
private:
	Vector_ref<Shape>vec;
};
void Group::add_shape(Shape& s) {
	vec.push_back(s);
}
void Group::move(int x, int y) {
	for (int i = 0; i < vec.size(); i++) {
		vec[i].move(x, y);
	}
}
void Group::set_element_color(int element, Color c) {
	vec[element].set_color(c);
}
void Group::set_element_fill_color(int element, Color c) {
	vec[element].set_fill_color(c);
}
void Group::set_group_color(int start, int end, Color c) {
	for (int i = start; i < end; i++) {
		vec[i].set_color(c);
	}
}
void Group::set_group_fill_color(int start, int end, Color c) {
	for (int i = start; i < end; i++) {
		vec[i].set_fill_color(c);
	}
}




struct My_window : Window {
	My_window(Point xy, int w, int h, const string& title)
		: Window(xy, w, h, title),
		button_pushed(false),
		next_button(Point(x_max() - 70, 0), 70, 20, "Next", cb_next), quit_button(Point(x_max() - 70, 30), 70, 20, "Quit", cb_quit)
	{
		attach(next_button);
		attach(quit_button);

		for (int x = 0; x < 8; x++) {
			for (int y = 0; y < 8; y++) {
				checkers.push_back(new Graph_lib::Rectangle{ {x * 50,y * 50},50,50 });
				//checkers[checkers.size() - 1].set_color(FL_BLACK);
				if ((y + x) % 2 == 0)checkers[checkers.size() - 1].set_fill_color(FL_BLACK);
				attach(checkers[checkers.size() - 1]);
				g.add_shape(checkers[checkers.size() - 1]);
			}
		}
		for (int x = 0; x < 8; x++) {
			for (int y = 0; y < 8; y++) {
				stones.push_back(new Graph_lib::Circle{ {x * 50 + 25,y * 50 + 25},20 });
				//	stones[stones.size() - 1].set_color(FL_BLACK);
				if (y < 3)stones[stones.size() - 1].set_fill_color(FL_RED);
				if (y > 5)stones[stones.size() - 1].set_fill_color(FL_YELLOW);
				attach(stones[stones.size() - 1]);
				g.add_shape(stones[stones.size() - 1]);
				if (y == 1)y = 5;
			}
		}

	}
	void wait_for_button() {
		while (!button_pushed) Fl::wait();
		button_pushed = false;
		Fl::redraw();
	}

	Button next_button;
	Button quit_button;

	Group g;
	Vector_ref< Graph_lib::Rectangle>checkers;
	Vector_ref< Graph_lib::Circle>stones;
private:
	bool button_pushed;
	static void cb_next(Address, Address addr) { static_cast<My_window*>(addr)->next(); }
	static void cb_quit(Address, Address addr) { static_cast<My_window*>(addr)->quit(); }

	void next() { button_pushed = true; }
	void quit() { hide(); button_pushed = true; }



	
};

int main() {
	My_window win{ {100,100},x_max(),y_max(),"Schach oder Dame" };

	
	win.wait_for_button();
	win.stones[5].move(50, 50);
	win.wait_for_button();
	win.stones[14].move(50, -50);
	win.wait_for_button();
	win.g.move(100, 100);
	win.wait_for_button();
	win.g.set_element_color(73, Color::blue);
	win.wait_for_button();
	win.g.set_element_fill_color(73, Color::blue);
	win.wait_for_button();
	win.g.set_group_color(17, 20, Color::green);
	win.wait_for_button();
	win.g.set_group_fill_color(17, 20, Color::green);
	win.wait_for_button();


	win.wait_for_button();

}


