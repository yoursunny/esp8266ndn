#ifndef ESP8266NDN_PACKET_BUFFER_HPP
#define ESP8266NDN_PACKET_BUFFER_HPP

#include "../ndn-cpp/lite/data-lite.hpp"
#include "../ndn-cpp/lite/interest-lite.hpp"
#include "../ndn-cpp/lite/network-nack-lite.hpp"

#include <tuple>

namespace ndn {

class PublicKey;

enum class PacketType {
  NONE     = 0,
  INTEREST = 0x05,
  DATA     = 0x06,
  NACK     = 0x0320,
};

/** \brief a received packet and its associated memory buffers
 */
class PacketBuffer
{
public:
  class Options
  {
  public:
    uint16_t maxSize = 1500;
    uint16_t maxNameComps = 24;
    uint16_t maxKeyNameComps = 12;
  };

  /** \brief allocate memory buffers
   */
  explicit
  PacketBuffer(const Options& options);

  ~PacketBuffer();

  /** \brief clear parse result and return buffer for receiving next packet
   *  \return buffer and buffer size
   */
  std::tuple<uint8_t*, size_t>
  useBuffer();

  /** \brief parse received packet
   *  \param len packet length
   */
  ndn_Error
  parse(size_t len);

  /** \brief determine packet type
   */
  PacketType
  getPktType() const;

  enum RawLayer {
    RAW_L2 = 2,
    RAW_L3 = 3,
  };

  /** \brief retrieve raw L2 or L3 packet buffer
   */
  std::tuple<const uint8_t*, size_t>
  getRaw(RawLayer layer) const;

  /** \brief get parsed Interest
   *  \pre getPacketType() == PacketType::INTEREST || getPacketType() == PacketType::NACK
   */
  const InterestLite*
  getInterest() const;

  /** \brief get parsed Data
   *  \pre getPacketType() == PacketType::DATA
   */
  const DataLite*
  getData() const;

  /** \brief get parsed Nack
   *  \pre getPacketType() == PacketType::NACK
   */
  const NetworkNackLite*
  getNack() const;

  enum VerifyResult {
    VERIFY_OK,        ///< verify success
    VERIFY_NO_PKT,    ///< packet type is not verifiable
    VERIFY_PARSE_ERR, ///< unable to parse SignatureValue
    VERIFY_BAD_SIG,   ///< signature value is wrong
  };

  /** \brief verify packet signature against a public key
   */
  VerifyResult
  verify(const PublicKey& pubKey) const;

private:
  ndn_Error
  parsePacket();

  ndn_Error
  parseInterestOrNack(const NetworkNackLite* nack);

  ndn_Error
  parseData();

  ndn_Error
  parseLpPacket();

  VerifyResult
  verifyInterest(const PublicKey& pubKey) const;

  VerifyResult
  verifyData(const PublicKey& pubKey) const;

  VerifyResult
  verifySig(const PublicKey& pubKey, const ndn::BlobLite& sig) const;

public:
  uint64_t endpointId;

private:
  uint8_t* m_buf;
  const uint8_t* m_netPkt;
  ndn_NameComponent* m_nameComps;
  ndn_NameComponent* m_keyNameComps;
  const uint16_t m_maxSize;
  const uint16_t m_maxNameComps;
  const uint16_t m_maxKeyNameComps;
  uint16_t m_netPktLen;
  uint16_t m_signedBegin;
  uint16_t m_signedEnd;
  union {
    struct {
      ndn_NetworkNack m_nack;
      ndn_Interest m_interest;
    };
    ndn_Data m_data;
  };
};

} // namespace ndn

#endif // ESP8266NDN_PACKET_BUFFER_HPP
