#pragma once

#include "Auxiliary.h"
#include <string>
#include <CameraIF/inc/CamGenClass.hpp>

class OutputBlock
{
	std::vector<_camera> CamConfig;
	std::string assigned_cam = "NULL";
	int assigned_cam_index = -1;
	int count_pan{ -123456789 }, count_tilt{ -123456789 }, count_zoom{ -123456789 };

	CamIF::CamGenericClass* CAM = NULL;
	double TimeWindow(double step_size, double speed, double resolution);

	public:
		OutputBlock() {};
		~OutputBlock();
		bool Initialize(const std::string& config_file, std::string& message);
		bool Reinitialize();
		bool SendPTZ(_cam_command);
		bool QueryPTZ(std::string mechanism, double& position);
		bool GetStatus();
		bool Assign_Camera(const std::string& cam_ID);
};


