#include <CommandStack.h>
#include "../libs/pugixml/pugixml.hpp"
#include <iostream>
#include <sstream>

// The code currently assumes zero (x, y) offset from the camera position
// Calculate required camera orientation from (camera state, asset state, camera state (absolute), target state)
// Calculate path (commands) to reach to the desired orientation using (current orientation, desired orientation, path type)
// Change the commands to track the target and control zoom
// Scan function to explore the surroundings 

bool CommandBlock::Initialize(const std::string config_file, std::string& message)
{
	return true;
}
bool CommandBlock::Reinitialize()
{
	CommandStack.clear();
	return true;
}

bool CommandBlock::Parse_Rules(std::vector<std::string> in_Rules_String, std::vector<_rule_fuzzy>& out_Rules)
{
	bool status = true;
	std::stringstream rule_stream;
	std::string rule_element;
	int i = 0, temp_operator;
	_rule_fuzzy temp_rule;
	out_Rules.clear();

	for (const auto& it_rule : in_Rules_String)
	{
		rule_stream << it_rule;
		while (rule_stream)
		{
			std::getline(rule_stream, rule_element, ',');
			switch (i)
			{
			case 0:
				temp_rule.err_variable = rule_element;
				break;
			case 1:
				temp_operator = 0*(rule_element == "<") + 1*(rule_element == ">") + 2*(rule_element == "<=") + 3*(rule_element == ">=") + 4*(rule_element == "=");
				temp_rule.comp_operator = (_operator)temp_operator;
				break;
			case 2:
				try
				{
					temp_rule.ref_err = std::stod(rule_element);
				}
				catch (std::string err_string)
				{
					if (print_out)
					{
						std::cout << "Rule not entered correctly !" << std::endl;
					}
					status = false;
					temp_rule.ref_err = 0;
				}
				break;
			case 3:
				try
				{
					temp_rule.out_rate = std::stod(rule_element);
				}
				catch (std::string err_string)
				{
					if (print_out)
					{
						std::cout << "Rule not entered correctly !" << std::endl;
					}
					status = false;
					temp_rule.out_rate = 0;
				}
				break;
			default:
				break;
			}
			i++;
		}
		i = 0;
		out_Rules.push_back(temp_rule);
	}

	return true;
}
bool CommandBlock::Set_Rules(std::vector<_rule_fuzzy> in_Rules_Pan, std::vector<_rule_fuzzy> in_Rules_Tilt)
{
	bool status = false;
	if (!in_Rules_Pan.empty())
	{
		Rules_Pan = in_Rules_Pan;
		status = true;
	}
	if (!in_Rules_Tilt.empty())
	{
		Rules_Tilt = in_Rules_Tilt;
	}
	return status;
}

bool CommandBlock::CalcRefState(_asset asset, _cam_state cam_state, _target target_state)
{
	_geo_point cam_position, target_position;
	last_asset_state = asset;
	last_cam_state = cam_state;
	last_target_state = target_state;

	if (!asset.in_geo || !target_state.pos.in_geo)
	{
		return false;
	}

	cam_position.alt = cam_state.height + asset.alt_z;
	cam_position.lat = asset.lat_y; // no offset
	cam_position.lon = asset.lon_x; // no offset

	target_position.alt = target_state.pos.alt_z;
	target_position.lat = target_state.pos.lat_y;
	target_position.lon = target_state.pos.lon_x;

	ref_cam_state.pan = CalcAzimuth(cam_position, target_position);
	ref_cam_state.tilt = CalcElevation(cam_position, target_position);

	return true;
}
bool CommandBlock::CalcRefState(_asset asset, _cam_state cam_state, _region_interest region)
{
	_geo_point cam_position, region_position;
	last_asset_state = asset;
	last_cam_state = cam_state;
	last_region_interest = region;
	
	if (!asset.in_geo)
	{
		return false;
	}

	cam_position.alt = cam_state.height + asset.alt_z;
	cam_position.lat = asset.lat_y; // no offset
	cam_position.lon = asset.lon_x; // no offset

	region_position.alt = region.pointA.alt;
	region_position.lat = region.pointA.lat;
	region_position.lon = region.pointA.lon;

	ref_cam_state.pan = CalcAzimuth(cam_position, region_position);
	ref_cam_state.tilt = CalcElevation(cam_position, region_position);

	return true;
}

