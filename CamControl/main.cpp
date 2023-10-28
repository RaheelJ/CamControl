#include "include/NetworkedOutput.h"
#include "WitmotionWT901/WitmotionBT.hpp"
#include <simpleble/Utils.h>
#include <iostream>
#include <string>
#include <conio.h>

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
int main()
{
	_bt_adapter_config source_config;
	source_config.address = "";
	source_config.max_nodes_allowed = 20;
	source_config.scan_time = 10e3;
	source_config.uuid = "";

	_bt_peripheral_config node_config;
	node_config.address = "db:f6:61:f1:0e:d3";
	node_config.uuid = "WT901BLE68";
	node_config.notification_time = 1000;

	std::vector<_bt_service> service_list;
	service_list.resize(1);
	service_list[0].characteristic_uuid = "0000ffe4-0000-1000-8000-00805f9a34fb";
	service_list[0].service_uuid = "0000ffe5-0000-1000-8000-00805f9a34fb";
	service_list[0].packet_size = 247;
	service_list[0].operation = READ;

	auto temp_adapters = SimpleBLE::Adapter::get_adapters();
	/*bt_link::source bt_source;
	bt_source.Initialize(source_config);
	
	bt_link::node bt_node;
	bt_node.Initialize(node_config, service_list);
	bt_source.Add_Node(bt_node);

	std::string in_data, out_data;
	bt_source.Use_Service(bt_node.Get_Address(), service_list[0], in_data, out_data);*/
	
	return 0;
}