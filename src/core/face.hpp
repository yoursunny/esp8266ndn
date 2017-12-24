#ifndef ESP8266NDN_FACE_HPP
#define ESP8266NDN_FACE_HPP

#include "transport.hpp"

#include "../ndn-cpp/lite/data-lite.hpp"
#include "../ndn-cpp/lite/interest-lite.hpp"
#include "../ndn-cpp/lite/util/dynamic-uint8-array-lite.hpp"

namespace ndn {

class PrivateKey;
class PublicKey;

/** \brief Interest handler
 */
typedef void (*InterestCallback)(void* arg, const InterestLite& interest, uint64_t endpointId);
/** \brief Data handler
 */
typedef void (*DataCallback)(void* arg, const DataLite& data, uint64_t endpointId);

/** \brief incoming buffer size, in octets
 */
#define NDNFACE_INBUF_SIZE 1500
/** \brief max NameComponent count when parsing Name of incoming Interest/Data
 */
#define NDNFACE_NAMECOMPS_MAX 24
/** \brief max ExcludeEntry count when parsing Exclude of incoming Interest
 */
#define NDNFACE_EXCLUDE_MAX 4
/** \brief max NameComponent count when parsing KeyLocator of incoming Interest/Data
 *         or preparing outgoing signed Interest
 */
#define NDNFACE_KEYNAMECOMPS_MAX 12
/** \brief max packets to receive and process on each loop
 */
#define NDNFACE_RECEIVE_MAX 4
/** \brief buffer size for preparing outgoing signed Interest SignatureInfo, in octets
 */
#define NDNFACE_SIGINFOBUF_SIZE 128
/** \brief outgoing buffer size, in octets
 */
#define NDNFACE_OUTBUF_SIZE 1500

/** \brief a Face provides NDN communication between microcontroller and a remote NDN forwarder
 *
 *  This Face provides packet encoding and decoding, but does not have internal FIB or PIT.
 *  Every incoming packet will be delivered to the application.
 *  Application is also responsible for maintaining timers for Interest timeout if needed.
 */
class Face
{
public:
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

  /** \brief set signing key
   */
  void
  setSigningKey(const PrivateKey& pvtkey);

  /** \brief set HMAC signing key
   *  \deprecated use setSigningKey
   *
   *  To generate a random HMAC key in Python:
   *  \code{.py}
   *  import random
   *  print ',\n'.join([ ', '.join([ '{0:#04x}'.format(random.randint(0,255)) for i in range(16) ]) for j in range(4) ])
   *  \endcode
   */
  void
  setHmacKey(const uint8_t* key, size_t keySize) __attribute__((deprecated));

  /** \brief loop the underlying transport
   */
  void
  loop();

  /** \brief verify the signature on current Interest against given public key
   *
   *  This function is only available within onInterest callback.
   */
  bool
  verifyInterest(const PublicKey& pubkey) const;

  /** \brief verify the signature on current Data against given public key
   *
   *  This function is only available within onData callback.
   */
  bool
  verifyData(const PublicKey& pubkey) const;

  /** \brief send a packet directly through underlying transport
   */
  ndn_Error
  sendPacket(const uint8_t* pkt, size_t len, uint64_t endpointId = 0);

  /** \brief send an Interest
   */
  ndn_Error
  sendInterest(InterestLite& interest, uint64_t endpointId = 0);

  /** \brief send a signed Interest
   *  \param interest the unsigned Interest; must have 2 available name components
   *                  will become signed
   *  \pre signing key is set
   */
  ndn_Error
  sendSignedInterest(InterestLite& interest, uint64_t endpointId = 0);

  /** \brief send a Data
   *  \pre signing key is set
   *
   *  The Data will be signed by the HMAC key before sent out.
   */
  ndn_Error
  sendData(DataLite& data, uint64_t endpointId = 0);

private:
  void
  processPacket(size_t len, uint64_t endpointId);

  void
  processInterest(size_t len, uint64_t endpointId);

  void
  processData(size_t len, uint64_t endpointId);

  ndn_Error
  sendInterestImpl(InterestLite& interest, uint64_t endpointId, bool needSigning);

private:
  Transport& m_transport;

  InterestCallback m_interestCb;
  void* m_interestCbArg;
  DataCallback m_dataCb;
  void* m_dataCbArg;

  uint8_t m_inBuf[NDNFACE_INBUF_SIZE];
  uint8_t m_outBuf[NDNFACE_OUTBUF_SIZE];
  DynamicUInt8ArrayLite m_outArr;
  uint8_t m_sigInfoBuf[NDNFACE_SIGINFOBUF_SIZE];
  DynamicUInt8ArrayLite m_sigInfoArr;
  uint8_t* m_sigBuf;

  const PrivateKey* m_signingKey;
  bool m_ownsSigningKey;

  InterestLite* m_thisInterest;
  DataLite* m_thisData;
  size_t m_signedBegin;
  size_t m_signedEnd;
};

} // namespace ndn

#endif // ESP8266NDN_FACE_HPP
