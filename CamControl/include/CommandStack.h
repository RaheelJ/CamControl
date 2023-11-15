#pragma once

#include <Auxiliary.h>
#include <NetworkedOutput.h>
#include <map>

// The code currently assumes zero (x, y) offset from the camera position
// Calculate required camera orientation from (camera state, asset state, camera state (absolute), target state)
// Calculate path (commands) to reach to the desired orientation using (current orientation, desired orientation, path type)
// Change the commands to track the target and control zoom
// Scan function to explore the surroundings 

class CommandBlock
{
	bool print_out = true;
	OutputBlock output_block;

	//Current state of camera driving mechanisms
	_cam_state last_cam_state, ref_cam_state;
	_asset last_asset_state;
	_target last_target_state;
	_region_interest last_region_interest;

	double sampling_time{ 1 };
	std::string path_type = "SHORTEST";
	std::string scan_type = "HELIX";
	double step_size_pan = 0.05;
	double step_size_tilt = 0.05;
	double step_size_zoom = 1.0;
	bool continuous_pan = "TRUE";
	double span_pan = 360;
	double span_tilt = 180;
	double zoom_span = 30;
	double speed_max = 90;		//deg/s
	double speed_min = 0.05;	//deg/s
	std::string control_mode = "FUZZY";

	std::vector<_cam_command> CommandStack;

	std::vector<std::string> Rules_String_Pan;
	std::vector<std::string> Rules_String_Tilt;
	std::vector<_rule_fuzzy> Rules_Pan;
	std::vector<_rule_fuzzy> Rules_Tilt;
	bool Parse_Rules(std::vector<std::string> in_Rules_String, std::vector<_rule_fuzzy>& out_Rules);
	bool Set_Rules(std::vector<_rule_fuzzy> in_Rules_Pan, std::vector<_rule_fuzzy> in_Rules_Tilt);

	//std::map<double, double> LogicMap_Pan, LogicMap_Tilt;
	//bool Create_LogicMap(std::vector<_rule_fuzzy> in_Rules, std::map<double, double> out_LogicMap);

	double last_speedref_pan{ 0 }, last_speedref_tilt{ 0 };
	double last_speedrate_pan{ 0 }, last_speedrate_tilt{ 0 };
	double upper_speed{ 80 }, lower_speed{ 4 };
	double max_speed_rate{ 80 };
	double Fuzzy_Control(double in_error, std::vector<_rule_fuzzy> in_Rules);
	std::vector<double> Update_SpeedRef(double in_speedrate_pan, double in_speedrate_tilt);
	bool AddCommands(int tilt_steps, int pan_steps, int zoom_steps, double in_speed_pan, double in_speed_tilt);
	bool ChangeZoom(int zoom_steps);

public:
	bool Initialize(const std::string config_file, std::string& message);
	bool Reinitialize();
	bool Assign_Camera(std::string cam_ID);
	bool CalcRefState(_asset, _cam_state, _target);
	bool CalcRefState(_asset, _cam_state, _region_interest);
	bool GeneratePath();
	bool GenerateScanPath(_cam_state);
	bool FollowPath();
};

