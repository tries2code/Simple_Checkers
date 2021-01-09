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

int main() {
	Simple_window win{ {100,100},x_max(),y_max(),"Chapter 14" };
	Group g;
	Vector_ref< Graph_lib::Rectangle>checkers;
	Vector_ref< Graph_lib::Circle>stones;

	for (int x = 0; x < 8; x++) {
		for (int y = 0; y < 8; y++) {
			checkers.push_back(new Graph_lib::Rectangle{ {x * 50,y * 50},50,50 });
			checkers[checkers.size() - 1].set_color(FL_BLACK);
			if ((y + x) % 2 == 0)checkers[checkers.size() - 1].set_fill_color(FL_BLACK);
			win.attach(checkers[checkers.size() - 1]);
			g.add_shape(checkers[checkers.size() - 1]);
		}
	}
	for (int x = 0; x < 8; x++) {
		for (int y = 0; y < 8; y++) {
			stones.push_back(new Graph_lib::Circle{ {x * 50 + 25,y * 50 + 25},20 });
			stones[stones.size() - 1].set_color(FL_BLACK);
			if (y < 3)stones[stones.size() - 1].set_fill_color(FL_RED);
			if (y > 5)stones[stones.size() - 1].set_fill_color(FL_YELLOW);
			win.attach(stones[stones.size() - 1]);
			g.add_shape(stones[stones.size() - 1]);
			if (y == 1)y = 5;
		}
	}

	win.wait_for_button();
	stones[5].move(50, 50);
	win.wait_for_button();
	stones[14].move(50, -50);
	win.wait_for_button();
	g.move(100, 100);
	win.wait_for_button();
	g.set_element_color(73, Color::blue);
	win.wait_for_button();
	g.set_element_fill_color(73, Color::blue);
	win.wait_for_button();
	g.set_group_color(17, 20, Color::green);
	win.wait_for_button();
	g.set_group_fill_color(17, 20, Color::green);
	win.wait_for_button();

}
