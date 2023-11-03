#pragma once
#include <simpleble/SimpleBLE.h>
#include <string>

enum _bt_operation
{
	READ,
	WRITE_REQUEST,
	WRITE_COMMAND,
	NOTIFY
};
struct _bt_adapter_config
{
	std::string name;
	std::string address;
	double scan_time = 0;				//ms
	int max_nodes_allowed = 1;
};
struct _bt_peripheral_config
{
	std::string name;
	std::string address;
};
struct _bt_service
{
	std::string name;
	std::string service_uuid;
	std::string characteristic_uuid;
	_bt_operation operation;
	int packet_size = 0;				//bytes
};

std::string Hex2String(std::vector<uint16_t> in_Hex);
std::vector<uint16_t> String2Hex(std::string in_string);

class bt_node_template
{
	virtual bool Initialize(_bt_peripheral_config in_peripheral_config, std::vector<_bt_service> in_Services) = 0;
	virtual bool Reinitialize() = 0;
	virtual bool Connect_Peripheral(SimpleBLE::Peripheral in_peripheral) = 0;;
	virtual bool Disconnect() = 0;
	virtual int Use_Service(std::string in_service_name, std::string in_Data, std::string& out_Data) = 0;
	virtual bool Is_Initialized() = 0;
	virtual std::string Get_Name() = 0;
};

namespace bt_link
{
	class node :public bt_node_template
	{
	public:
		node() {};
		~node();
		bool Initialize(_bt_peripheral_config in_peripheral_config, std::vector<_bt_service> in_Services);
		bool Reinitialize();
		bool Connect_Peripheral(SimpleBLE::Peripheral in_peripheral);
		bool Disconnect();
		int Use_Service(std::string in_service_name, std::string in_Data, std::string& out_Data);
		bool Get_Orientation(double& yaw, double& pitch, double& roll);
		bool Is_Initialized();
		std::string Get_Name();

	private:
		_bt_peripheral_config peripheral_config;
		std::vector<_bt_service> Services;
		SimpleBLE::Peripheral peripheral;
		const int max_subscriptions = 2;
		std::vector<std::string> Notifications;
		std::vector<_bt_service> Subscriptions;
		bool is_initialized = false;
		bool print_on = true;
		//9-axis measurments
		double yaw{ 0 }, pitch{ 0 }, roll{ 0 };
		double acc_x{ 0 }, acc_y{ 0 }, acc_z{ 0 };
		double vel_x{ 0 }, vel_y{ 0 }, vel_z{ 0 };

		//device status
		double signal_strength{ 0 };
		bool connection_status{ false };
		double device_voltage{ 0 };

		double Calc_Value(uint16_t h_byte, uint16_t l_byte, double max_value);
		double Calc_Power();
	};
	class source
	{
		_bt_adapter_config adapter_config;
		SimpleBLE::Adapter adapter;
		std::vector<node*> Nodes;
		int max_nodes_allowed = 0;
		bool print_on{ true };
		bool is_initialized = false;

		public:
			source() {};
			~source();
			bool Initialize(_bt_adapter_config in_adapter_config);
			bool Reinitialize();
			bool Add_Node(node& in_node);
			bool Delete_Node(std::string in_name);
			int Use_Service(std::string in_node_address, std::string in_service_name, std::string in_data, std::string& out_data);
			int Num_Nodes();
	};
}

