#include"Graph.h"
#include"Simple_window.h"



/*
fltkd.lib
wsock32.lib
comctl32.lib
fltkjpegd.lib
fltkimagesd.lib
%(AdditionalDependencies)

*/

int main() {

	Simple_window win{ {100,100},800,600,"Test Fenster" };

	win.wait_for_button();

}