bool CommandBlock::ChangeZoom(int zoom_steps)
{
	AddCommands(0, 0, zoom_steps, 0, 0);
	return true;
}
bool CommandBlock::GenerateScanPath(_cam_state cam_state)
{
	return true;
}

/* Step based position controller
	1. Pan and tilt motions are performed in steps determined by user-defined/adaptive steps
	2. Step size is ensured using a calculated time window without any feedback
	3. Due to the delays involved, the step sizes are not accurate and hence does not allow precise positioning of the camera */
double CommandBlock::Fuzzy_Control(double in_error, std::vector<_rule_fuzzy> in_Rules)
{
	double out_rate{ 0 };
	bool found{ false };
	//Valid for only 2 rules at the moment, need modification for generalized approach
	for (const auto& it_rule : in_Rules)
	{
		switch (it_rule.comp_operator)
		{
		case GREATER_THAN_OR_EQUAL:
			if (abs(in_error) >= it_rule.ref_err)
			{
				out_rate = (in_error > 0) ? it_rule.out_rate : -it_rule.out_rate;
				if (abs(out_rate) > max_speed_rate)
				{
					out_rate = (out_rate > 0) ? max_speed_rate : -max_speed_rate;
				}
				found = true;
			}
			break;
		case LESS_THAN:
			if (abs(in_error) < it_rule.ref_err)
			{
				out_rate = (in_error > 0) ? it_rule.out_rate : -it_rule.out_rate;
				if (abs(out_rate) > max_speed_rate)
				{
					out_rate = (out_rate > 0) ? max_speed_rate : -max_speed_rate;
				}
				found = true;
			}
			break;
		default:
			break;
		}
		if (found)
		{
			break;
		}
	}
	return out_rate;
}
std::vector<double> CommandBlock::Update_SpeedRef(double in_speedrate_pan, double in_speedrate_tilt)
{
	std::vector<double> out_speed_ref;
	last_speedrate_pan = in_speedrate_pan;
	last_speedref_pan = last_speedref_pan + in_speedrate_pan * sampling_time;
	if (abs(last_speedref_pan) > upper_speed)
	{
		last_speedref_pan = (last_speedref_pan > 0) ? upper_speed : -upper_speed;
	}
	else if (abs(last_speedref_pan) < lower_speed)
	{
		last_speedref_pan = (last_speedref_pan > 0) ? lower_speed : -lower_speed;
	}

	last_speedrate_tilt = in_speedrate_tilt;
	last_speedref_tilt = last_speedref_tilt + in_speedrate_tilt * sampling_time;
	if (abs(last_speedref_tilt) > upper_speed)
	{
		last_speedref_tilt = (last_speedref_tilt > 0) ? upper_speed : -upper_speed;
	}
	else if (abs(last_speedref_tilt) < lower_speed)
	{
		last_speedref_tilt = (last_speedref_tilt > 0) ? lower_speed : -lower_speed;
	}

	out_speed_ref.push_back(last_speedref_pan);
	out_speed_ref.push_back(last_speedref_tilt);
	return out_speed_ref;
}

