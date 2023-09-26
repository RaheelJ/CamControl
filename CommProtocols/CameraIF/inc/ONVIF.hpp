#pragma once

#include <CameraIF/inc/CamGenClass.hpp>
#include <libONVIF/include/OnvifPtzClient.h>
#include <libONVIF/include/OnvifDiscoveryClient.h>
#include <libONVIF/include/OnvifDevice.h>
#include <libONVIF/include/OnvifDeviceClient.h>
#include <QString>

namespace CamIF
{
    class CamIF_ONVIF : public CamGenericClass
    {
    public:
        //constructor

        CamIF_ONVIF(QString ip, QString port, QString user, QString pass, QString profile);

        bool CameraOn(void);
        /*turn camera off */
        bool CameraOff(void);
        /*zoom wide */
        bool ZoomWide(void);
        /*zoom tele */
        bool ZoomTele(void);
        /*zoom stop */
        bool ZoomStop(void);
        /*tiltagle 0-180  speed 0 -->1 */
        bool CamDown(float tiltangle, float tiltspeed);
        /*tiltagle 0-180  speed 0 -->1 */
        bool CamUp(float tiltangle, float tiltspeed);
        /*panangle 0-180  speed 0 -->1 */
        bool CamLeft(float panangle, float panspeed);
        /*panangle 0-180  speed 0 -->1 */
        bool CamRigth(float panangle, float panspeed);
        /*panangle 0-180 tiltagle 0-180 speed 0 -->1 */
        bool CamUpRight(float panangle, float panspeed, float tiltangle, float tiltspeed);
        /*panangle 0-180 tiltagle 0-180 speed 0 -->1 */
        bool CamUpLeft(float panangle, float panspeed, float tiltangle, float tiltspeed);
        /*panangle 0-180 tiltagle 0-180 speed 0 -->1 */
        bool CamDownRight(float panangle, float panspeed, float tiltangle, float tiltspeed);
        /*panangle 0-180 tiltagle 0-180 speed 0 -->1 */
        bool CamDownLeft(float panangle, float panspeed, float tiltangle, float tiltspeed);
        /*go down left */
        bool CamStop(void);

        bool CamQueryPan();
        bool CamQueryTilt();
        bool CamQueryZoom();

    private:
        bool motionwrapper(float pan, float panSpeed, float tilt, float tiltSpeed, float zoom, float zoomSpeed);
        bool motionwrapperabs(float pan, float panSpeed, float tilt, float tiltSpeed, float zoom, float zoomSpeed);
        QSharedPointer<SoapCtx> ctx;
        OnvifPtzClient* Ptz;
        QString Profile;
    };
}