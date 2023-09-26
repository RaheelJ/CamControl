#include "include/NetworkedOutput.h"
#include <iostream>
#include <string>
#include <conio.h>

int main()
{
	OutputBlock CamHandler;
	std::string config_file, message;
	char user_input{ 'x' };
	bool status{ false };

	config_file = "input/Camera_Controller-1_d20.xml";
	status = CamHandler.Initialize(config_file, message);
	if (!status)
	{
		std::cout << "Couldn't Initialize !" << std::endl;
		_getch();
		return -1;
	}

	CamHandler.Assign_Camera("cam_1");
	while (1)
	{
		user_input = _getch();
		if (user_input == 'w')
		{
			CamHandler.SendPTZ(UP);
		}
		else if (user_input == 's')
		{
			CamHandler.SendPTZ(DOWN);
		}
		else if (user_input == 'd')
		{
			CamHandler.SendPTZ(RIGHT);
		}
		else if (user_input == 'a')
		{
			CamHandler.SendPTZ(LEFT);
		}
		else if (user_input == 'x')
		{
			break;
		}
		else
		{
			std::cout << "Invalid Command !" << std::endl;
		}
	}

	return 0;
}