// -*- mode: c++; eval: (c-set-style "linux"); -*-
//////////////////////////////////////////////////////////////////
//
// RAS Listener Class.
//
// This Class will listen for incoming packets and let another
// new started thread analyze and answer the PDU
//
// This work is published under the GNU Public License (GPL)
// see file COPYING for details.
// We also explicitely grant the right to link this code
// with the OpenH323 library.
//
//
//////////////////////////////////////////////////////////////////

#ifndef RASWORKER_H
#define RASWORKER_H "@(#) $Id$"

#include <ptlib.h>
#include <ptlib/sockets.h>
#include <h225.h>
#include "gkauth.h"
#include "RasTbl.h"

class GK_RASListener;

class GK_RASWorker : public PThread {
public:
	GK_RASWorker(PPER_Stream initial_pdu, PIPSocket::Address rx_addr, WORD rx_port, GK_RASListener & server);
	virtual ~GK_RASWorker() =0;
	virtual void Main(); // Do the actual work.
protected:
	virtual void OnUnknown(H225_RasMessage &pdu);
	virtual void SendRas();
	virtual void ForwardRasMsg(H225_RasMessage &msg);

	// Need this for internal use.
	virtual void ProcessARQ(endptr &RequestingEP, endptr &CalledEP, H225_AdmissionRequest &arq, BOOL &bReject);


	PPER_Stream raw_pdu;
	PIPSocket::Address addr;
	WORD port;
	H225_RasMessage answer_pdu;
	H225_RasMessage pdu;
	GK_RASListener & master;
	BOOL need_answer;
};

class H323RasWorker : public GK_RASWorker {
public:
	H323RasWorker(PPER_Stream initial_pdu, PIPSocket::Address rx_addr, WORD rx_port, GK_RASListener & server);
	virtual ~H323RasWorker();
	virtual void Main(); // Do the actual work.
	virtual void Terminate();
protected:
	virtual void OnGRQ(H225_GatekeeperRequest &gqr);
	virtual void OnRRQ(H225_RegistrationRequest &gqr);
	virtual void OnURQ(H225_UnregistrationRequest &gqr);
	virtual void OnARQ(H225_AdmissionRequest &gqr);
	virtual void OnDRQ(H225_DisengageRequest &gqr);
	virtual void OnBRQ(H225_BandwidthRequest &gqr);
	virtual void OnLRQ(H225_LocationRequest &gqr);
	virtual void OnIRR(H225_InfoRequestResponse &gqr);
	virtual void OnRAI(H225_ResourcesAvailableIndicate &gqr);
private:
	GkAuthenticatorList *authList;
};

class NeighborWorker : public GK_RASWorker {
public:
	NeighborWorker(PPER_Stream initial_pdu, endptr called, PIPSocket::Address rx_addr, WORD rx_port, GK_RASListener & server);
	NeighborWorker(PPER_Stream initial_pdu, PIPSocket::Address rx_addr, WORD rx_port, GK_RASListener & server);
	virtual ~NeighborWorker();
	virtual void Main(); // Do the actual work.
protected:
	virtual void OnARQ(H225_AdmissionRequest &arq);
private:
	endptr m_called;
};

#endif /* RASWORKER_H */
