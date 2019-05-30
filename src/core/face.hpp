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

  /** \brief access the internal receive buffer
   *  \sa swapPacketBuffer()
   */
  const PacketBuffer*
  getPacketBuffer() const
  {
    return m_pb;
  }

  /** \brief assign a new receive buffer, and return the current one
   *
   *  Face maintains an internal packet buffer for receiving packets. That
   *  buffer is overwritten every time \c loop() tries to receive a packet.
   *  If the application desires to retain a receive packet, the onInterest/
   *  onData/onNack callback may retrieve the current buffer using this
   *  function, and cause \c loop() to use another buffer for the next receive.
   *
   *  This function may also be used to assign the initial packet buffer.
   *  This is useful if a non-default PacketBuffer::Options is desired.
   *
   *  \param pb the new buffer; if nullptr, \c loop() will allocate a new
   *            packet buffer with default setting.
   *  \return the current buffer; nullptr if no internal buffer was allocated
   */
  PacketBuffer*
  swapPacketBuffer(PacketBuffer* pb);

  /** \brief receive and process up to \p packetLimit packets
   */
  void
  loop(int packetLimit = 4);

  /** \brief verify the signature on current Interest against given public key
   *
   *  This function is only available within onInterest callback before calling
   *  \c swapPacketBuffer(). Otherwise, invoke \c PacketBuffer::verify().
   */
  bool
  verifyInterest(const PublicKey& pubKey) const;

  /** \brief verify the signature on current Data against given public key
   *
   *  This function is only available within onData callback before calling
   *  \c swapPacketBuffer(). Otherwise, invoke \c PacketBuffer::verify().
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
  ndn_Error
  receive(uint64_t& endpointId);

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

  PacketBuffer* m_pb;

  PacketHandler* m_handler;
  bool m_wantNack;

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
