#pragma once
#include <simpleble/SimpleBLE.h>
#include <string>

enum _bt_operation
{
	READ,
	WRITE,
	SUBSCRIBE
};
struct _bt_adapter_config
{
	std::string uuid;
	std::string address;
	double scan_time = 0;				//ms
	int max_nodes_allowed = 1;
};
struct _bt_peripheral_config
{
	std::string uuid;
	std::string address;
	double notification_time = 0;			//ms
};
struct _bt_service
{
	std::string service_uuid;
	std::string characteristic_uuid;
	_bt_operation operation;
	int packet_size = 0;				//bytes
};

namespace bt_link
{
	struct node
	{
		bool Initialize(_bt_peripheral_config in_peripheral_config, std::vector<_bt_service> in_Services);
		bool Reinitialize();
		bool Connect_Peripheral(SimpleBLE::Peripheral in_peripheral);
		int Use_Service(_bt_service in_service, std::string in_Data, std::string& out_Data);
		bool Get_Orientation(double& yaw, double& pitch, double& roll);
		bool Is_Initialized();
		std::string Get_Address();

	private:
		_bt_peripheral_config peripheral_config;
		std::vector<_bt_service> Services;
		SimpleBLE::Peripheral peripheral;
		bool is_initialized = false;
		bool print_on = true;

		//9-axis measurments
		double yaw{ 0 }, pitch{ 0 }, roll{ 0 };
		//double acc_x{ 0 }, acc_y{ 0 }, acc{ z };
		//double vel_x{ 0 }, vel_y{ 0 }, vel_z{ 0 };

		//device status
		double signal_strength{ 0 };
		bool connection_status{ false };
		double device_voltage{ 0 };

		bool Calc_Angle();
		double Calc_Power();
	};
	class source
	{
		_bt_adapter_config adapter_config;
		SimpleBLE::Adapter adapter;
		std::vector<node> Nodes;
		int max_nodes_allowed = 0;
		bool print_on{ true };
		bool is_initialized = false;

		public:
			bool Initialize(_bt_adapter_config in_adapter_config);
			bool Reinitialize();
			bool Add_Node(node& in_node);
			int Use_Service(std::string in_node_address, _bt_service in_service, std::string in_data, std::string& out_data);
			int Num_Nodes();
	};
}