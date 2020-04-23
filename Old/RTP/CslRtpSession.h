///
///  CslRTPSession.h -- Contains a subclass of RTPSession, which is the core of RTP
///
//	Permission is hereby granted, free of charge, to any person obtaining a
//	copy of this software and associated documentation files (the "Software"),
//	to deal in the Software without restriction, including without limitation
//	the rights to use, copy, modify, merge, publish, distribute, sublicense,
//	and/or sell copies of the Software, and to permit persons to whom the
//	Software is furnished to do so, subject to the following conditions:
//
//	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
//	THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
//	FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
//	DEALINGS IN THE SOFTWARE.
//	--------------------------------------------------------------------------
///
/// What's here:
///
/// CslRtpSession: Adds functionality for handling RTP packets sent by the server.
///
/// TODO: Need to unify the client/server main programs into one, so that the user has the option
/// of either transmitting data or listening to a port. Also need to extend functionality so that
/// the client specifies the server to listen to, rather than the server adding clients manually.
/// We also need to write some methods to access the ring buffer, rather than through direct access.

#ifndef CSL_CSLRTPSESSION_H
#define CSL_CSLRTPSESSION_H

#include "CSL_Core.h"
#include "Interleaver.h"
#include "RingBuffer.h"
#include "CslRtpIncludes.h"
#include <iostream>

namespace csl {

/// Enumeration to define the possible states of our internal RingBuffer
enum RtpBufferState {
	kNormal = 0,
	kBuffering,
	kOverflow,
	kUnderrun,
	kInactive,

	kNumStates
};

class CslRtpSession : public RTPSession
{
public:
	CslRtpSession(unsigned numChans, unsigned bufferFrames);			///< Constructor
	~CslRtpSession();		///< Destructor
	RingBuffer mRtpBuffer;	///< The ring buffer between the RTP client and the PortAudio object
	Buffer mTmpBuffer;			///< A temporary buffer used throughout the class
protected:
	void OnRTPPacket(RTPPacket * pack, const RTPTime &receivetime, const RTPAddress *senderaddress);
	void OnRTCPCompoundPacket(RTCPCompoundPacket *pack, const RTPTime &receivetime, const RTPAddress *senderaddress);

	void OnSSRCCollision(RTPSourceData *srcdat,const RTPAddress *senderaddress,bool isrtp);
	void OnCNAMECollision(RTPSourceData *srcdat,const RTPAddress *senderaddress,
	                      const u_int8_t *cname,size_t cnamelength);
	void OnNewSource(RTPSourceData *srcdat);
	void OnRemoveSource(RTPSourceData *srcdat);
	void OnTimeout(RTPSourceData *srcdat);
	void OnBYETimeout(RTPSourceData *srcdat);
	void OnAPPPacket(RTCPAPPPacket *apppacket,const RTPTime &receivetime,
	                 const RTPAddress *senderaddress);
	void OnUnknownPacketType(RTCPPacket *rtcppack,const RTPTime &receivetime,
	                         const RTPAddress *senderaddress);
	void OnUnknownPacketFormat(RTCPPacket *rtcppack,const RTPTime &receivetime,
	                           const RTPAddress *senderaddress);
	void OnNoteTimeout(RTPSourceData *srcdat);
	void OnBYEPacket(RTPSourceData *srcdat);

};

};

#endif