#include "vrpn_Analog.h"
#include "vrpn_Button.h"

#include <iostream>

using namespace std;

//Global variables
bool activated = true;
float xpos = 0, ypos = 0;
bool mouse_moving = false;
bool left_mb = false;
bool right_mb = false;
char mouse_area = ' ';
char old_area = ' ';

void VRPN_CALLBACK handle_analog(void* userData, const vrpn_ANALOGCB a)
{
	//printf("Received x: %f y: %f \n", a.channel[0], a.channel[1]);
	mouse_moving = true;
	xpos = a.channel[0];
	ypos = a.channel[1];
}

void VRPN_CALLBACK handle_button(void* userData, const vrpn_BUTTONCB b)
{
	//printf("Button %d is now in state %d\n", b.button, b.state);
	if (b.button == 0) left_mb = b.state;
	else if (b.button == 2) right_mb = b.state;
}

int main(int argc, char* argv[])
{
	char *addr = "Mouse0@localhost:3883";

	vrpn_Analog_Remote *analog = new vrpn_Analog_Remote(addr);
	vrpn_Button_Remote *button = new vrpn_Button_Remote(addr);
	analog->register_change_handler(NULL, handle_analog);
	button->register_change_handler(NULL, handle_button);

	while (1) {
		//Reset the "moving" state, it will be true if the analog handler is called.
		mouse_moving = false; 

		//Handlers
		analog->mainloop();
		button->mainloop();

		//Activation/Deactivation
		if (!mouse_moving && left_mb && activated)
		{
			activated = false;
			printf("Disabled\n");
		}
		else if (!mouse_moving && right_mb && !activated)
		{
			activated = true;
			printf("Enabled :)\n");
		}

		//Behavior
		if (activated)
		{
			if (right_mb) printf("\a");

			if (xpos < 0.5 && ypos < 0.5) mouse_area = 'A';
			if (xpos >= 0.5 && ypos < 0.5) mouse_area = 'B';
			if (xpos < 0.5 && ypos >= 0.5) mouse_area = 'C';
			if (xpos >= 0.5 && ypos >= 0.5) mouse_area = 'D';
			printf("Zone : %c\n", mouse_area);
			
			if (mouse_area != old_area) printf("\a");
			old_area = mouse_area;
		}

		Sleep(100);
	}
}