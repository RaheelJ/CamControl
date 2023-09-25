#pragma once
#include <cstdint>


namespace CamIF
{
    class CamGenericClass
    {
    public:

        /**
         * @brief Class constructor.
         * 1. CAMERA_ON
            2. CAMERA_OFF
            3. ZOOM_WIDE
            4. ZOOM_TELE
            5. ZOOM_STOP
            6. DOWN
            7. UP
            8. LEFT
            9. RIGHT
            10. UP_RIGHT
            11. UP_LEFT
            12. DOWN_RIGHT
            13. DOWN_LEFT
            14. STOP

         */
        CamGenericClass()
        {

        }

        /**
         * @brief Class destructor.
         */
        ~CamGenericClass()
        {

        }
        /*turn camera on */
        virtual bool CameraOn(void)=0;
        /*turn camera off */
        virtual bool CameraOff(void)=0;
        /*zoom wide */
        virtual bool ZoomWide(void) = 0;
        /*zoom tele */
        virtual bool ZoomTele(void) = 0;
        /*zoom stop */
        virtual bool ZoomStop(void) = 0;
        /*go down */
        virtual bool CamDown(float tiltangle, float tiltspeed) = 0;
        /*go up */
        virtual bool CamUp(float tiltangle, float tiltspeed) = 0;
        /*go left */
        virtual bool CamLeft(float panangle, float panspeed) = 0;
        /*go right */
        virtual bool CamRigth(float panangle, float panspeed) = 0;
        /*go up right */
        virtual bool CamUpRight(float panangle, float panspeed ,float tiltangle, float tiltspeed) = 0;
        /*go up left */
        virtual bool CamUpLeft(float panangle, float panspeed, float tiltangle, float tiltspeed) = 0;

        /*go down right */
        virtual bool CamDownRight(float panangle, float panspeed, float tiltangle, float tiltspeed) = 0;
        /*go down left */
        virtual bool CamDownLeft(float panangle, float panspeed, float tiltangle, float tiltspeed) = 0;
        /*go down left */
        virtual bool CamStop(void) = 0;
        virtual bool CamQueryPan() = 0;
        virtual bool CamQueryTilt() = 0;
        virtual bool CamQueryZoom() = 0;



    private:

    };
}