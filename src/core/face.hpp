#ifndef ESP8266NDN_FACE_HPP
#define ESP8266NDN_FACE_HPP

#include "../ndn-cpp/lite/interest-lite.hpp"
#include "../ndn-cpp/lite/data-lite.hpp"
#include "../ndn-cpp/c/util/crypto.h"
#include "transport.hpp"

namespace ndn {

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
 */
#define NDNFACE_KEYNAMECOMPS_MAX 12
/** \brief max packets to receive and process on each loop
 */
#define NDNFACE_RECEIVE_MAX 4
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

  /** \brief set HMAC signing key
   *
   *  To generate a random HMAC key in Python:
   *  \code{.py}
   *  import random
   *  print ',\n'.join([ ', '.join([ '{0:#04x}'.format(random.randint(0,255)) for i in range(16) ]) for j in range(4) ])
   *  \endcode
   */
  void
  setHmacKey(const uint8_t* key, size_t keySize);

  /** \brief loop the underlying transport
   */
  void
  loop();

  /** \brief send a packet directly through underlying transport
   */
  void
  sendPacket(const uint8_t* pkt, size_t len, uint64_t endpointId = 0);

  /** \brief send an Interest
   */
  void
  sendInterest(InterestLite& interest, uint64_t endpointId = 0);

  /** \brief send a Data
   *  \pre HMAC signing key is set
   *
   *  The Data will be signed by the HMAC key before sent out.
   */
  void
  sendData(DataLite& data, uint64_t endpointId = 0);

private:
  /** \brief process an incoming packet
   */
  void
  processPacket(const uint8_t* pkt, size_t len, uint64_t endpointId);

  void
  processInterest(const uint8_t* pkt, size_t len, uint64_t endpointId);

  void
  processData(const uint8_t* pkt, size_t len, uint64_t endpointId);

private:
  Transport& m_transport;

  InterestCallback m_interestCb;
  void* m_interestCbArg;
  DataCallback m_dataCb;
  void* m_dataCbArg;

  const uint8_t* m_hmacKey;
  size_t m_hmacKeySize;
  uint8_t m_hmacKeyDigest[ndn_SHA256_DIGEST_SIZE];
};

} // namespace ndn

#endif // ESP8266NDN_FACE_HPP
