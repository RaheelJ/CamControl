#define _USE_MATH_DEFINES               /* Adds math constants */ 

#include <Auxiliary.h>
#include <random>
#include <GeographicLib/Geodesic.hpp>

double ReadParam(double param, double lower_lim, double upper_lim, int& error)
{
    double out = 0;
    if (param <= upper_lim && param >= lower_lim) {
        out = param;
    }
    else {
        error = error + 1;
    }
    return out;
}
std::string ReadSpacedParam(std::string param, int index, char separator)
{
    std::string temp;
    std::vector<std::string> Elements;
    for (int it = 0; it < param.size(); it++)
    {
        if (param[it] == separator)
        {
            if (temp.size() > 0)
            {
                Elements.push_back(temp);
                temp.clear();
            }
            continue;
        }
        else if (param[it] == ' ')
        {
            continue;
        }
        temp.push_back(param[it]);
    }
    if (temp.size() > 0)
    {
        Elements.push_back(temp);
    }

    return Elements[index];
}

double RandNum(double min, double max)
{
    double rand_double = 0;
    double rand_num = std::rand();
    double rand_max = RAND_MAX;
    rand_double = min + (rand_num / rand_max) * (max - min);
    return rand_double;
}

double rad_to_deg(double in) {
    return 180 * fmod((in), 2 * M_PI) / M_PI;
}
double deg_to_rad(double in) {
    return M_PI * fmod((in), 360) / 180;
}


double CalcAzimuth(_geo_point asset, _geo_point target)
{
    GeographicLib::Geodesic geod(GeographicLib::Constants::WGS84_a(), GeographicLib::Constants::WGS84_f());
    double azimuth, dist, temp_angle;
    geod.Inverse(asset.lat, asset.lon, target.lat, target.lon, dist, azimuth, temp_angle);
    //azimuth = deg_to_rad(azimuth);
    return azimuth;
}
double CalcDistance(_geo_point asset, _geo_point target)
{
    GeographicLib::Geodesic geod(GeographicLib::Constants::WGS84_a(), GeographicLib::Constants::WGS84_f());
    double azimuth, dist, temp_angle;
    geod.Inverse(asset.lat, asset.lon, target.lat, target.lon, dist, azimuth, temp_angle);
    return dist;
}
double CalcElevation(_geo_point asset, _geo_point target)
{
    double elevation, delta_altitude;
    double azimuth, dist, temp_angle;
    GeographicLib::Geodesic geod(GeographicLib::Constants::WGS84_a(), GeographicLib::Constants::WGS84_f());
    geod.Inverse(asset.lat, asset.lon, target.lat, target.lon, dist, azimuth, temp_angle);
    if (dist < 0.0001)
    {
        return 0.0;
    }

    delta_altitude = target.alt - asset.alt;
    elevation = atan(delta_altitude / dist);
    return elevation;
}
