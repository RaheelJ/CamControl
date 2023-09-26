#include <NetworkedOutput.h>
#include <CameraIF/inc/PelcoDTCP.hpp>
#include <CameraIF/inc/ONVIF.hpp>
#include <pugixml/pugixml.hpp>
#include <iostream>
#include <chrono>
#include <thread>

OutputBlock::~OutputBlock()
{
	delete CAM;
}

bool OutputBlock::Initialize(const std::string& config_file, std::string& message)
{
	pugi::xml_document Doc;
	int error = 0;
	try
	{
		pugi::xml_parse_result Result = Doc.load_file(config_file.c_str());		/* Read "config_file" */
		if (Result) {}
		else {
			throw "File not found or formatting error!";	                    /* Throw exception if file not read */
		}
	}
	catch (const char* error_2)
	{
		message = "File not found or formatting error!";
		std::cerr << error_2 << "\n";							/* Print the error message */
		return false;
	}

	pugi::xml_node Root = Doc.child("CamControl");				/* Accessing Root Node */
	if (Root) {}
	else {
		message = "Root node not found!";
		return false;
	}

	_camera temp_cam;
	try
	{
		for (pugi::xml_node CameraConfig = Root.child("CameraManager"); CameraConfig; CameraConfig = CameraConfig.next_sibling("CameraManager"))
		{
			temp_cam.ID = CameraConfig.child_value("cam_name");
			temp_cam.asset_ID = CameraConfig.child_value("asset");
			temp_cam.initial_state.offset_x = ReadParam(std::stod(CameraConfig.child_value("cam_offset_x")), 0.0, 1e8, error);
			temp_cam.initial_state.offset_y = ReadParam(std::stod(CameraConfig.child_value("cam_offset_y")), 0.0, 1e8, error);
			temp_cam.type = CameraConfig.child("CameraSpecifications").child_value("cam_type");
			temp_cam.aspect_ratio[0] = std::stod(ReadSpacedParam(CameraConfig.child("CameraSpecifications").child_value("aspect_ratio"), 0, ','));
			temp_cam.aspect_ratio[1] = std::stod(ReadSpacedParam(CameraConfig.child("CameraSpecifications").child_value("aspect_ratio"), 1, ','));
			//temp_cam.night_vision = ((std::string)CameraConfig.child("CameraSpecifications").child_value("night_vision") == "true") ? true : false;
			//temp_cam.optical_zoom = ((std::string)CameraConfig.child("CameraSpecifications").child_value("optical_zoom") == "true") ? true : false;

			if (temp_cam.optical_zoom)
			{
				temp_cam.zoom.zoom_step = ReadParam(std::stod(CameraConfig.child("CameraSpecifications").child("OpticalZoom").child_value("zoom_step")), 0.0001, 1e8, error);
				temp_cam.zoom.max_x = ReadParam(std::stod(CameraConfig.child("CameraSpecifications").child("OpticalZoom").child_value("max_zoom")), 0.0001, 1e8, error);
				temp_cam.zoom.max_FOV_horizontal = ReadParam(std::stod(CameraConfig.child("CameraSpecifications").child("OpticalZoom").child_value("FOV_horizontal_max")), 0.0001, 1e8, error);
				temp_cam.zoom.min_FOV_horizontal = ReadParam(std::stod(CameraConfig.child("CameraSpecifications").child("OpticalZoom").child_value("FOV_horizontal_min")), 0.0001, temp_cam.zoom.max_FOV_horizontal, error);
			}

			temp_cam.resolution = ReadParam(std::stod(CameraConfig.child("PTZsettings").child_value("resolution")), 0.0001, 359.9999, error);
			temp_cam.pan.span = ReadParam(std::stod(CameraConfig.child("PTZsettings").child("Pan").child_value("span_pan")), 0.0001, 360.0, error);
			temp_cam.pan.step_size = ReadParam(std::stod(CameraConfig.child("PTZsettings").child("Pan").child_value("step_size_pan")), temp_cam.resolution, 360.0, error);
			temp_cam.pan.continuous = ((std::string)CameraConfig.child("PTZsettings").child("Pan").child_value("continuous_pan") == "true") ? true : false;
			temp_cam.tilt.span = ReadParam(std::stod(CameraConfig.child("PTZsettings").child("Tilt").child_value("span_tilt")), 0.0001, 360.0, error);
			temp_cam.tilt.step_size = ReadParam(std::stod(CameraConfig.child("PTZsettings").child("Tilt").child_value("step_size_tilt")), temp_cam.resolution, 360.0, error);
			temp_cam.tilt.continuous = ((std::string)CameraConfig.child("PTZsettings").child("Tilt").child_value("continuous_tilt") == "true") ? true : false;
			temp_cam.speed.adaptive = ((std::string)CameraConfig.child("PTZsettings").child("Speed").child_value("speed_control") == "adaptive") ? true : false;
			temp_cam.speed.max_value = ReadParam(std::stod(CameraConfig.child("PTZsettings").child("Speed").child_value("max_speed")), temp_cam.resolution, 1e8, error);
			if (!temp_cam.speed.adaptive)
			{
				temp_cam.speed.fixed_value = ReadParam(std::stod(CameraConfig.child("PTZsettings").child("Speed").child_value("fixed_speed")), temp_cam.resolution, temp_cam.speed.max_value, error);
			}

			temp_cam.initial_state.height = ReadParam(std::stod(CameraConfig.child("InitialState").child_value("height")), 0.0, 1e8, error);
			temp_cam.initial_state.pan = ReadParam(std::stod(CameraConfig.child("InitialState").child_value("pan")), 0.0, temp_cam.pan.span, error);
			temp_cam.initial_state.tilt = ReadParam(std::stod(CameraConfig.child("InitialState").child_value("tilt")), 0.0, temp_cam.tilt.span, error);
			if (temp_cam.optical_zoom)
			{
				temp_cam.initial_state.zoom = ReadParam(std::stod(CameraConfig.child("InitialState").child_value("zoom")), 0.0, temp_cam.zoom.max_x, error);
				temp_cam.initial_state.FOV_horizontal = ReadParam(std::stod(CameraConfig.child("InitialState").child_value("FOV_horizontal")), temp_cam.zoom.min_FOV_horizontal, temp_cam.zoom.max_FOV_horizontal, error);
			}
			temp_cam.initial_state.ref_azimuth = ReadParam(std::stod(CameraConfig.child("InitialState").child("Reference").child_value("azimuth")), -180.0, 180.0, error);
			temp_cam.initial_state.ref_elevation = ReadParam(std::stod(CameraConfig.child("InitialState").child("Reference").child_value("elevation")), -90.0, 90.0, error);

			temp_cam.protocol = CameraConfig.child("CommSettings").child_value("comm_protocol");
			if (temp_cam.protocol == "PELCO_D")
			{
				temp_cam.cam_address = ReadParam(std::stoi(CameraConfig.child("CommSettings").child_value("cam_address")), 0.0, 256.0, error);
				temp_cam.serial.comm_port = CameraConfig.child("CommSettings").child("SerialSettings").child_value("cam_address");
				temp_cam.serial.baud_rate = ReadParam(std::stoi(CameraConfig.child("CommSettings").child("SerialSettings").child_value("baudrate")), 2400, 1e8, error);
				temp_cam.serial.timeout = ReadParam(std::stoi(CameraConfig.child("CommSettings").child("SerialSettings").child_value("timeout")), 0.0001, 1e8, error);
			}
			else if (temp_cam.protocol == "PELCO_D_TCP")
			{
				temp_cam.cam_address = ReadParam(std::stoi(CameraConfig.child("CommSettings").child_value("cam_address")), 0.0, 256.0, error);
				temp_cam.network.IP = CameraConfig.child("CommSettings").child("NetworkSettings").child_value("IP");
				temp_cam.network.port = ReadParam(std::stod(CameraConfig.child("CommSettings").child("NetworkSettings").child_value("port")), 0, 255, error);
				temp_cam.network.username = CameraConfig.child("CommSettings").child("NetworkSettings").child_value("username");
				temp_cam.network.password = CameraConfig.child("CommSettings").child("NetworkSettings").child_value("password");
			}
			else if (temp_cam.protocol == "ONVIF")
			{
				temp_cam.network.IP = CameraConfig.child("CommSettings").child("NetworkSettings").child_value("IP");
				temp_cam.network.port = ReadParam(std::stod(CameraConfig.child("CommSettings").child("NetworkSettings").child_value("port")), 0, 255, error);
				temp_cam.network.username = CameraConfig.child("CommSettings").child("NetworkSettings").child_value("username");
				temp_cam.network.password = CameraConfig.child("CommSettings").child("NetworkSettings").child_value("password");
			}

			CamConfig.push_back(temp_cam);
		}
	}
	catch (std::string error_param)
	{
		message = "Could not initialize. Some parameters are not accessible !";
		std::cerr << error_param << "\n";							/* Print the error message */
		return false;
	}

	/* Check for out of range parameters */
	if (error == 1) {
		message = std::to_string(error) + " parameter is out of range";
		return false;
	}
	else if (error > 1) {
		message = std::to_string(error) + " parameters are out of range";
		return false;
	}

	return true;
}
bool OutputBlock::Reinitialize()
{
	assigned_cam_index = -1;
	assigned_cam = "NULL";

	return true;
}

