#include"Graph.h"
#include"Simple_window.h"


// https://bumpyroadtocode.com/2017/08/05/how-to-install-and-use-fltk-1-3-4-in-visual-studio-2017-complete-guide/

int main() {

	Simple_window win{ {100,100},800,600,"Test Fenster" };

	win.wait_for_button();

}
