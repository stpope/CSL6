///
///  CslRtpSession.cpp -- Contains a subclass of RTPSession, which is the core of RTP
///
///  This code is common to both RtpSender and RtpReceiver, as these two units have a lot of
///  redundancy betwen them.
///

#include "CslRtpSession.h"

using namespace std;
using namespace csl;

CslRtpSession::CslRtpSession(unsigned numChans, unsigned bufferFrames) 
	: mRtpBuffer(numChans, bufferFrames), // Default to 1 mono channel being set,
	  mTmpBuffer(1, CGestalt::maxBufferFrames()) 	// Init the encapsulated buffer size (hard-coded to mono, for now)
{
//	RTPSession::RTPSession();
	mTmpBuffer.allocateMonoBuffers();				// Allocate the mTmpBuffer buffers
}

// ---------------------------------------------------------------
CslRtpSession::~CslRtpSession() {
	mTmpBuffer.freeMonoBuffers();							// Free the temp buffer, and delete it
}
/*
// ---------------------------------------------------------------
void CslRtpSession::OnPollThreadStep()	// If there are packets to read (or it's time to send an RTCP packet)
{
	BeginDataAccess();		// Call this when we access the RTP session, to prevent interference by other access
		
	// check incoming packets
	if (GotoFirstSourceWithData())
	{
		do		// Do the following for all waiting packets
		{
			RTPPacket *pack;
			RTPSourceData *srcdat;
			
			srcdat = GetCurrentSourceInfo();
			
			while ((pack = GetNextPacket()) != NULL)
			{
				ProcessRTPPacket(*srcdat,*pack);	// Process this packet if it exists
				delete pack;
			}
		} while (GotoNextSourceWithData());
	}		
	EndDataAccess();	// Release the RTP resources to be used elsewhere

	RTCPCompoundPacketBuilder 

}
*/
// ---------------------------------------------------------------
// OnRTCPCompoundPacket parses the compound packet to display all of the data
// available onto the console
//
void CslRtpSession::OnRTCPCompoundPacket(RTCPCompoundPacket *pack, const RTPTime &receivetime, const RTPAddress *senderaddress)
{
	RTCPPacket * rtcpPacket;

	// I know there should be a way to do this with polymorphism, but I can't figure it out...
	RTCPSRPacket * srPacket; RTCPRRPacket * rrPacket; RTCPSDESPacket * sdesPacket;
	RTCPBYEPacket * byePacket; RTCPAPPPacket * appPacket; RTCPUnknownPacket * unknownPacket;

	BeginDataAccess();
	cout << "RTCP Compound Packet (" << pack->GetCompoundPacketLength() << " bytes) \n";
	unsigned i = 0, chunkCnt = 0;
	bool avail = false;
	char * text;
	pack->GotoFirstPacket();	// Reset the packet iterator
	while (0 != (rtcpPacket = pack->GetNextPacket()))	// For all packets...
	{
		i++;
		cout << "  1: RTCP type " << rtcpPacket->GetPacketType() << " ";	// Display the info
		switch (rtcpPacket->GetPacketType()) {
			case RTCPPacket::SR:
				cout << "(SR) ";
				srPacket = (RTCPSRPacket*) rtcpPacket;
				cout << "SenderSSRC: " << srPacket->GetSenderSSRC() << " ";
				cout << "RTPtime: " << srPacket->GetRTPTimestamp() << "\n";
				cout << "     PackCnt: " << srPacket->GetSenderPacketCount() << " ";
				cout << "OctetCnt: " << srPacket->GetSenderOctetCount() << "\n";
				for (int j = 0; j < srPacket->GetReceptionReportCount(); j++) {
					cout << "    RR# " << i << ": ";
					cout << "SSRC: " << srPacket->GetSSRC(i) << " ";
					cout << "%Lost: " << 100*srPacket->GetFractionLost(i) << " ";
					cout << "#Lost: " << srPacket->GetLostPacketCount(i) << " ";
					cout << "HighestSeq: " << srPacket->GetExtendedHighestSequenceNumber(i) << " ";
					cout << "Jitter: " << srPacket->GetJitter(i) << " ";
					cout << "LSR: " << srPacket->GetLSR(i) << " ";
					cout << "DLSR: " << srPacket->GetDLSR(i) << "\n";
				}
				break;
			case RTCPPacket::RR:
				cout << "(RR) ";
				rrPacket = (RTCPRRPacket*) rtcpPacket;
				cout << "SenderSSRC: " << rrPacket->GetSenderSSRC() << " ";
				cout << "ReportCnt: " << rrPacket->GetReceptionReportCount() << "\n";
				for (int j = 0; j < rrPacket->GetReceptionReportCount(); j++) {
					cout << "    RR# " << i << ": ";
					cout << "SSRC: " << rrPacket->GetSSRC(i) << " ";
					cout << "%Lost: " << 100*rrPacket->GetFractionLost(i) << " ";
					cout << "#Lost: " << rrPacket->GetLostPacketCount(i) << "\n";
					cout << "    HighestSeq: " << rrPacket->GetExtendedHighestSequenceNumber(i) << " ";
					cout << "Jitter: " << rrPacket->GetJitter(i) << "\n";
					cout << "    LSR: " << rrPacket->GetLSR(i) << " ";
					cout << "DLSR: " << rrPacket->GetDLSR(i) << "\n";
				}
				break;
			case RTCPPacket::SDES:
				sdesPacket = (RTCPSDESPacket*) rtcpPacket;
				cout << "(SDES) ";
				cout << sdesPacket->GetChunkCount() << " chunks  \n";
				avail = sdesPacket->GotoFirstChunk();
				chunkCnt = 0;
				while (avail) {
					chunkCnt++;
					cout << "    (Chunk #" << chunkCnt << ") ";
					cout << "    SSRC: " << sdesPacket->GetChunkSSRC() << "\n";
					avail = sdesPacket->GotoNextChunk();
				}
				avail = sdesPacket->GotoFirstItem();
				chunkCnt = 0;
				while (avail) {
					chunkCnt++;
					cout << "    (Item #" << chunkCnt << ") ";
					cout << "    Type: " << sdesPacket->GetItemType() << " ";
					switch (sdesPacket->GetItemType()) {
						case RTCPSDESPacket::None:		cout << "(None) ";		break;
						case RTCPSDESPacket::CNAME:		cout << "(CNAME) ";		break;
						case RTCPSDESPacket::NAME:		cout << "(NAME) ";		break;
						case RTCPSDESPacket::EMAIL:		cout << "(EMAIL) ";		break;
						case RTCPSDESPacket::PHONE:		cout << "(PHONE) ";		break;
						case RTCPSDESPacket::LOC:		cout << "(LOC) ";		break;
						case RTCPSDESPacket::TOOL:		cout << "(TOOL) ";		break;
						case RTCPSDESPacket::NOTE:		cout << "(NOTE) ";		break;
						case RTCPSDESPacket::PRIV:		cout << "(PRIV) ";		break;
						case RTCPSDESPacket::Unknown:	cout << "(Unknown) ";	break;
					}
					cout << "(" << sdesPacket->GetItemLength() << " bytes) ";
					cout << (char *) sdesPacket->GetItemData() << "\n";
					avail = sdesPacket->GotoNextItem();
				}
				break;
			case RTCPPacket::BYE:
				cout << "(BYE) \n";
				break;
			case RTCPPacket::APP:
				cout << "(APP) \n";
				break;
			case RTCPPacket::Unknown:
				cout << "(Unknown) \n";
				break;
		}
		EndDataAccess();
	};
}