bool OutputBlock::Assign_Camera(const std::string& cam_ID)
{
	bool found = false;
	int it = 0;

	for (it=0; it<CamConfig.size(); it++)
	{
		if (CamConfig[it].ID == cam_ID)
		{
			assigned_cam = cam_ID;
			assigned_cam_index = it;
			found = true;
			break;
		}
	}

	if (found)
	{
		if (CamConfig[assigned_cam_index].protocol == "PELCO_D_TCP")
		{
			CAM = new CamIF::CamIF_PelcoDTCP(CamConfig[assigned_cam_index].network.IP.c_str(), std::to_string(CamConfig[assigned_cam_index].network.port).c_str(), CamConfig[assigned_cam_index].cam_address);
			CAM->CameraOn();

			count_pan = 0;
			count_tilt = 0;
			count_zoom = 0;
		}
		else if (CamConfig[assigned_cam_index].protocol == "PELCO_D_TCP")
		{
			CAM = new CamIF::CamIF_ONVIF(CamConfig[assigned_cam_index].network.IP.c_str(), std::to_string(CamConfig[assigned_cam_index].network.port).c_str(), CamConfig[assigned_cam_index].network.username.c_str(), CamConfig[assigned_cam_index].network.password.c_str(), CamConfig[assigned_cam_index].profile.c_str());
			CAM->CameraOn();

			count_pan = 0;
			count_tilt = 0;
			count_zoom = 0;
		}
		return true;
	}
	
	return false;
}
double OutputBlock::TimeWindow(double step_size, double speed, double resolution)
{
	double time = -1;
	
	if (step_size > 0 && speed > 0 && step_size >= resolution && speed >= resolution)
	{
		int num_steps = std::round(step_size / resolution);
		double rounded_step_size = resolution * num_steps;
		int num_speed_steps = std::round(speed / resolution);
		double rounded_speed = num_speed_steps * resolution;

		time = rounded_step_size / rounded_speed;
	}

	return time;
}

