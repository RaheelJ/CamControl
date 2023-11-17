#include "include/NetworkedOutput.h"
#include "WitmotionWT901/WitmotionBT.hpp"
#include <iostream>
#include <string>
#include <conio.h>
#include <thread>
#include <InputInterface.h>
#include <CoverageQueue.h>
#include <CommandStack.h>

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
int main_bt()
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
int main()
{
	// Camera Control Centre ///////////////////////////////////////////////////////////////////////////////////////////
	
	// Input Block /////////////////////////////////////////////////////////////////////////////////////////////////////
	_target T1_t0, T1_t1;
	T1_t0.ID = 100;
	T1_t0.time = 0;
	T1_t0.pos = { true, 2, 2, 1000, 0, 0, 0 };
	T1_t0.threat = { 8.0, 0.8 };
	T1_t0.type = { AIRCRAFT, 400.0 };
	T1_t0.vel = { 2, 2, 2, 0, 0, 0 };
	T1_t1 = T1_t0;
	T1_t1.time = 1.0;

	_target T2_t0, T2_t1;
	T2_t0.ID = 101;
	T2_t0.time = 0;
	T2_t0.pos = { true, 4, 4, 2000, 0, 0, 0 };
	T2_t0.threat = { 6.0, 0.88 };
	T2_t0.type = { AIRCRAFT, 800.0 };
	T2_t0.vel = { 8, 8, 2, 0, 0, 0 };
	T2_t1 = T2_t0;
	T2_t1.time = 1.0;

	_asset A1_t0, A1_t1;
	A1_t0.time = 0.0;
	A1_t0.ID = 01;
	A1_t1 = A1_t0;
	A1_t1.time = 1.0;

	InputBlock IB1;
	std::string message_IB1;
	std::vector<_target> out_Targets_IB1, out_Targets_IB1_prev, in_Targets_IB1 = { T1_t0, T2_t0 };
	std::vector<_asset> out_Assets_IB1, out_Assets_IB1_prev, in_Assets_IB1 = { A1_t0 };
	IB1.Initialize("", message_IB1);
	IB1.AddTargets(in_Targets_IB1, 0.0);
	IB1.UpdateAssets(in_Assets_IB1, 0.0);
	IB1.GetTargets(out_Targets_IB1, out_Targets_IB1_prev);
	IB1.GetAssets(out_Assets_IB1, out_Assets_IB1_prev);
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	 
	// Coverage Queue /////////////////////////////////////////////////////////////////////////////////////////////////
	/*CoverageBlock CB1;
	std::string config_CB1, message_CB1;
	bool status_CB1 = false;
	config_CB1 = "config/Camera_Controller-1_d20.xml";
	std::vector<_target> in_Targets_CB1 = out_Targets_IB1;
	_queue_element out_Q1_CB1{ 0 };
	std::vector<_queue_element> out_Queue_CB1;

	status_CB1 = CB1.Initialize(config_CB1, message_CB1);
	CB1.UpdateQueue(in_Targets_CB1, 1.0);
	CB1.GetQueue(out_Queue_CB1);
	CB1.PopQueue(out_Q1_CB1);
	CB1.PopQueue(out_Q1_CB1);
	CB1.ModifyQueue(out_Q1_CB1, -1);
	CB1.GetQueue(out_Queue_CB1);*/
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// BT IMU Data Acquisition /////////////////////////////////////////////////////////////////////////////////////////
	_bt_adapter_config source_config;
	source_config.address = "";
	source_config.max_nodes_allowed = 20;
	source_config.scan_time = 6e3;
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

	bt_link::source bt_source;
	bt_source.Initialize(source_config);

	bt_link::node bt_node;
	bt_node.Initialize(node_config, service_list);
	bt_source.Add_Node(bt_node);

	std::vector<uint16_t> in_data{ 0xFF, 0xAA, 0x27, 0x3A, 0x00 };
	std::string out_data;
	double yaw{ 0 }, pitch{ 0 }, roll{ 0 };

	bt_source.Use_Service(bt_node.Get_Name(), service_list[0].name, Hex2String(in_data), out_data);
	std::this_thread::sleep_for(std::chrono::milliseconds(10));
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Command Stack ///////////////////////////////////////////////////////////////////////////////////////////////////
	CommandBlock CM1;
	std::string config_CM1 = "config/Camera_Controller-1_d20.xml";
	std::string message_CM1;
	std::vector<_asset> in_Assets_CM1 = out_Assets_IB1;
	std::vector<_target> in_Targets_CM1 = out_Targets_IB1;
	_cam_state cam_state_CM1;

	CM1.Initialize(config_CM1, message_CM1);
	CM1.Assign_Camera("cam_1");

	while (1)
	{
		// BT module mounted at the bottom of the camera
		// Camera pan (+ve yaw) --> Module z-axis (-ve yaw)
		// Camera tilt (+ve pitch)  --> Module y-axis (-ve pitch)
		// Camera no roll motion --> constant x-axis (roll)
		bt_node.Get_Orientation(yaw, pitch, roll);
		
		cam_state_CM1.pan = -yaw;
		cam_state_CM1.tilt = -pitch;
		CM1.CalcRefState(in_Assets_CM1[0], cam_state_CM1, in_Targets_CM1[0]);
		CM1.GeneratePath();
		CM1.FollowPath();
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
	
	return 0;
}