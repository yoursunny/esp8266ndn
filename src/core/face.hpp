#ifndef ESP8266NDN_FACE_HPP
#define ESP8266NDN_FACE_HPP

#include "packet-handler.hpp"

#include "../ndn-cpp/lite/util/dynamic-uint8-array-lite.hpp"
#include "../ndn-cpp/lite/util/dynamic-malloc-uint8-array-lite.hpp"

#include <memory>
#include <Print.h>

namespace ndn {

class PrivateKey;
class PublicKey;
class Transport;

/** \brief max NameComponent count when preparing outgoing signed Interest
 */
#define NDNFACE_KEYNAMECOMPS_MAX 12
/** \brief buffer size for preparing outgoing signed Interest SignatureInfo, in octets
 */
#define NDNFACE_SIGINFOBUF_SIZE 128
/** \brief outgoing buffer size, in octets
 */
#define NDNFACE_OUTBUF_SIZE 1500
/** \brief where to start encoding Interest when sending Nack
 */
#define NDNFACE_OUTNACK_HEADROOM 32

/** \brief a Face provides NDN communication between microcontroller and a remote NDN forwarder
 *
 *  This Face provides packet encoding and decoding, but does not have internal FIB or PIT.
 *  Every incoming packet will be delivered to the application.
 *  Application is also responsible for maintaining timers for Interest timeout if needed.
 */
class Face
{
public:
  explicit
  Face(Transport& transport);

  ~Face();

  /** \brief add a callback handler
   *  \param prio priority, smaller number means higher priority.
   */
  bool
  addHandler(PacketHandler* h, int8_t prio = 0);

  /** \brief remove a callback handler
   */
  bool
  removeHandler(PacketHandler* h);

  /** \brief enable per-packet tracing
   */
  void
  enableTracing(Print& output, const String& prefix = "");

  /** \brief set whether face should respond Nack~NoRoute upon unhandled Interest
   */
  void
  enableNack(bool wantNack)
  {
    m_wantNack = wantNack;
  }

  /** \brief set default signing key
   */
  void
  setSigningKey(const PrivateKey& pvtkey);

  /** \brief extract the packet buffer of current received packet
   *
   *  This function is only available within PacketHandler::process* functions.
   *  When a PacketBuffer is no longer needed, it should be promptly returned via \c pushPacketBuffer.
   */
  PacketBuffer*
  popReceiveBuffer();

  /** \brief add a receive buffer
   */
  void
  pushReceiveBuffer(PacketBuffer* pb);

  /** \brief allocate new receive buffers
   *  \return number of new receive buffers
   */
  int
  addReceiveBuffers(int count, const PacketBuffer::Options& options = {});

  /** \brief receive and process packets
   */
  void
  loop();

  /** \brief verify the signature on current Interest against given public key
   *
   *  This function is only available within PacketHandler::processInterest before
   *  calling \c popPacketBuffer(). Otherwise, invoke \c PacketBuffer::verify().
   */
  bool
  verifyInterest(const PublicKey& pubKey) const;

  /** \brief verify the signature on current Data against given public key
   *
   *  This function is only available within PacketHandler::processData before
   *  calling \c popPacketBuffer(). Otherwise, invoke \c PacketBuffer::verify().
   */
  bool
  verifyData(const PublicKey& pubKey) const;

  /** \brief send a packet directly through underlying transport
   */
  ndn_Error
  sendPacket(const uint8_t* pkt, size_t len, uint64_t endpointId = 0);

  /** \brief send an Interest
   */
  ndn_Error
  sendInterest(const InterestLite& interest, uint64_t endpointId = 0);

  /** \brief send a signed Interest
   *  \param[inout] interest the unsigned Interest; must have 2 available name components
   *  \param key private key, nullptr to use default signing key
   */
  ndn_Error
  sendSignedInterest(InterestLite& interest, uint64_t endpointId = 0,
                     const PrivateKey* pvtkey = nullptr);

  /** \brief send a Data
   *  \param key private key, nullptr to use default signing key
   */
  ndn_Error
  sendData(DataLite& data, uint64_t endpointId = 0, const PrivateKey* pvtkey = nullptr);

  /** \brief send a Nack
   */
  ndn_Error
  sendNack(const NetworkNackLite& nack, const InterestLite& interest, uint64_t endpointId = 0);

private:
  static void
  transportReceive(void* self, PacketBuffer* pb);

  void
  receive(PacketBuffer* pb);

  /** \brief send an Interest, possibly after signing
   */
  ndn_Error
  sendInterestImpl(InterestLite& interest, uint64_t endpointId, const PrivateKey* pvtkey);

  /** \brief sign [input, input+inputLen) into m_sigBuf
   *  \post m_sigBuf has SignatureValue TLV; signature starts at offset 2
   */
  ndn_Error
  signImpl(const PrivateKey& pvtkey, const uint8_t* input, size_t inputLen, int& sigLen);

private:
  Transport& m_transport;
  PacketHandler* m_handler;
  bool m_wantNack;
  bool m_addedReceiveBuffers;
  PacketBuffer* m_pb;

  class TracingHandler;
  std::unique_ptr<TracingHandler> m_tracing;

  uint8_t m_outBuf[NDNFACE_OUTBUF_SIZE];
  DynamicUInt8ArrayLite m_outArr;
  uint8_t m_sigInfoBuf[NDNFACE_SIGINFOBUF_SIZE];
  DynamicUInt8ArrayLite m_sigInfoArr;
  DynamicMallocUInt8ArrayLite m_sigBuf;

  const PrivateKey* m_signingKey;
};

} // namespace ndn

#endif // ESP8266NDN_FACE_HPP
