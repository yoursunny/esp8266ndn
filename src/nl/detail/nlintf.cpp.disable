#include "nlintf.hpp"

#include "../../ndn-lite/ndn-error-code.h"
#include "../../ndn-lite/forwarder/forwarder.h"

#include <cstring>

namespace ndn {
namespace detail {

static int
nop_updown(ndn_face_intf* self)
{
  return NDN_SUCCESS;
}

static void
nop_destroy(ndn_face_intf* self)
{
}

NlIntf::NlIntf(int intfType, ndn_face_intf_send sendFunc)
{
  memset(this, 0, sizeof(ndn_face_intf_t));
  type = intfType;
  state = NDN_FACE_STATE_UP;
  up = nop_updown;
  down = nop_updown;
  send = sendFunc;
  destroy = nop_destroy;
  face_id = NDN_INVALID_ID;
}

NlIntf::~NlIntf()
{
  end();
}

bool
NlIntf::begin()
{
  int res = ndn_forwarder_register_face(this);
  return res == NDN_SUCCESS || res == NDN_FWD_NO_EFFECT;
}

bool
NlIntf::end()
{
  int res = ndn_forwarder_unregister_face(this);
  return res == NDN_SUCCESS || res == NDN_FWD_NO_EFFECT;
}

int
NlIntf::intfReceive(const uint8_t* pkt, size_t len)
{
  return ndn_forwarder_receive(this, const_cast<uint8_t*>(pkt), len);
}

ndn_Error
NlIntf::addRoute(const NameLite& name)
{
  if (name.size() > NDN_NAME_COMPONENTS_SIZE) {
    return NDN_ERROR_attempt_to_add_a_component_past_the_maximum_number_of_components_allowed_in_the_name;
  }
  ndn_name_t nlName = {0};
  nlName.components_size = name.size();

  for (size_t i = 0; i < name.size(); ++i) {
    const NameLite::Component& comp = name.get(i);
    const BlobLite& value = comp.getValue();
    if (value.size() > NDN_NAME_COMPONENT_BUFFER_SIZE) {
      return NDN_ERROR_TLV_length_exceeds_buffer_length;
    }
    name_component_t* nlComp = &nlName.components[i];
    nlComp->type = comp.getType() == ndn_NameComponentType_OTHER_CODE ?
                   comp.getOtherTypeCode() : comp.getType();
    nlComp->size = comp.getValue().size();
    if (!value.isNull()) {
      std::memcpy(nlComp->value, value.buf(), value.size());
    }
  }

  return ndn_forwarder_add_route_by_name(this, &nlName) == NDN_SUCCESS ?
         NDN_ERROR_success : NDN_ERROR_SocketTransport_error_in_getaddrinfo;
}

} // namespace detail
} // namespace ndn