bool OutputBlock::SendPTZ(_cam_command command)
{
	double time = -1;
	unsigned int ms_time = 0;

	if (CAM)
	{
		switch (command)
		{
		case LEFT:
			time = TimeWindow(CamConfig[assigned_cam_index].pan.step_size, CamConfig[assigned_cam_index].speed.fixed_value, CamConfig[assigned_cam_index].resolution);
			ms_time = (unsigned int)round(time * 1000);
			CAM->CamLeft(CamConfig[assigned_cam_index].pan.step_size, CamConfig[assigned_cam_index].speed.fixed_value);
			std::this_thread::sleep_for(std::chrono::milliseconds(ms_time));
			CAM->CamStop();
			count_pan--;
			break;
		case RIGHT:
			time = TimeWindow(CamConfig[assigned_cam_index].pan.step_size, CamConfig[assigned_cam_index].speed.fixed_value, CamConfig[assigned_cam_index].resolution);
			ms_time = (unsigned int)round(time * 1000);
			CAM->CamRigth(CamConfig[assigned_cam_index].pan.step_size, CamConfig[assigned_cam_index].speed.fixed_value);
			std::this_thread::sleep_for(std::chrono::milliseconds(ms_time));
			CAM->CamStop();
			count_pan++;
			break;
		case UP:
			time = TimeWindow(CamConfig[assigned_cam_index].tilt.step_size, CamConfig[assigned_cam_index].speed.fixed_value, CamConfig[assigned_cam_index].resolution);
			ms_time = (unsigned int)round(time * 1000);
			CAM->CamUp(CamConfig[assigned_cam_index].tilt.step_size, CamConfig[assigned_cam_index].speed.fixed_value);
			std::this_thread::sleep_for(std::chrono::milliseconds(ms_time));
			CAM->CamStop();
			count_tilt++;
			break;
		case DOWN:
			time = TimeWindow(CamConfig[assigned_cam_index].tilt.step_size, CamConfig[assigned_cam_index].speed.fixed_value, CamConfig[assigned_cam_index].resolution);
			ms_time = (unsigned int)round(time * 1000);
			CAM->CamDown(CamConfig[assigned_cam_index].tilt.step_size, CamConfig[assigned_cam_index].speed.fixed_value);
			std::this_thread::sleep_for(std::chrono::milliseconds(ms_time));
			CAM->CamStop();
			count_tilt--;
			break;
		case UP_LEFT:
			time = TimeWindow(CamConfig[assigned_cam_index].tilt.step_size, CamConfig[assigned_cam_index].speed.fixed_value, CamConfig[assigned_cam_index].resolution);
			ms_time = (unsigned int)round(time * 1000);
			CAM->CamUpLeft(CamConfig[assigned_cam_index].pan.step_size, CamConfig[assigned_cam_index].speed.fixed_value, CamConfig[assigned_cam_index].tilt.step_size, CamConfig[assigned_cam_index].speed.fixed_value);
			std::this_thread::sleep_for(std::chrono::milliseconds(ms_time));
			count_tilt++;
			count_pan--;
			CAM->CamStop();
			break;
		case UP_RIGHT:
			time = TimeWindow(CamConfig[assigned_cam_index].tilt.step_size, CamConfig[assigned_cam_index].speed.fixed_value, CamConfig[assigned_cam_index].resolution);
			ms_time = (unsigned int)round(time * 1000);
			CAM->CamUpRight(CamConfig[assigned_cam_index].pan.step_size, CamConfig[assigned_cam_index].speed.fixed_value, CamConfig[assigned_cam_index].tilt.step_size, CamConfig[assigned_cam_index].speed.fixed_value);
			std::this_thread::sleep_for(std::chrono::milliseconds(ms_time));
			CAM->CamStop();
			count_tilt++;
			count_pan++;
			break;
		case DOWN_LEFT:
			time = TimeWindow(CamConfig[assigned_cam_index].tilt.step_size, CamConfig[assigned_cam_index].speed.fixed_value, CamConfig[assigned_cam_index].resolution);
			ms_time = (unsigned int)round(time * 1000);
			CAM->CamDownLeft(CamConfig[assigned_cam_index].pan.step_size, CamConfig[assigned_cam_index].speed.fixed_value, CamConfig[assigned_cam_index].tilt.step_size, CamConfig[assigned_cam_index].speed.fixed_value);
			std::this_thread::sleep_for(std::chrono::milliseconds(ms_time));
			CAM->CamStop();
			count_tilt--;
			count_pan--;
			break;
		case DOWN_RIGHT:
			time = TimeWindow(CamConfig[assigned_cam_index].tilt.step_size, CamConfig[assigned_cam_index].speed.fixed_value, CamConfig[assigned_cam_index].resolution);
			ms_time = (unsigned int)round(time * 1000);
			CAM->CamDownRight(CamConfig[assigned_cam_index].pan.step_size, CamConfig[assigned_cam_index].speed.fixed_value, CamConfig[assigned_cam_index].tilt.step_size, CamConfig[assigned_cam_index].speed.fixed_value);
			std::this_thread::sleep_for(std::chrono::milliseconds(ms_time));
			CAM->CamStop();
			count_tilt--;
			count_pan++;
			break;
		case ZOOM_IN:
			ms_time = (unsigned int)round(CamConfig[assigned_cam_index].zoom.zoom_step * 1000);
			CAM->ZoomTele();
			std::this_thread::sleep_for(std::chrono::milliseconds(ms_time));
			CAM->ZoomStop();
			count_zoom++;
			break;
		case ZOOM_OUT:
			ms_time = (unsigned int)round(CamConfig[assigned_cam_index].zoom.zoom_step * 1000);
			CAM->ZoomWide();
			std::this_thread::sleep_for(std::chrono::milliseconds(ms_time));
			CAM->ZoomStop();
			count_zoom--;
			break;
		default:
			break;
		}
	}
	return true;
}
bool OutputBlock::QueryPTZ(std::string mechanism, double& position)
{
	if (mechanism == "PAN")
	{
		CAM->CamQueryPan();
		std::this_thread::sleep_for(std::chrono::milliseconds(300));
		position = count_pan * CamConfig[assigned_cam_index].pan.step_size;
	}
	else if(mechanism == "TILT")
	{
		CAM->CamQueryTilt();
		std::this_thread::sleep_for(std::chrono::milliseconds(300));
		position = count_tilt * CamConfig[assigned_cam_index].tilt.step_size;
	}
	else
	{
		CAM->CamQueryZoom();
		std::this_thread::sleep_for(std::chrono::milliseconds(300));
		position = count_zoom * CamConfig[assigned_cam_index].zoom.zoom_step;
	}

	return true;
}
bool OutputBlock::GetStatus()
{
	std::this_thread::sleep_for(std::chrono::milliseconds(300));
	return true;
}


