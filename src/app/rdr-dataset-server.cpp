#include "rdr-dataset-server.hpp"
#include "../core/with-components-buffer.hpp"
#include "../ndn-cpp/lite/encoding/tlv-0_2-wire-format-lite.hpp"
#include "../ndn-cpp/lite/util/dynamic-malloc-uint8-array-lite.hpp"

#include <cstring>

namespace ndn {

static const char* RDR_METADATA_V = "metadata";
static const ndn::NameLite::Component RDR_METADATA(reinterpret_cast<const uint8_t*>(RDR_METADATA_V), strlen(RDR_METADATA_V),
                                                   ndn_NameComponentType_OTHER_CODE, 32);

RdrDatasetServer::Context::Context(RdrDatasetServer& server, const ndn::InterestLite& interest, uint64_t endpointId)
  : server(server)
  , interest(interest)
  , endpointId(endpointId)
{
}

const ndn::NameLite::Component&
RdrDatasetServer::Context::getParam(int i) const
{
  return interest.getName().get(paramOffset + i);
}

bool
RdrDatasetServer::begin(Face& face, const NameLite& prefix, VersionFormat versionFormat)
{
  this->prefix = &prefix;
  switch (versionFormat) {
    case VersionFormat::VERSION:
      m_toVersion = &ndn::NameLite::Component::toVersion;
      m_fromVersion = &ndn::NameLite::Component::setVersion;
      break;
    case VersionFormat::TIMESTAMP:
      m_toVersion = &ndn::NameLite::Component::toTimestamp;
      m_fromVersion = &ndn::NameLite::Component::setTimestamp;
      break;
  }
  return face.addHandler(this);
}

bool
RdrDatasetServer::processInterest(const ndn::InterestLite& interest, uint64_t endpointId)
{
  const ndn::NameLite& name = interest.getName();
  if (prefix == nullptr || !prefix->match(name)) {
    return false;
  }

  Context ctx(*this, interest, endpointId);
  if (name.size() > prefix->size() && name.get(-1).equals(RDR_METADATA) &&
      interest.getCanBePrefix() && interest.getMustBeFresh()) {
    ctx.isRdr = true;
    ctx.paramOffset = prefix->size();
    ctx.paramCount = name.size() - prefix->size() - 1;
    return processRdrDiscovery(ctx);
  }
  else if (name.size() >= prefix->size() + 2 &&
           name.get(-1).toSegment(ctx.segment) == NDN_ERROR_success &&
           (name.get(-2).*m_toVersion)(ctx.version) == NDN_ERROR_success) {
    ctx.isRdr = false;
    ctx.paramOffset = prefix->size();
    ctx.paramCount = name.size() - prefix->size() - 2;
    return processSegmentInterest(ctx);
  }
  return false;
}

ndn_Error
RdrDatasetServer::sendRdrMetadata(Context& ctx, uint64_t datasetVersion, uint64_t rdrVersion)
{
  uint8_t versionBuf[9];
  ndn::NameLite::Component versionComp;
  (versionComp.*m_fromVersion)(datasetVersion, versionBuf, sizeof(versionBuf));

  const ndn::NameLite& interestName = ctx.interest.getName();
  ndn::NameACB versioned(interestName.size());
  versioned.set(interestName);
  versioned.pop();
  versioned.append(versionComp);

  ndn::DynamicMallocUInt8ArrayLite payloadBuf(0);
  size_t signedBegin, signedEnd, payloadLen;
  ndn_Error error = ndn::Tlv0_2WireFormatLite::encodeName(
    versioned, &signedBegin, &signedEnd, payloadBuf, &payloadLen);
  if (error != NDN_ERROR_success) {
    return error;
  }

  if (rdrVersion == 0) {
    rdrVersion = datasetVersion;
  }

  ndn::DataACB data(interestName.size() + 1, 0);
  ndn::NameLite& dataName = data.getName();
  dataName.set(interestName);
  // reuse versionBuf: version in payload has been copied to payloadBuf
  dataName.appendVersion(rdrVersion, versionBuf, sizeof(versionBuf));
  data.getMetaInfo().setFreshnessPeriod(1);
  data.getMetaInfo().setFinalBlockId(data.getName().get(-1));
  data.setContent(ndn::BlobLite(payloadBuf.getArray(), payloadLen));
  return getFace()->sendData(data, ctx.endpointId);
}

ndn_Error
RdrDatasetServer::sendSegment(Context& ctx, const BlobLite& content, bool isFinalBlock, Milliseconds freshnessPeriod)
{
  const ndn::NameLite& interestName = ctx.interest.getName();
  ndn::DataACB data(interestName.size(), 0);
  data.getName().set(interestName);
  data.getMetaInfo().setFreshnessPeriod(freshnessPeriod);
  if (isFinalBlock) {
    data.getMetaInfo().setFinalBlockId(data.getName().get(-1));
  }
  data.setContent(content);
  return getFace()->sendData(data, ctx.endpointId);
}

} // namespace ndn
