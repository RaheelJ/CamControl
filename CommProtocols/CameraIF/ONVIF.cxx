#include <CameraIF/inc/ONVIF.hpp>
#include <QString>

using namespace CamIF;

CamIF_ONVIF::CamIF_ONVIF(QString ip, QString port, QString user, QString pass, QString profile) :CamGenericClass()
{
	ctx = QSharedPointer<SoapCtx>::create();
	
	Request<_tds__GetServices> request;
	request.IncludeCapability = false;
	OnvifDeviceClient onvifDevice(QUrl("http://" + ip + ":" + port + "/onvif/device_service"), ctx);
	auto servicesResponse = onvifDevice.GetServices(request);
	
	if (servicesResponse) {
		for (auto service : servicesResponse.GetResultObject()->Service) {
			qDebug() << "namespace:" << service->Namespace.toStdString().c_str() << "Url:" << service->XAddr.toStdString().c_str();
			if (service->Namespace == "http://www.onvif.org/ver20/ptz/wsdl") {
				Ptz = new OnvifPtzClient(QUrl(service->XAddr.toStdString().c_str()), ctx, nullptr);
				ctx->SetAuth(user, pass);
				//Ptz->SetAuth(user, pass, AUTO);
				Profile = profile;
				/*Request<_tptz__GotoHomePosition> request;
				request.ProfileToken = profile;
				auto servicesResponse = Ptz->GotoHomePosition(request);
				if (servicesResponse.IsAuthFault()) {
					qDebug() << "authentication failed";
				}*/
			}
		}
	}
	
	
}

bool CamIF_ONVIF::motionwrapper(float pan, float panSpeed, float tilt, float tiltSpeed, float zoom, float zoomSpeed)
{
	if (Ptz == nullptr)
	{
		qDebug() << "failed to make motion";
		return false;
	}
	Request<_tptz__RelativeMove> rRequest;
	Response<_tptz__RelativeMoveResponse> response;
	rRequest.ProfileToken = Profile;
	rRequest.Translation = new tt__PTZVector();
	rRequest.Translation->PanTilt = new tt__Vector2D();
	rRequest.Translation->Zoom = new tt__Vector1D();
	rRequest.Speed = new tt__PTZSpeed();
	rRequest.Speed->PanTilt = new tt__Vector2D();
	rRequest.Speed->Zoom = new tt__Vector1D();

	//pan
	rRequest.Translation->PanTilt->x = pan;
	rRequest.Speed->PanTilt->x = panSpeed;
	//tilt
	rRequest.Translation->PanTilt->y = tilt;
	rRequest.Speed->PanTilt->y = tiltSpeed;
	//zoom
	rRequest.Translation->Zoom->x = zoom;
	rRequest.Speed->Zoom->x = zoomSpeed;
	response = Ptz->RelativeMove(rRequest);

	//delete rRequest.Translation->PanTilt;
	//delete rRequest.Translation->Zoom;
	//delete rRequest.Translation;
	//delete rRequest.Speed->PanTilt;
	//delete rRequest.Speed->Zoom;
	//delete rRequest.Speed;

	if (response.IsFault())
	{
		qDebug() << "failed to make motion 2";
		return false;
	}
	return true;

}

