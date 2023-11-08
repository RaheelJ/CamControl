#include "include/NetworkedOutput.h"
#include "WitmotionWT901/WitmotionBT.hpp"
#include <iostream>
#include <string>
#include <conio.h>
#include <thread>

int main_cam()
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

	CamHandler.Assign_Camera("cam_2");
	/*while (1)
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
	}*/

	return 0;
}
int main()
{
	_bt_adapter_config source_config;
	source_config.address = "";
	source_config.max_nodes_allowed = 20;
	source_config.scan_time = 8e3;
	source_config.name = "";

	_bt_peripheral_config node_config;
	node_config.address = "db:f6:61:f1:0e:d3";
	node_config.name = "WT901BLE68";

	std::vector<_bt_service> service_list;
	service_list.resize(2);
	service_list[0].name = "notify_9_axis";
	service_list[0].characteristic_uuid = "0000ffe4-0000-1000-8000-00805f9a34fb";
	service_list[0].service_uuid = "0000ffe5-0000-1000-8000-00805f9a34fb";
	service_list[0].packet_size = 247;
	service_list[0].operation = NOTIFY;

	service_list[1].name = "request_9_axis";
	service_list[1].characteristic_uuid = "0000ffe9-0000-1000-8000-00805f9a34fb";
	service_list[1].service_uuid = "0000ffe5-0000-1000-8000-00805f9a34fb";
	service_list[1].packet_size = 247;
	service_list[1].operation = WRITE_REQUEST;

	bt_link::source bt_source;
	bt_source.Initialize(source_config);
	
	bt_link::node bt_node;
	bt_node.Initialize(node_config, service_list);
	bt_source.Add_Node(bt_node);

	std::vector<uint16_t> in_data{ 0xFF, 0xAA, 0x27, 0x3A, 0x00 };
	std::string out_data;

	bt_source.Use_Service(bt_node.Get_Name(), service_list[0].name, Hex2String(in_data), out_data);
	bt_source.Use_Service(bt_node.Get_Name(), service_list[1].name, Hex2String(in_data), out_data);
	
	double yaw{ 0 }, pitch{ 0 }, roll{ 0 };
	
	std::this_thread::sleep_for(std::chrono::milliseconds(1));
	bt_node.Get_Orientation(yaw, pitch, roll);
	std::this_thread::sleep_for(std::chrono::milliseconds(1));
	bt_node.Get_Orientation(yaw, pitch, roll);

	return 0;
}