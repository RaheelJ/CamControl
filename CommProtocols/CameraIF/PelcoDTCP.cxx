#include <CameraIF/inc/PelcoDTCP.hpp>

using boost::asio::ip::tcp;

using namespace CamIF;
using namespace pelco;

CamIF_PelcoDTCP::CamIF_PelcoDTCP(const char* ip, const char* port,UINT8 pelcoaddr)  :CamGenericClass(), ConnectSocket(io_context)
{
	tcp::resolver resolver(io_context);
	try {
		boost::asio::connect(ConnectSocket, resolver.resolve(ip, port));
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << "\n";
	}
	
	PelcodAddress = pelcoaddr;
}
CamIF_PelcoDTCP::~CamIF_PelcoDTCP() 
{
	
}

bool CamIF_PelcoDTCP::tcppelcodsend( pelco::PelcoDCommands cmd,UINT8 data1, UINT8 data2)
{
	
	//format command
	// 
	// Send an initial buffer
	int iResult,res;
	res = pelcoparser.GetCommand(sendbuf, PelcodAddress, cmd, data1, data2);
	/*res = 1;
	sendbuf[0] = 0xFF;
	sendbuf[1] = 1;
	sendbuf[2] = 0x0;
	sendbuf[3] = 0x02;
	sendbuf[4] = 0x20;
	sendbuf[5] = 0x00;
	sendbuf[6] = 0x23;*/
	if (!res)
	{
		return false;
	}
	try
	{
		boost::asio::write(ConnectSocket, boost::asio::buffer(sendbuf, 7));
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << "\n";
		return false;
	}
	return true;
}
bool CamIF_PelcoDTCP::CameraOn(void)
{
	return tcppelcodsend(PelcoDCommands::CAMERA_ON, 0, 0);
}

/*turn camera off */
bool CamIF_PelcoDTCP::CameraOff(void)
{
	return tcppelcodsend(PelcoDCommands::CAMERA_OFF, 0, 0);
}
/*zoom wide */
bool CamIF_PelcoDTCP::ZoomWide(void)
{
	return tcppelcodsend(PelcoDCommands::ZOOM_WIDE, 0, 0);
}
/*zoom tele */
bool CamIF_PelcoDTCP::ZoomTele(void)
{
	return tcppelcodsend(PelcoDCommands::ZOOM_TELE, 0, 0);
}
/*zoom stop */
bool CamIF_PelcoDTCP::ZoomStop(void)
{
	return tcppelcodsend(PelcoDCommands::ZOOM_STOP, 0, 0);
}
/*go down */
bool CamIF_PelcoDTCP::CamDown(float tiltangle, float tiltspeed)
{
	return tcppelcodsend(PelcoDCommands::DOWN, 0, (unsigned char)tiltspeed);
}
/*go up */
bool CamIF_PelcoDTCP::CamUp(float tiltangle, float tiltspeed)
{
	return tcppelcodsend(PelcoDCommands::UP, 0, (unsigned char)tiltspeed);
}
/*go left */
bool CamIF_PelcoDTCP::CamLeft(float panangle, float panspeed)
{
	return tcppelcodsend(PelcoDCommands::LEFT, (unsigned char)panspeed, 0);
}
/*go right */
bool CamIF_PelcoDTCP::CamRigth(float panangle, float panspeed)
{
	return tcppelcodsend(PelcoDCommands::RIGHT, (unsigned char)panspeed, 0);
}
/*go up right */
bool CamIF_PelcoDTCP::CamUpRight(float panangle, float panspeed, float tiltangle, float tiltspeed)
{
	return tcppelcodsend(PelcoDCommands::UP_RIGHT, (unsigned char)panspeed, (unsigned char)tiltspeed);
}
/*go up left */
bool CamIF_PelcoDTCP::CamUpLeft(float panangle, float panspeed, float tiltangle, float tiltspeed)
{
	return tcppelcodsend(PelcoDCommands::UP_LEFT, (unsigned char)panspeed, (unsigned char)tiltspeed);
}


/*go down right */
bool CamIF_PelcoDTCP::CamDownRight(float panangle, float panspeed, float tiltangle, float tiltspeed)
{
	return tcppelcodsend(PelcoDCommands::DOWN_RIGHT, (unsigned char)panspeed, (unsigned char)tiltspeed);
}
/*go down left */
bool CamIF_PelcoDTCP::CamDownLeft(float panangle, float panspeed, float tiltangle, float tiltspeed)
{
	return tcppelcodsend(PelcoDCommands::DOWN_LEFT, (unsigned char)panspeed, (unsigned char)tiltspeed);
}
/*go down left */
bool CamIF_PelcoDTCP::CamStop(void)
{
	return tcppelcodsend(PelcoDCommands::STOP, 0, 0);
}

bool CamIF_PelcoDTCP::CamQueryPan()
{
	return tcppelcodsend(PelcoDCommands::QUERY_PAN, 0, 0);
}
bool CamIF_PelcoDTCP::CamQueryTilt()
{
	return tcppelcodsend(PelcoDCommands::QUERY_TILT, 0, 0);
}
bool CamIF_PelcoDTCP::CamQueryZoom()
{
	return tcppelcodsend(PelcoDCommands::QUERY_ZOOM, 0, 0);
}