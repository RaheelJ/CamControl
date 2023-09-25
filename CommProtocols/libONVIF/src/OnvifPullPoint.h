/* Copyright(C) 2018 Björn Stresing
 *
 * This program is free software : you can redistribute it and / or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.If not, see < http://www.gnu.org/licenses/>.
 */
#pragma once
#include "Client.h"
#include "OnvifEventExport.h"
#include "Response.h"
#include <QList>
#include <QThread>


class OnvifEventClient;
struct OnvifPullPointPrivate;

class OnvifPullPointWorker : public QThread {

	Q_OBJECT

public:
	OnvifPullPointWorker(const QUrl &rEndpoint, QObject *pParent = nullptr);
	virtual ~OnvifPullPointWorker();
	bool StartListening();
	void StopListening();

signals:
	void MessageReceived(const Response<wsnt__NotificationMessageHolderType> &rResponse);
	void LostPullPoint(const SimpleResponse &rCause);
	void ResumedPullPoint();
	void UnsuccessfulPull(int unsuccessfulPullcount, const SimpleResponse &rCause);

protected:
	void run();

private:
	Q_DISABLE_COPY(OnvifPullPointWorker);

	Response<_tev__CreatePullPointSubscriptionResponse> Setup();

	QUrl mEndpoint;
	OnvifEventClient *mpClient;
};


/*!
 *
 * \brief A Onvif pull point listener
 *
 * Use this class to listen (non blocking) for events generated by a pull point
 *
 */
class ONVIFEVENT_EXPORT OnvifPullPoint : public Client {

	Q_OBJECT

public:
	/*!
	 *
	 * \brief Construct a pull point listener
	 *
	 * \param rEndpoint The WS event endpoint this listener will use for its WS calls
	 * \param pParent A QObject parent
	 *
	 */
	OnvifPullPoint(const QUrl &rEndpoint, QObject *pParent = nullptr);
	virtual ~OnvifPullPoint();
	//! Check if the pull point is active and we listen for events
	bool Active();

signals:
	//! Emitted if a event occurs holding the message of the event
	void MessageReceived(const Response<_tev__PullMessagesResponse> &rResponse);
	//! Emitted as soon as a pull failed
	void LostPullPoint(const SimpleResponse &rCause);
	//! Emitted as soon as a successful pull was received after a failed pull
	void ResumedPullPoint();
	//! Emitted if a message pull was unsuccessful. Contains the number of consecutive failed pulls
	void UnsuccessfulPull(int unsuccessfulPullcount, const SimpleResponse &rCause);
	//! Emitted if the active state changes
	void ActiveChanged(bool isActive);

public slots:
	//! Start listening for events. Non blocking
	void Start();
	//! Stop listening for events
	void Stop();

private:
	Q_DISABLE_COPY(OnvifPullPoint);

	OnvifPullPointPrivate *mpD;
};
