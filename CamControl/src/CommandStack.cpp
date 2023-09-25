#include <CommandStack.h>

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

bool CommandBlock::AddCommands(int tilt_steps, int pan_steps, int zoom_steps)
{
	// Assumption of equal step_sizes for pan and tilt
	int quadrant = 1 * (tilt_steps > 0 && pan_steps > 0) + 2 * (tilt_steps > 0 && pan_steps < 0) + 3 * (tilt_steps < 0 && pan_steps < 0) + 4 * (tilt_steps < 0 && pan_steps > 0);
	bool more_pan_steps = abs(pan_steps) > abs(tilt_steps);
	int diagonal_steps = (int)more_pan_steps * abs(pan_steps) + (int)(!more_pan_steps) * abs(tilt_steps);
	int straight_steps = abs(abs(pan_steps) - abs(tilt_steps));

	switch (quadrant)
	{
		case 1:
			for (int it = 0; it < diagonal_steps; it++)
			{
				CommandStack.push_back(UP_RIGHT);
			}
			for (int ii = 0; ii < straight_steps; ii++)
			{
				if (more_pan_steps)
				{
					CommandStack.push_back(RIGHT);
				}
				else
				{
					CommandStack.push_back(UP);
				}
			}
			break;
		case 2:
			for (int it = 0; it < diagonal_steps; it++)
			{
				CommandStack.push_back(UP_LEFT);
			}
			for (int ii = 0; ii < straight_steps; ii++)
			{
				if (more_pan_steps)
				{
					CommandStack.push_back(LEFT);
				}
				else
				{
					CommandStack.push_back(UP);
				}
			}
			break;
		case 3:
			for (int it = 0; it < diagonal_steps; it++)
			{
				CommandStack.push_back(DOWN_LEFT);
			}
			for (int ii = 0; ii < straight_steps; ii++)
			{
				if (more_pan_steps)
				{
					CommandStack.push_back(LEFT);
				}
				else
				{
					CommandStack.push_back(DOWN);
				}
			}
			break;
		case 4:
			for (int it = 0; it < diagonal_steps; it++)
			{
				CommandStack.push_back(DOWN_RIGHT);
			}
			for (int ii = 0; ii < straight_steps; ii++)
			{
				if (more_pan_steps)
				{
					CommandStack.push_back(RIGHT);
				}
				else
				{
					CommandStack.push_back(DOWN);
				}
			}
			break;
		default:
			break;
	}

	for (int it = 0; it < abs(zoom_steps); it++)
	{
		if (zoom_steps > 0)
		{
			CommandStack.push_back(ZOOM_IN);
		}
		else
		{
			CommandStack.push_back(ZOOM_OUT);
		}
	}

	return true;
}
bool CommandBlock::GeneratePath()
{
	double delta_tilt = ref_cam_state.tilt - last_cam_state.tilt;
	double tilt_steps = delta_tilt / step_size_tilt;

	double delta_pan = ref_cam_state.pan - last_cam_state.pan;
	if (continuous_pan && (delta_pan > 180 || delta_pan < -180))
	{
		delta_pan = (delta_pan > 0) ? delta_pan - 360 : delta_pan + 360;
	}
	double pan_steps = delta_pan / step_size_pan;

	return true && AddCommands(tilt_steps, pan_steps, 0);
}
bool CommandBlock::ChangeZoom(int zoom_steps)
{
	AddCommands(0, 0, zoom_steps);
	return true;
}
bool CommandBlock::GenerateScanPath(_cam_state cam_state)
{
	return true;
}
