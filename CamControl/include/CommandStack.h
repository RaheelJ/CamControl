#pragma once

#include <Auxiliary.h>

// The code currently assumes zero (x, y) offset from the camera position
// Calculate required camera orientation from (camera state, asset state, camera state (absolute), target state)
// Calculate path (commands) to reach to the desired orientation using (current orientation, desired orientation, path type)
// Change the commands to track the target and control zoom
// Scan function to explore the surroundings 

class CommandBlock
{
	//Current state of camera driving mechanisms
	_cam_state last_cam_state, ref_cam_state;
	_asset last_asset_state;
	_target last_target_state;
	_region_interest last_region_interest;
	std::string path_type = "SHORTEST";
	std::string scan_type = "HELIX";
	double step_size_pan = 0.05;
	double step_size_tilt = 0.05;
	double step_size_zoom = 0.08;
	bool continuous_pan = "TRUE";
	double span_pan = 360;
	double span_tilt = 180;
	double zoom_span = 30;

	std::vector<_cam_command> CommandStack;

	bool Initialize(const std::string config_file, std::string& message);
	bool Reinitialize();

	bool ChangeZoom(int zoom_steps);
	bool GeneratePath();
	bool GenerateScanPath(_cam_state);
	bool AddCommands(int tilt_steps, int pan_steps, int zoom_steps);

	bool CalcRefState(_asset, _cam_state, _target);
	bool CalcRefState(_asset, _cam_state, _region_interest);
};

