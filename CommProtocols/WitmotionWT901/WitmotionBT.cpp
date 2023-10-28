#include <WitmotionWT901/WitmotionBT.hpp>
#include <SimpleBLE/Utils.h>
#include <iostream>

bool bt_link::node::Initialize(_bt_peripheral_config in_peripheral_config, std::vector<_bt_service> in_services)
{
	//peripheral_config = in_peripheral_config;
	peripheral_config.address = "db:f6:61:f1:0e:d3";
	peripheral_config.uuid= "WT901BLE68";
	peripheral_config.notification_time = 1000;			//ms
	
	//Services = in_services;
	_bt_service temp_service;
	temp_service.service_uuid = "0000ffe5-0000-1000-8000-00805f9a34fb";
	temp_service.characteristic_uuid = "0000ffe4-0000-1000-8000-00805f9a34fb";
	//temp_service.characteristic_uuid = "0000ffe90000-1000-8000-00805f9a34fb";
	temp_service.packet_size = 247;
	temp_service.operation = READ;
	Services.push_back(temp_service);

	is_initialized = true;
	if (is_initialized && print_on)
	{
		std::cout << "Node initialized: " << peripheral_config.uuid << ", " << peripheral_config.address << std::endl;
	}

	return is_initialized;
}
bool bt_link::node::Reinitialize()
{
	peripheral.disconnect();
	connection_status = false;

	yaw = 0;
	pitch = 0;
	roll = 0;

	signal_strength = 0;
	device_voltage = 0;

	if (print_on)
	{
		std::cout << "Node reinitialized: " << peripheral_config.uuid << ", " << peripheral_config.address << std::endl;
	}

	return true;
}
bool bt_link::node::Connect_Peripheral(SimpleBLE::Peripheral in_peripheral)
{
	bool status = false;

	peripheral = in_peripheral;
	peripheral.connect();
	if (peripheral.is_connected())
	{
		signal_strength = peripheral.rssi();
		connection_status = true;
		status = true;
		if (print_on)
		{
			std::cout << "Peripheral connected: " << peripheral.identifier() << ", " << peripheral.rssi() << ", " << peripheral.mtu() << std::endl;
		}
	}
	else
	{
		connection_status = false;
		status = false;
		if (print_on)
		{
			std::cout << "Peripheral could not establish connection: " << peripheral_config.uuid << ", " << peripheral_config.address << std::endl;
		}
	}

	return status;
}
int bt_link::node::Use_Service(_bt_service in_service, std::string in_Data, std::string& out_Data)
{
	bool found = false;
	SimpleBLE::ByteArray RX_Data;
	SimpleBLE::ByteArray TX_Data = in_Data;

	if (!peripheral.is_connected())
	{
		if (print_on)
		{
			std::cout << "Peripheral not connected: " << peripheral.identifier() << ", " << peripheral.address() << std::endl;
		}
		return -1;
	}

	for (auto& it_service : peripheral.services())
	{
		if (it_service.uuid() == in_service.service_uuid)
		{
			for (const auto& it_service_user : Services)
			{
				if (it_service.uuid() == it_service_user.service_uuid)
				{
					for (auto& it_characteristic : it_service.characteristics())
					{
						if (it_characteristic.uuid() == in_service.characteristic_uuid)
						{
							found = true;
							RX_Data = (in_service.operation == READ) ? peripheral.read(in_service.service_uuid, in_service.characteristic_uuid) : "NIL";
							out_Data = RX_Data;
							if (print_on)
							{
								std::cout << "Service: " << in_service.service_uuid << in_service.characteristic_uuid << std::endl;
								std::cout << "Received data: " << out_Data << std::endl;
							}
							//peripheral.write_request(in_service.service_uuid, in_service.characteristic_uuid, TX_Data);
						}
					}
				}
			}
		}
	}
	if (!found)
	{
		if (print_on)
		{
			std::cout << "Service not found: " << in_service.service_uuid << in_service.characteristic_uuid << std::endl;
		}
	}

	//Calculate the orientation


	return 0;
}
bool bt_link::node::Get_Orientation(double& out_yaw, double& out_pitch, double& out_roll)
{
	if (is_initialized)
	{
		out_yaw = yaw;
		out_pitch = pitch;
		out_roll = roll;
	}
	else
	{
		return false;
	}

	return true;
}
bool bt_link::node::Is_Initialized()
{
	return is_initialized;
}
std::string bt_link::node::Get_Address()
{
	return peripheral_config.address;
}

bool bt_link::source::Initialize(_bt_adapter_config in_adapter_config)
{
	bool found = false;
	adapter_config = in_adapter_config;

	if (!SimpleBLE::Adapter::bluetooth_enabled()) 
	{
		if (print_on)
		{
			std::cout << "Bluetooth is not enabled" << std::endl;
		}
		return false;
	}

	auto temp_adapters = SimpleBLE::Adapter::get_adapters();
	if (temp_adapters.empty()) 
	{
		if (print_on)
		{
			std::cout << "No Bluetooth adapters found" << std::endl;
		}
		return false;
	}

	// Use the first adapter
	for (auto& it_adapter : temp_adapters)
	{
		if (it_adapter.address() == adapter_config.address && it_adapter.identifier() == adapter_config.uuid)
		{
			adapter = it_adapter;
			found = true;
		}
	}
	if (!found)
	{
		adapter = temp_adapters[0];
		adapter_config.address = adapter.address();
		adapter_config.uuid = adapter.identifier();
	}
	is_initialized = true;

	if (print_on)
	{
		std::cout << "Adapter: " << adapter.identifier() << ", " << adapter.address() << std::endl;
	}

	return is_initialized;	
}
bool bt_link::source::Reinitialize()
{
	Initialize(adapter_config);
	for (auto& it_node : Nodes)
	{
		it_node.Reinitialize();
	}
	Nodes.clear();
	return true;
}
bool bt_link::source::Add_Node(node& in_node)
{
	bool found = false;
	bool status = false;
	std::vector<SimpleBLE::Peripheral> temp_peripherals;

	for (auto& it_node : Nodes)
	{
		if (it_node.Get_Address() == in_node.Get_Address())
		{
			status = true;
			found = true;
			if (print_on)
			{
				std::cout << "Node is already added !" << std::endl;
			}
		}
	}
	if (!found && in_node.Is_Initialized())
	{
		adapter.scan_for(adapter_config.scan_time);
		temp_peripherals = adapter.scan_get_results();
		for (auto it_peripheral : temp_peripherals)
		{
			if (it_peripheral.address() == in_node.Get_Address())
			{
				status = in_node.Connect_Peripheral(it_peripheral);
				Nodes.push_back(in_node);
			}
		}
	}

	return status;
}
int bt_link::source::Use_Service(std::string in_node_address, _bt_service in_service, std::string in_data, std::string& out_data)
{
	bool found = false;
	int status = 0;

	for (auto& it_node : Nodes)
	{
		if (it_node.Get_Address() == in_node_address)
		{
			found = true;
			status = it_node.Use_Service(in_service, in_data, out_data);
		}
	}
	if (!found)
	{
		status = -1;
	}

	return status;
}
int bt_link::source::Num_Nodes()
{
	return Nodes.size();
}