bool CommandBlock::AddCommands(int tilt_steps, int pan_steps, int zoom_steps, double in_speed_pan, double in_speed_tilt)
{
	// Assumption of equal step_sizes for pan and tilt
	int quadrant = 1 * (tilt_steps > 0 && pan_steps > 0) + 2 * (tilt_steps > 0 && pan_steps < 0) + 3 * (tilt_steps < 0 && pan_steps < 0) + 4 * (tilt_steps < 0 && pan_steps > 0);
	bool more_pan_steps = abs(pan_steps) > abs(tilt_steps);
	int diagonal_steps = (int)more_pan_steps * abs(pan_steps) + (int)(!more_pan_steps) * abs(tilt_steps);
	int straight_steps = abs(abs(pan_steps) - abs(tilt_steps));

	if (control_mode == "SPEED_STEPS")
	{
		switch (quadrant)
		{
		case 1:
			for (int it = 0; it < diagonal_steps; it++)
			{
				CommandStack.push_back({ UP_RIGHT, 0, 0 });
			}
			for (int ii = 0; ii < straight_steps; ii++)
			{
				if (more_pan_steps)
				{
					CommandStack.push_back({ RIGHT, 0, 0 });
				}
				else
				{
					CommandStack.push_back({ UP, 0, 0 });
				}
			}
			break;
		case 2:
			for (int it = 0; it < diagonal_steps; it++)
			{
				CommandStack.push_back({ UP_LEFT, 0, 0 });
			}
			for (int ii = 0; ii < straight_steps; ii++)
			{
				if (more_pan_steps)
				{
					CommandStack.push_back({ LEFT, 0, 0 });
				}
				else
				{
					CommandStack.push_back({ UP, 0, 0 });
				}
			}
			break;
		case 3:
			for (int it = 0; it < diagonal_steps; it++)
			{
				CommandStack.push_back({ DOWN_LEFT, 0, 0 });
			}
			for (int ii = 0; ii < straight_steps; ii++)
			{
				if (more_pan_steps)
				{
					CommandStack.push_back({ LEFT, 0, 0 });
				}
				else
				{
					CommandStack.push_back({ DOWN, 0, 0 });
				}
			}
			break;
		case 4:
			for (int it = 0; it < diagonal_steps; it++)
			{
				CommandStack.push_back({ DOWN_RIGHT, 0, 0 });
			}
			for (int ii = 0; ii < straight_steps; ii++)
			{
				if (more_pan_steps)
				{
					CommandStack.push_back({ RIGHT, 0, 0 });
				}
				else
				{
					CommandStack.push_back({ DOWN, 0, 0 });
				}
			}
			break;
		default:
			break;
		}
	}
	else if (control_mode == "FUZZY")
	{
		CommandStack.push_back({ SET_SPEED_PAN_TILT, in_speed_pan, in_speed_tilt });
	}
		
	for (int it = 0; it < abs(zoom_steps); it++)
	{
		if (zoom_steps > 0)
		{
			CommandStack.push_back({ ZOOM_IN, 0, 0 });
		}
		else
		{
			CommandStack.push_back({ ZOOM_OUT, 0, 0 });
		}
	}

	return true;
}
bool CommandBlock::GeneratePath()
{
	std::vector<double> speed_ref;
	double delta_tilt = ref_cam_state.tilt - last_cam_state.tilt;
	double tilt_steps = delta_tilt / step_size_tilt;

	double delta_pan = ref_cam_state.pan - last_cam_state.pan;
	if (continuous_pan && (delta_pan > 180 || delta_pan < -180))
	{
		delta_pan = (delta_pan > 0) ? delta_pan - 360 : delta_pan + 360;
	}
	double pan_steps = delta_pan / step_size_pan;

	double speedrate_pan = Fuzzy_Control(delta_pan, Rules_Pan);
	double speedrate_tilt = Fuzzy_Control(delta_tilt, Rules_Tilt);
	speed_ref = Update_SpeedRef(speedrate_pan, speedrate_tilt);
	

	if (speed_ref.empty())
	{
		return false;
	}
	if (delta_pan < step_size_pan)
	{
		speed_ref[0] = 0;
	}
	if (delta_tilt < step_size_tilt)
	{
		speed_ref[1] = 0;
	}

	return true && AddCommands(tilt_steps, pan_steps, 0, speed_ref[0], speed_ref[1]);
}
bool CommandBlock::Get_CommandStack(std::vector<_cam_command> out_CommandStack)
{
	out_CommandStack = CommandStack;
	return true;
}
	

