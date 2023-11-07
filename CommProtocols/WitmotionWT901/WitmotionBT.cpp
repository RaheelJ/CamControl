#include <WitmotionWT901/WitmotionBT.hpp>
#include <SimpleBLE/Utils.h>
#include <thread>
#include <iostream>

std::string Hex2String(std::vector<uint16_t> in_Hex)
{
	std::string out_string;
	for (const auto& it : in_Hex)
	{
		out_string.push_back(it);
	}
	return out_string;
}
std::vector<uint16_t> String2Hex(std::string in_string)
{
	std::vector<uint16_t> out_Hex;
	for (const auto& it : in_string)
	{
		out_Hex.push_back(it);
	}
	return out_Hex;
}

bt_link::node::~node()
{
	Disconnect();
}
bool bt_link::node::Initialize(_bt_peripheral_config in_peripheral_config, std::vector<_bt_service> in_services)
{
	//peripheral_config = in_peripheral_config;
	peripheral_config.address = "db:f6:61:f1:0e:d3";
	peripheral_config.name= "WT901BLE68";

	//Services = in_services;
	_bt_service temp_service;
	temp_service.name = "notify_9_axis";
	temp_service.service_uuid = "0000ffe5-0000-1000-8000-00805f9a34fb";
	temp_service.characteristic_uuid = "0000ffe4-0000-1000-8000-00805f9a34fb";
	temp_service.packet_size = 247;
	temp_service.operation = NOTIFY;
	Services.push_back(temp_service);

	temp_service.name = "request_9_axis";
	temp_service.service_uuid = "0000ffe5-0000-1000-8000-00805f9a34fb";
	temp_service.characteristic_uuid = "0000ffe9-0000-1000-8000-00805f9a34fb";
	temp_service.packet_size = 247;
	temp_service.operation = WRITE_REQUEST;
	Services.push_back(temp_service);

	is_initialized = true;
	if (is_initialized && print_on)
	{
		//std::cout << "Node initialized: " << peripheral_config.uuid << ", " << peripheral_config.address << std::endl;
	}

	return is_initialized;
}
bool bt_link::node::Reinitialize()
{
	Disconnect();
	connection_status = false;

	yaw = 0;
	pitch = 0;
	roll = 0;

	signal_strength = 0;
	device_voltage = 0;

	if (print_on)
	{
		std::cout << "Node reinitialized: " << peripheral_config.name << ", " << peripheral_config.address << std::endl;
	}

	return true;
}
bool bt_link::node::Connect_Peripheral(SimpleBLE::Peripheral in_peripheral)
{
	bool status = false;
	std::vector<SimpleBLE::Service> All_Services;
	std::vector<SimpleBLE::Characteristic> All_Ch;

	peripheral = in_peripheral;
	peripheral.connect();
	if (peripheral.is_connected())
	{
		signal_strength = peripheral.rssi();
		connection_status = true;
		status = true;
		if (print_on)
		{
			std::cout << "Peripheral: " << peripheral.identifier() << ", " << peripheral.rssi() << ", " << peripheral.mtu() << std::endl;
			All_Services = peripheral.services();
			for (auto& it_service : All_Services)
			{
				std::cout << "Service: " << it_service.uuid() << ", " << it_service.data() << std::endl;
				
				All_Ch = it_service.characteristics();
				std::cout << "\t" << "Characteristics: " << std::endl;
				for (auto& it_ch : All_Ch)
				{
					std::cout << "\t" << it_ch.uuid() << ", R: " << it_ch.can_read() << ", N: " << it_ch.can_notify() << ", WC: " << it_ch.can_write_command() << ", WR: " << it_ch.can_write_request() << std::endl;
				}
			}
		}
	}
	else
	{
		connection_status = false;
		status = false;
		if (print_on)
		{
			std::cout << "Peripheral could not establish connection: " << peripheral_config.name << ", " << peripheral_config.address << std::endl;
		}
	}

	return status;
}
bool bt_link::node::Disconnect()
{
	if (peripheral.is_connected())
	{
		for (const auto& it_subscription : Subscriptions)
		{
			peripheral.unsubscribe(it_subscription.service_uuid, it_subscription.characteristic_uuid);
		}
		peripheral.disconnect();
		Subscriptions.clear();
		Notifications.clear();
	}
	return true;
}
int bt_link::node::Use_Service(std::string in_service_name, std::string in_Data, std::string& out_Data)
{
	bool found = false;
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
		for (const auto& it_service_user : Services)
		{
			if (it_service.uuid() == it_service_user.service_uuid && it_service_user.name == in_service_name)
			{
				for (auto& it_characteristic : it_service.characteristics())
				{
					if (it_characteristic.uuid() == it_service_user.characteristic_uuid)
					{
						found = true;
						switch (it_service_user.operation)
						{
						case READ:
							try
							{
								out_Data = peripheral.read(it_service_user.service_uuid, it_service_user.characteristic_uuid);
							}
							catch (std::string err)
							{
								if (print_on)
								{
									std::cout << err << std::endl;
								}
							}
							break;
						case WRITE_COMMAND:
							try
							{
								peripheral.write_command(it_service_user.service_uuid, it_service_user.characteristic_uuid, in_Data);
							}
							catch (std::string err)
							{
								if (print_on)
								{
									std::cout << err << std::endl;
								}
							}
							break;
						case WRITE_REQUEST:
							try 
							{
								peripheral.write_request(it_service_user.service_uuid, it_service_user.characteristic_uuid, in_Data);
							}
							catch (std::string err)
							{
								if (print_on)
								{
									std::cout << err << std::endl;
								}
							}
							break;
						case NOTIFY:
							for (const auto& it_subscription : Subscriptions)
							{
								if (it_subscription.characteristic_uuid == it_service_user.characteristic_uuid && it_subscription.service_uuid == it_service_user.service_uuid)
								{
									if (print_on)
									{
										std::cout << "Subscription already enabled !" << std::endl;
									}
									break;
								}
							}
							Subscriptions.push_back(it_service_user);
							Notifications.push_back("New_Subscription");
							switch (Subscriptions.size())
							{
								case 1:
									try
									{
										peripheral.notify(it_service_user.service_uuid, it_service_user.characteristic_uuid, [&](SimpleBLE::ByteArray Received_Bytes) {
											if (print_on)
											{
												std::vector<uint16_t> Hex_Data = String2Hex(Received_Bytes);
												std::cout << "Received Data: ";
												for (const auto& it : Hex_Data)
												{
													std::cout << it << " ";
												}
												std::cout << std::endl;
											}
											try
											{
												Notifications[0] = Received_Bytes;
											}
											catch (std::string err)
											{
												if (print_on)
												{
													std::cout << err << std::endl;
												}
											}});
									}
									catch (std::string err)
									{
										if (print_on)
										{
											std::cout << err << std::endl;
										}
									}
									break;
								case 2:
									try
									{
										peripheral.notify(it_service_user.service_uuid, it_service_user.characteristic_uuid, [&](SimpleBLE::ByteArray Received_Bytes) {
											if (print_on)
											{
												std::vector<uint16_t> Hex_Data = String2Hex(Received_Bytes);
												std::cout << "Received Data: ";
												for (const auto& it : Hex_Data)
												{
													std::cout << it << " ";
												}
												std::cout << std::endl;
											}
											Notifications[1] = Received_Bytes; });
									}
									catch (std::string err)
									{
										if (print_on)
										{
											std::cout << err << std::endl;
										}
									}
									break;
								default:
									if (print_on)
									{
										std::cout << "Number of subscriptions exceeded the limit !" << std::endl;
									}
									break;
							}
							break;
						default:
							break;
						}
						if (print_on)
						{
							//std::cout << "Service: " << in_service.service_uuid << in_service.characteristic_uuid << std::endl;
							//std::cout << "Received data: " << out_Data << std::endl;
						}
						break;
					}
				}
				if (found)
				{
					break;
				}
			}
		}
		if (found)
		{
			break;
		}
	}

	if (!found)
	{
		if (print_on)
		{
			std::cout << "Service not found: " << in_service_name << std::endl;
		}
	}

	//Calculate the orientation


	return 0;
}
bool bt_link::node::Get_Orientation(double& out_yaw, double& out_pitch, double& out_roll)
{
	std::vector<uint16_t> HexArray;
	if (is_initialized)
	{
		for (int i = 0; i < Subscriptions.size(); i++)
		{
			if (Subscriptions[i].name == "notify_9_axis")
			{
				HexArray = String2Hex(Notifications[i]);
				yaw = Calc_Value(HexArray[19], HexArray[18], 180);			//deg(-180 to 180)
				pitch = Calc_Value(HexArray[17], HexArray[16], 180);		//deg(-180 to 180)
				roll = Calc_Value(HexArray[15], HexArray[14], 180);			//deg(-180 to 180)
				break;
			}
		}
		out_yaw = yaw;
		out_pitch = pitch;
		out_roll = roll;
		if (print_on)
		{
			std::cout << "Euler Angles:  " << "Y: " << yaw << "  P: " << pitch << "  R: " << roll << std::endl;
		}
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
std::string bt_link::node::Get_Name()
{
	return peripheral_config.name;
}
double bt_link::node::Calc_Value(uint16_t h_byte, uint16_t l_byte, double max_value)
{
	uint16_t temp_1 = (h_byte << 8) | l_byte;
	short temp_2 = (short)temp_1;
	double out_value = (double)temp_2 * max_value / 32768;
	return out_value;
}

bt_link::source::~source()
{
	Nodes.clear();
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
		if (it_adapter.address() == adapter_config.address && it_adapter.identifier() == adapter_config.name)
		{
			adapter = it_adapter;
			found = true;
		}
	}
	if (!found)
	{
		adapter = temp_adapters[0];
		adapter_config.address = adapter.address();
		adapter_config.name = adapter.identifier();
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
		it_node->Reinitialize();
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
		if (it_node->Get_Name() == in_node.Get_Name())
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
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		temp_peripherals = adapter.scan_get_results();
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		for (auto it_peripheral : temp_peripherals)
		{
			if (it_peripheral.identifier() == in_node.Get_Name())
			{
				status = in_node.Connect_Peripheral(it_peripheral);
				Nodes.push_back(&in_node);
			}
		}
	}

	return status;
}
bool bt_link::source::Delete_Node(std::string in_name)
{
	bool found = false;
	for (int it_node=0 ; it_node<Nodes.size(); it_node++)
	{
		if (in_name == Nodes[it_node]->Get_Name())
		{
			Nodes[it_node]->Disconnect();
			Nodes.erase(Nodes.begin() + it_node);
			found = true;
			break;
		}
	}
	if (!found)
	{
		if (print_on)
		{
			std::cout << "Node not found. Can't be deleted !" << std::endl;
		}
	}
	return true;
}
int bt_link::source::Use_Service(std::string in_node_name, std::string in_service_name, std::string in_data, std::string& out_data)
{
	bool found = false;
	int status = 0;

	for (auto& it_node : Nodes)
	{
		if (it_node->Get_Name() == in_node_name)
		{
			found = true;
			status = it_node->Use_Service(in_service_name, in_data, out_data);
			break;
		}
	}
	if (!found)
	{
		status = -1;
		if (print_on)
		{
			std::cout << "Node not found. Can't use the service !" << std::endl;
		}
	}

	return status;
}
int bt_link::source::Num_Nodes()
{
	return Nodes.size();
}