void CslRtpSession::OnRTPPacket(RTPPacket * pack, const RTPTime &receivetime, const RTPAddress *senderaddress)
{
	Interleaver interleaver;
	// You can inspect the packet and the source's info here
//	cout << "Packet # " << rtppack.GetExtendedSequenceNumber() << "\n"; // " from SSRC " << srcdat.GetSSRC() << "\n";
//	cout << rtppack.GetPayloadLength() << " bytes of data with timestamp " << rtppack.GetTimestamp() << "\n";
//	cout << "With a type of " << rtppack.GetPayloadType() << "\n";

	BeginDataAccess();
	unsigned payloadLength = pack->GetPayloadLength() / sizeof(short);
	RTPTime diffTime(receivetime.CurrentTime());		// packetTime now contains the network latency of this packet
	diffTime -= receivetime;

/*	cout	<< "inf: " << srcdat.SR_Prev_GetRTPTimestamp() << " "
			<< srcdat.SR_GetRTPTimestamp() << " "
			<< srcdat.RR_GetReceiveTime().GetDouble()
			<< srcdat.RR_Prev_GetReceiveTime().GetDouble()
			<< srcdat.INF_GetRoundtripTime().GetDouble() "\n";
*/
	if ((pack->GetSequenceNumber() % 100) == 0) {
		cout << "Net latency: ";
		cout << diffTime.GetDouble() * 1000. << "ms "
	//			  << packetTime.GetNTPTime().GetMSW() << "."
	//			  << packetTime.GetNTPTime().GetLSW() << " "
				  << "Seq# " << pack->GetExtendedSequenceNumber() << " "
				  << "Timestamp " << pack->GetTimestamp() << "\n";
	}


//	short * payload = (short *) rtppack.GetPayloadData();		// Get the payload data into a short array
	mTmpBuffer.setSizeOnly(1,payloadLength);							// Set the temp buffer's payload length

	interleaver.deinterleave(mTmpBuffer, (short *) pack->GetPayloadData(), payloadLength,1);	// Deinterleave the payload into the buffer
	EndDataAccess();
	mRtpBuffer.writeBuffer(mTmpBuffer, 0);								// Write the temp Buffer into the Ring Buffer
}
/*
// ---------------------------------------------------------------
void CslRtpSession::ProcessRTPPacket(const RTPSourceData &srcdat,const RTPPacket &rtppack)
{
	Interleaver interleaver;
	// You can inspect the packet and the source's info here
//	cout << "Packet # " << rtppack.GetExtendedSequenceNumber() << "\n"; // " from SSRC " << srcdat.GetSSRC() << "\n";
//	cout << rtppack.GetPayloadLength() << " bytes of data with timestamp " << rtppack.GetTimestamp() << "\n";
//	cout << "With a type of " << rtppack.GetPayloadType() << "\n";

	RTPTime packetTime(rtppack.GetReceiveTime());
	RTPTime diffTime(packetTime.CurrentTime());		// packetTime now contains the network latency of this packet
	diffTime -= packetTime;

//	cout	<< "inf: " << srcdat.SR_Prev_GetRTPTimestamp() << " "
//			<< srcdat.SR_GetRTPTimestamp() << " "
//			<< srcdat.RR_GetReceiveTime().GetDouble()
//			<< srcdat.RR_Prev_GetReceiveTime().GetDouble()
//			<< srcdat.INF_GetRoundtripTime().GetDouble() "\n";

	unsigned payloadLength = rtppack.GetPayloadLength() / sizeof(short);
	cout << "Net latency: ";
	cout << diffTime.GetDouble() << "us "
//			  << packetTime.GetNTPTime().GetMSW() << "."
//			  << packetTime.GetNTPTime().GetLSW() << " "
			  << "Seq# " << rtppack.GetSequenceNumber() << " "
			  << "Timestamp " << rtppack.GetTimestamp() << "\n";


//	short * payload = (short *) rtppack.GetPayloadData();		// Get the payload data into a short array
	tBuf->setSizeOnly(1,payloadLength);							// Set the temp buffer's payload length

	interleaver.deinterleave(*tBuf, (short *) rtppack.GetPayloadData(), payloadLength,1);	// Deinterleave the payload into the buffer
	buffer->writeBuffer(*tBuf, 0);								// Write the temp Buffer into the Ring Buffer

} */

