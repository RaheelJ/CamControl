
#include "PelcoDSerial.hpp"

using namespace CamIF;

CamIF_PelcoDSerial::CamIF_PelcoDSerial() :CamGenericClass()
{

}
bool CamIF_PelcoDSerial::CameraOn(void)
{
	return true;
}

/*turn camera off */
bool CamIF_PelcoDSerial::CameraOff(void)
{
	return true;
}
/*zoom wide */
bool CamIF_PelcoDSerial::ZoomWide(void)
{
	return true;
}
/*zoom tele */
bool CamIF_PelcoDSerial::ZoomTele(void)
{
	return true;
}
/*zoom stop */
bool CamIF_PelcoDSerial::ZoomStop(void)
{
	return true;
}
/*go down */
bool CamIF_PelcoDSerial::CamDown(float tiltangle, float tiltspeed)
{
	return true;
}
/*go up */
bool CamIF_PelcoDSerial::CamUp(float tiltangle, float tiltspeed)
{
	return true;
}
/*go left */
bool CamIF_PelcoDSerial::CamLeft(float panangle, float panspeed)
{
	return true;
}
/*go right */
bool CamIF_PelcoDSerial::CamRigth(float panangle, float panspeed)
{
	return true;
}
/*go up right */
bool CamIF_PelcoDSerial::CamUpRight(float panangle, float panspeed, float tiltangle, float tiltspeed)
{
	return true;
}
/*go up left */
bool CamIF_PelcoDSerial::CamUpLeft(float panangle, float panspeed, float tiltangle, float tiltspeed)
{
	return true;
}

/*go down right */
bool CamIF_PelcoDSerial::CamDownRight(float panangle, float panspeed, float tiltangle, float tiltspeed)
{
	return true;
}
/*go down left */
bool CamIF_PelcoDSerial::CamDownLeft(float panangle, float panspeed, float tiltangle, float tiltspeed)
{
	return true;
}
/*go down left */
bool CamIF_PelcoDSerial::CamStop(void)
{
	return true;
}