bool CamIF_ONVIF::motionwrapperabs(float pan, float panSpeed, float tilt, float tiltSpeed, float zoom, float zoomSpeed)
{
	if (Ptz == nullptr)
	{
		qDebug() << "failed to make motion";
		return false;
	}
	Request<_tptz__AbsoluteMove> rRequest;
	Response<_tptz__AbsoluteMoveResponse> response;
	rRequest.ProfileToken = Profile;
	rRequest.Position = new tt__PTZVector();
	rRequest.Position->PanTilt = new tt__Vector2D();
	rRequest.Position->Zoom = new tt__Vector1D();
	rRequest.Speed = new tt__PTZSpeed();
	rRequest.Speed->PanTilt = new tt__Vector2D();
	rRequest.Speed->Zoom = new tt__Vector1D();

	//pan
	rRequest.Position->PanTilt->x = pan;
	rRequest.Speed->PanTilt->x = panSpeed;
	//tilt
	rRequest.Position->PanTilt->y = tilt;
	rRequest.Speed->PanTilt->y = tiltSpeed;
	//zoom
	rRequest.Position->Zoom->x = zoom;
	rRequest.Speed->Zoom->x = zoomSpeed;
	response = Ptz->AbsoluteMove(rRequest);

	delete rRequest.Position->PanTilt;
	delete rRequest.Position->Zoom;
	delete rRequest.Position;
	delete rRequest.Speed->PanTilt;
	delete rRequest.Speed->Zoom;
	delete rRequest.Speed;

	if (response.IsFault())
	{
		qDebug() << "failed to make motion 2";
		return false;
	}
	return true;

}
bool CamIF_ONVIF::CameraOn(void)
{
	qDebug() << "this command is not suppported in ONVIF";
	return false;
}

/*turn camera off */
bool CamIF_ONVIF::CameraOff(void)
{
	qDebug() << "this command is not suppported in ONVIF";
	return false;
}
/*zoom wide */
bool CamIF_ONVIF::ZoomWide(void)
{
	return motionwrapperabs(0, 0, 0, 0, 0, 1);
}
/*zoom tele */
bool CamIF_ONVIF::ZoomTele(void)
{
	return motionwrapperabs(0, 0, 0, 0, 1, 1);
}
/*zoom stop */
bool CamIF_ONVIF::ZoomStop(void)
{
	return true;
}
/*tiltagle 0-180  speed 0 -->1 */
bool CamIF_ONVIF::CamDown(float tiltangle, float tiltspeed)
{
	return motionwrapper(0, 0, -tiltangle, tiltspeed, 0, 0);
}
/*tiltagle 0-180  speed 0 -->1 */
bool CamIF_ONVIF::CamUp(float tiltangle, float tiltspeed)
{
	return motionwrapper(0,0, tiltangle, tiltspeed,0,0);
}
/*panangle 0-180  speed 0 -->1 */
bool CamIF_ONVIF::CamLeft(float panangle, float panspeed)
{
	return motionwrapper(-panangle, panspeed, 0, 0, 0, 0);
}
/*panangle 0-180  speed 0 -->1 */
bool CamIF_ONVIF::CamRigth(float panangle, float panspeed)
{
	return motionwrapper(panangle, panspeed, 0, 0, 0, 0);
}
/*go up right */
bool CamIF_ONVIF::CamUpRight(float panangle, float panspeed, float tiltangle, float tiltspeed)
{
	return motionwrapper(panangle, panspeed, tiltangle, tiltspeed, 0, 0);
}
/*go up left */
bool CamIF_ONVIF::CamUpLeft(float panangle, float panspeed, float tiltangle, float tiltspeed)
{
	return motionwrapper(-panangle, panspeed, tiltangle, tiltspeed, 0, 0);
}

/*go down right */
bool CamIF_ONVIF::CamDownRight(float panangle, float panspeed, float tiltangle, float tiltspeed)
{
	return motionwrapper(panangle, panspeed, -tiltangle, tiltspeed, 0, 0);
}
/*go down left */
bool CamIF_ONVIF::CamDownLeft(float panangle, float panspeed, float tiltangle, float tiltspeed)
{
	return motionwrapper(-panangle, panspeed, -tiltangle, tiltspeed, 0, 0);
}
/*go down left */
bool CamIF_ONVIF::CamStop(void)
{
	Request<_tptz__Stop> rRequest;
	Response<_tptz__StopResponse> response;
	rRequest.ProfileToken = Profile;
	rRequest.PanTilt = nullptr;
	rRequest.Zoom= nullptr;
	response = Ptz->Stop(rRequest);
	if (response.IsFault())
	{
		qDebug() << "failed to stop ";
		return false;
	}
	return true;
}

bool CamIF_ONVIF::CamQueryPan()
{
	return true;
}
bool CamIF_ONVIF::CamQueryTilt()
{
	return true;
}
bool CamIF_ONVIF::CamQueryZoom()
{
	return true;
}