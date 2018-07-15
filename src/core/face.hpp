#ifndef ESP8266NDN_FACE_HPP
#define ESP8266NDN_FACE_HPP

#include "packet-buffer.hpp"

#include "../ndn-cpp/lite/util/dynamic-uint8-array-lite.hpp"

namespace ndn {

class PrivateKey;
class PublicKey;
class Transport;

/** \brief Interest handler
 */
typedef void (*InterestCallback)(void* arg, const InterestLite& interest, uint64_t endpointId);
/** \brief Data handler
 */
typedef void (*DataCallback)(void* arg, const DataLite& data, uint64_t endpointId);
/** \brief Nack handler
 */
typedef void (*NackCallback)(void* arg, const NetworkNackLite& nackHeader, const InterestLite& interest, uint64_t endpointId);

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

  /** \brief set incoming Interest handler
   *
   *  Only one handler is allowed. This overwrites any previous handler setting.
   */
  void
  onInterest(InterestCallback cb, void* cbarg);

  /** \brief set incoming Data handler
   *
   *  Only one handler is allowed. This overwrites any previous handler setting.
   */
  void
  onData(DataCallback cb, void* cbarg);

  /** \brief set incoming Nack handler
   *
   *  Only one handler is allowed. This overwrites any previous handler setting.
   */
  void
  onNack(NackCallback cb, void* cbarg);

  /** \brief set signing key
   */
  void
  setSigningKey(const PrivateKey& pvtkey);

  /** \brief receive and process up to \p packetLimit packets
   */
  void
  loop(int packetLimit = 4);

  /** \brief verify the signature on current Interest against given public key
   *
   *  This function is only available within onInterest callback.
   */
  bool
  verifyInterest(const PublicKey& pubKey) const;

  /** \brief verify the signature on current Data against given public key
   *
   *  This function is only available within onData callback.
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
   *  \param interest the unsigned Interest; must have 2 available name components
   *                  will become signed
   *  \pre signing key is set
   */
  ndn_Error
  sendSignedInterest(InterestLite& interest, uint64_t endpointId = 0);

  /** \brief send a Data
   *  \pre signing key is set
   */
  ndn_Error
  sendData(DataLite& data, uint64_t endpointId = 0);

  /** \brief send a Nack
   */
  ndn_Error
  sendNack(const NetworkNackLite& nack, const InterestLite& interest, uint64_t endpointId = 0);

private:
  ndn_Error
  receive(PacketBuffer* pb, uint64_t* endpointId);

  ndn_Error
  sendInterestImpl(InterestLite& interest, uint64_t endpointId, bool needSigning);

private:
  Transport& m_transport;

  PacketBuffer* m_pb;

  InterestCallback m_interestCb;
  void* m_interestCbArg;
  DataCallback m_dataCb;
  void* m_dataCbArg;
  NackCallback m_nackCb;
  void* m_nackCbArg;

  uint8_t m_outBuf[NDNFACE_OUTBUF_SIZE];
  DynamicUInt8ArrayLite m_outArr;
  uint8_t m_sigInfoBuf[NDNFACE_SIGINFOBUF_SIZE];
  DynamicUInt8ArrayLite m_sigInfoArr;
  uint8_t* m_sigBuf;

  const PrivateKey* m_signingKey;
};

} // namespace ndn

#endif // ESP8266NDN_FACE_HPP
