#ifndef ESP8266NDN_RDR_DATASET_SERVER_HPP
#define ESP8266NDN_RDR_DATASET_SERVER_HPP

#include "../core/face.hpp"

namespace ndn {

/** \brief Producer to serve a versioned dataset with RDR version discovery.
 *  \sa https://redmine.named-data.net/projects/ndn-tlv/wiki/RDR
 */
class RdrDatasetServer : public PacketHandler
{
protected:
  enum class VersionFormat
  {
    VERSION,   ///< dataset version uses Version marker
    TIMESTAMP, ///< dataset version uses Timestamp marker
  };

  bool
  begin(Face& face, const NameLite& prefix, VersionFormat versionFormat = VersionFormat::VERSION);

  /** \brief Parsed incoming Interest.
   *
   *  RdrDatasetServer recognizes two Interest types:
   *
   *  \li RDR Interest: /prefix/param0/param1/32=metadata CanBePrefix=1 MustBeFresh=1
   *  \li Segment Interest: /prefix/param0/param1/version/segment
   *
   *  paramOffset and paramCount refer to the name components after prefix and before 32=metadata or version.
   *  version and segment are applicable to segment Interests only.
   */
  class Context
  {
  public:
    RdrDatasetServer& server;
    const ndn::InterestLite& interest;
    uint64_t endpointId = 0;
    bool isRdr = false;
    int paramOffset = 0;
    int paramCount = 0;
    uint64_t version = 0;
    uint64_t segment = 0;

  public:
    const ndn::NameLite::Component&
    getParam(int i) const;

  private:
    Context(RdrDatasetServer& server, const ndn::InterestLite& interest, uint64_t endpointId);
    friend class RdrDatasetServer;
  };

  /** \brief Transmit an RDR metadata packet.
   *  \param datasetVersion version of dataset.
   *  \param rdrVersion version of RDR metadata; if zero, use \p datasetVersion.
   */
  ndn_Error
  sendRdrMetadata(Context& ctx, uint64_t datasetVersion, uint64_t rdrVersion = 0);

  /** \brief Transmit a Data segment.
   *
   *  RdrDatasetServer does not currently perform dataset segmentation.
   */
  ndn_Error
  sendSegment(Context& ctx, const BlobLite& content, bool isFinalBlock = true, Milliseconds freshnessPeriod = 1000);

private:
  bool
  processInterest(const ndn::InterestLite& interest, uint64_t endpointId) override;

  virtual bool
  processRdrDiscovery(Context& ctx) = 0;

  virtual bool
  processSegmentInterest(Context& ctx) = 0;

protected:
  const NameLite* prefix = nullptr;

private:
  ndn_Error (ndn::NameLite::Component::*m_toVersion)(uint64_t&) const = nullptr;
  ndn_Error (ndn::NameLite::Component::*m_fromVersion)(uint64_t, uint8_t*, size_t) = nullptr;
};

} // namespace ndn

#endif // ESP8266NDN_RDR_DATASET_SERVER_HPP
