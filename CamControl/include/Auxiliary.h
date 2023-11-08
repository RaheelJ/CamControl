#pragma once
#include <string>
#include <vector>

struct _asset
{
	double time;
	std::string ID;
	bool in_geo;
	double lon_x, lat_y, alt_z;
	double speed_x, speed_y, speed_z;
	double yaw, pitch, roll;			//Asset Orientation
};
struct _cam_state
{
	double time;
	bool is_absolute = true;
	double offset_x = 0, offset_y = 0;
	double pan, tilt, zoom;
	double height;
	double FOV_horizontal;
	double ref_azimuth, ref_elevation;
};

struct _zoom_control
{
	int max_x;
	double zoom_step;
	double max_FOV_horizontal;
	double min_FOV_horizontal;
};
struct _PTZ_control
{
	bool continuous;
	double step_size;
	double span;
};
struct _speed_control
{
	bool adaptive{ false };
	double max_value;
	double fixed_value;
};

struct _network
{
	std::string IP;
	int port;
	std::string username;
	std::string password;
};
struct _serial_comm
{
	std::string comm_port;
	int baud_rate;
	int timeout;
};

struct _camera
{
	std::string profile = "Profile1";
	std::string asset_ID;
	bool GPS_available{ true };
	bool night_vision{ true };
	bool optical_zoom{ true };
	int cam_address;
	bool extended_instructions{ true };
	std::string type{ "optical" };
	std::string ONVIF_profile{ "PROFILE_T" };
	std::string ID;
	std::string protocol;
	_network network;
	_serial_comm serial;
	double resolution;
	int aspect_ratio[2];
	_speed_control speed;
	_PTZ_control pan;
	_PTZ_control tilt;
	_zoom_control zoom;
	_cam_state state;
	_cam_state initial_state;
};
enum _cam_command_type
{
	LEFT, 
	RIGHT, 
	UP, 
	DOWN, 
	UP_RIGHT, 
	DOWN_RIGHT, 
	UP_LEFT, 
	DOWN_LEFT,
	ZOOM_IN,
	ZOOM_OUT,
	SET_SPEED_PAN,
	SET_SPEED_TILT,
	SET_SPEED_PAN_TILT
};
struct _cam_command
{
	_cam_command_type type;
	double value_1 = 0;
	double value_2 = 0;
};

struct _geo_point
{
	double lat, lon;					//degrees
	double alt;
};
struct _position
{
	bool in_geo;							//1=Geographical Coordinates, 0=Local/Cartesian Coordiantes
	double lat_y, lon_x, alt_z;				//unit: degrees and meters
	double std_lat_y, std_lon_x, std_alt_z;	//standard deviations
};
struct _velocity
{
	double x_lon, y_lat, z_alt;					//unit: m/s
	double std_x_lon, std_y_lat, std_z_alt;		//standard deviations
};

enum _target_category
{
	CAR,
	SHIP,
	AIRCRAFT,
	PEDISTRIAN,
	MISSILE
};
struct _target_threat
{
	double level;				//Saaty's scale
	double detect_confidence;	//detection confidence
};
struct _target_type
{
	std::string category;		//target category
	double size;				//target size 
};
struct _target
{
	int ID;						//target or track ID
	double time;				//time instant
	_target_type type;
	_position pos;
	_velocity vel;
	_target_threat threat;
};

struct _queue_element
{
	double priority, Npriority;		//Normalized and not normalized priority				
	int ID;							
	int count;						//number of times the element has been covered 
	int last_stoppage_criteria;		//criteria that stopped the coverage
	double last_coverage_time;		//time at which the element was covered most recently
};
struct _priority_weights
{
	double W_threat;
	double W_std_deg;
	double W_std;
	double W_type;
	double W_detect;
};
struct _region_interest
{
	int ID;							//-ve for regions
	double priority{ 0.5 };
	_geo_point pointA, pointB, pointC, pointD;
};

enum _operator
{
	LESS_THAN,
	GREATER_THAN,
	LESS_THAN_OR_EQUAL,
	GREATER_THAN_OR_EQUAL,
	EQUAL
};
struct _rule_fuzzy
{
	std::string name;
	std::string err_variable;
	_operator comp_operator;
	double ref_err;
	double out_rate;
};

double ReadParam(double, double, double, int&);
double RandNum(double min, double max);
std::string ReadSpacedParam(std::string, int, char);
double rad_to_deg(double in);
double deg_to_rad(double in);
double CalcAzimuth(_geo_point, _geo_point);
double CalcElevation(_geo_point, _geo_point);
double CalcDistance(_geo_point, _geo_point);
