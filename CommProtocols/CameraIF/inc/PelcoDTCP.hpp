#pragma once
#include "CamGenClass.hpp"
#include <stdlib.h>
#include <stdio.h> 
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <boost/asio.hpp>
#include <Pelco_D_Parser/PelcoDProtocolParser.h>

using boost::asio::ip::tcp;
namespace CamIF
{
	class CamIF_PelcoDTCP : public CamGenericClass
	{
    public:
        //constructor

        CamIF_PelcoDTCP(const char* ip, const char* port, UINT8 pelcoaddr);
        ~CamIF_PelcoDTCP();
        bool CameraOn(void);
        /*turn camera off */
        bool CameraOff(void);
        /*zoom wide */
        bool ZoomWide(void);
        /*zoom tele */
        bool ZoomTele(void) ;
        /*zoom stop */
        bool ZoomStop(void);
        /*go down speed 0-3f angle is not used*/
        bool CamDown(float tiltangle, float tiltspeed);
        /*go up speed 0-3f  angle is not used*/
        bool CamUp(float tiltangle, float tiltspeed);
        /*go left speed 0-3f angle not used*/
        bool CamLeft(float panangle, float panspeed);
        /*go right speed 0-3f angle not used*/
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

        bool CamQueryPan();
        bool CamQueryTilt();
        bool CamQueryZoom();

    private:

        //genreric function
        bool tcppelcodsend(pelco::PelcoDCommands cmd,UINT8 data1, UINT8 data2);
        boost::asio::io_context io_context;
        tcp::socket ConnectSocket ;
        UINT8 sendbuf[7];
        UINT8 PelcodAddress;
        pelco::PelcoDProtocolParser pelcoparser;

	};
}