void CslRtpSession::OnSSRCCollision(RTPSourceData *srcdat,const RTPAddress *senderaddress,bool isrtp)
{
	cout << "SSRC Collision!" << "\n";
}
void CslRtpSession::OnCNAMECollision(RTPSourceData *srcdat,const RTPAddress *senderaddress,
	                              const u_int8_t *cname,size_t cnamelength)
{
	cout << "CNAME Collision: " << (char *) cname << "\n";
	return;
}
void CslRtpSession::OnNewSource(RTPSourceData *srcdat)
{
	cout << "New source:  SSRC: " << srcdat->GetSSRC() << " ";
	cout << "RTP:  " << srcdat->GetRTPDataAddress() << " ";
	cout << "RTCP: " << srcdat->GetRTCPDataAddress() << "\n";
//	if (!srcdat->IsOwnSSRC())
//		this->AddDestination(srcdat->GetSSRC());
	return;
}
void CslRtpSession::OnRemoveSource(RTPSourceData *srcdat)
{
	cout << "Source removed:  SSRC: " << srcdat->GetSSRC() << " ";
	cout << "RTP:  " << srcdat->GetRTPDataAddress() << " ";
	cout << "RTCP: " << srcdat->GetRTCPDataAddress() << "\n";
	return;
}
void CslRtpSession::OnTimeout(RTPSourceData *srcdat)
{
	cout << "Timeout:  SSRC: " << srcdat->GetSSRC() << " ";
	cout << "RTP:  " << srcdat->GetRTPDataAddress() << " ";
	cout << "RTCP: " << srcdat->GetRTCPDataAddress() << "\n";
	return;
}
void CslRtpSession::OnBYETimeout(RTPSourceData *srcdat)
{
	cout << "BYE Timeout:  SSRC: " << srcdat->GetSSRC() << " ";
	cout << "RTP:  " << srcdat->GetRTPDataAddress() << " ";
	cout << "RTCP: " << srcdat->GetRTCPDataAddress() << "\n";
	return;
}
void CslRtpSession::OnAPPPacket(RTCPAPPPacket *apppacket,const RTPTime &receivetime,
	                         const RTPAddress *senderaddress)
{
	cout << "APP Packet:  SSRC: " << apppacket->GetSSRC() << " ";
	cout  << (char *) apppacket->GetName() << ": ";
	cout << "(" << apppacket->GetAPPDataLength() << " bytes)\n";
	return;
}
void CslRtpSession::OnUnknownPacketType(RTCPPacket *rtcppack,const RTPTime &receivetime,
	                                 const RTPAddress *senderaddress)
{
	cout << "Unknown Packet Type: ";
	cout << "(" << rtcppack->GetPacketLength() << " bytes)\n";
	return;
}
void CslRtpSession::OnUnknownPacketFormat(RTCPPacket *rtcppack,const RTPTime &receivetime,
	                                   const RTPAddress *senderaddress)
{
	cout << "Unknown Packet Type: ";
	cout << "(" << rtcppack->GetPacketLength() << " bytes)\n";
	return;
}
void CslRtpSession::OnNoteTimeout(RTPSourceData *srcdat)
{
	cout << "Note Timeout:  SSRC: " << srcdat->GetSSRC() << " ";
	cout << "RTP:  " << srcdat->GetRTPDataAddress() << " ";
	cout << "RTCP: " << srcdat->GetRTCPDataAddress() << "\n";
	return;
}
void CslRtpSession::OnBYEPacket(RTPSourceData *srcdat)
{
	cout << "BYE Packet:  SSRC: " << srcdat->GetSSRC() << " ";
	cout << "RTP:  " << srcdat->GetRTPDataAddress() << " ";
	cout << "RTCP: " << srcdat->GetRTCPDataAddress() << "\n";
	return;
}