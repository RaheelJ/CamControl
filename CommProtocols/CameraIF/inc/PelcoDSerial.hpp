#pragma once
#include "CamGenClass.hpp"
#include <PelcoDProtocolParser.h>
namespace CamIF
{
    class CamIF_PelcoDSerial : public CamGenericClass
    {
    public:
        //constructor

        CamIF_PelcoDSerial();

        bool CameraOn(void);
        /*turn camera off */
        bool CameraOff(void);
        /*zoom wide */
        bool ZoomWide(void);
        /*zoom tele */
        bool ZoomTele(void);
        /*zoom stop */
        bool ZoomStop(void);
        /*go down speed 0-3f angle is not used*/
        bool CamDown(float tiltangle, float tiltspeed);
        /*go down speed 0-3f angle is not used*/
        bool CamUp(float tiltangle, float tiltspeed);
        /*go left */
        bool CamLeft(float panangle, float panspeed);
        /*go right */
        bool CamRigth(float panangle, float panspeed);
        /*go up right */
        bool CamUpRight(float panangle, float panspeed, float tiltangle, float tiltspeed);
        /*go up left */
        bool CamUpLeft(float panangle, float panspeed, float tiltangle, float tiltspeed);

        /*go down right */
        bool CamDownRight(float panangle, float panspeed, float tiltangle, float tiltspeed);
        /*go down left */
        bool CamDownLeft(float panangle, float panspeed, float tiltangle, float tiltspeed);
        /*go down left */
        bool CamStop(void);
    };
}