/**
 * Copyright (C) 2013-2016 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version, with the additional exemption that
 * compiling, linking, and/or using OpenSSL is allowed.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * A copy of the GNU Lesser General Public License is in the file COPYING.
 */

#include <ndn-cpp/c/errors.h>

const char *
ndn_getErrorString(int error)
{
  switch (error) {
  case NDN_ERROR_success:
    return      "Success";
  case NDN_ERROR_element_of_value_is_not_a_decimal_digit:
    return      "Element of value is not a decimal digit";
  case NDN_ERROR_read_past_the_end_of_the_input:
    return      "Read past the end of the input";
  case NDN_ERROR_the_first_header_octet_may_not_be_zero:
    return      "The first header octet may not be zero";
  case NDN_ERROR_header_type_is_not_a_DTAG:
    return      "Header type is not a DTAG";
  case NDN_ERROR_did_not_get_the_expected_DTAG:
    return      "Did not get the expected DTAG";
  case NDN_ERROR_did_not_get_the_expected_element_close:
    return      "Did not get the expected element close";
  case NDN_ERROR_item_is_not_UDATA:
    return      "Item is not UDATA";
  case NDN_ERROR_header_type_is_out_of_range:
    return      "Header type is out of range";
  case NDN_ERROR_encodeTypeAndValue_miscalculated_N_encoding_bytes:
    return      "EncodeTypeAndValue miscalculated N encoding bytes";
  case NDN_ERROR_attempt_to_add_a_component_past_the_maximum_number_of_components_allowed_in_the_name:
    return      "Attempt to add a component past the maximum number of components allowed in the name";
  case NDN_ERROR_cannot_add_an_entry_past_the_maximum_number_of_entries_allowed_in_the_exclude:
    return      "Cannot add an entry past the maximum number of entries allowed in the exclude";
  case NDN_ERROR_cannot_add_an_entry_past_the_maximum_number_of_entries_allowed_in_the_LpPacket:
    return      "Cannot add an entry past the maximum number of entries allowed in the LpPacket";
  case NDN_ERROR_findElementEnd_unexpected_close_tag:
    return      "FindElementEnd unexpected close tag";
  case NDN_ERROR_cannot_store_more_header_bytes_than_the_size_of_headerBuffer:
    return      "Cannot store more header bytes than the size of headerBuffer";
  case NDN_ERROR_findElementEnd_cannot_read_header_type_and_value:
    return      "FindElementEnd cannot read header type and value";
  case NDN_ERROR_findElementEnd_unrecognized_header_type:
    return      "FindElementEnd unrecognized header type";
  case NDN_ERROR_findElementEnd_unrecognized_state:
    return      "FindElementEnd unrecognized state";
  case NDN_ERROR_DynamicUInt8Array_realloc_function_pointer_not_supplied:
    return      "DynamicUInt8Array realloc function pointer not supplied";
  case NDN_ERROR_DynamicUInt8Array_realloc_failed:
    return      "DynamicUInt8Array realloc failed";
  case NDN_ERROR_unrecognized_ndn_ExcludeType:
    return      "Unrecognized ndn_ExcludeType";
  case NDN_ERROR_unrecognized_ndn_ContentType:
    return      "Unrecognized ndn_ContentType";
  case NDN_ERROR_unrecognized_ndn_KeyLocatorType:
    return      "Unrecognized ndn_KeyLocatorType";
  case NDN_ERROR_unrecognized_ndn_KeyNameType:
    return      "Unrecognized ndn_KeyNameType";
  case NDN_ERROR_decodeKeyLocator_unrecognized_key_locator_type:
    return      "decodeKeyLocator unrecognized key locator type";
  case NDN_ERROR_unrecognized_ndn_SocketTransport:
    return      "Unrecognized ndn_SocketTransport";
  case NDN_ERROR_SocketTransport_error_in_getaddrinfo:
    return      "SocketTransport error in getaddrinfo";
  case NDN_ERROR_SocketTransport_cannot_connect_to_socket:
    return      "SocketTransport cannot connect to socket";
  case NDN_ERROR_SocketTransport_socket_is_not_open:
    return      "SocketTransport socket is not open";
  case NDN_ERROR_SocketTransport_error_in_send:
    return      "SocketTransport error in send";
  case NDN_ERROR_SocketTransport_error_in_poll:
    return      "SocketTransport error in poll";
  case NDN_ERROR_SocketTransport_error_in_recv:
    return      "SocketTransport error in recv";
  case NDN_ERROR_SocketTransport_error_in_close:
    return      "SocketTransport error in close";
  case NDN_ERROR_Name_component_does_not_begin_with_the_expected_marker:
    return      "Name component does not begin with the expected marker";
  case NDN_ERROR_Time_functions_are_not_supported_by_the_standard_library:
    return      "Time functions are not supported by the standard library";
  case NDN_ERROR_Calendar_time_value_out_of_range:
    return      "Calendar time value out of range";
  case NDN_ERROR_Invalid_length_for_nonNegativeInteger:
    return      "Invalid length for nonNegativeInteger";
  case NDN_ERROR_did_not_get_the_expected_TLV_type:
    return      "Did not get the expected TLV type";
  case NDN_ERROR_TLV_length_exceeds_buffer_length:
    return      "TLV length exceeds buffer length";
  case NDN_ERROR_TLV_length_does_not_equal_total_length_of_nested_TLVs:
    return      "TLV length does not equal total length of nested TLVs";
  case NDN_ERROR_decodeSignatureInfo_unrecognized_SignatureInfo_type:
    return      "decodeSignatureInfo unrecognized SignatureInfo type";
  case NDN_ERROR_encodeSignatureInfo_unrecognized_SignatureType:
    return      "encodeSignatureInfo unrecognized SignatureType";
  case NDN_ERROR_Unimplemented_operation:
    return      "Unimplemented operation";
  case NDN_ERROR_ElementReader_ElementListener_is_not_specified:
    return      "ElementReader ElementListener is not specified";
  case NDN_ERROR_ElementReader_The_incoming_packet_exceeds_the_maximum_limit_getMaxNdnPacketSize:
    return      "ElementReader The incoming packet exceeds the maximum limit Face::getMaxNdnPacketSize()";
  case NDN_ERROR_Interest_has_a_selected_delegation_but_no_link_object:
    return      "Interest has a selected delegation but no link object";
  case NDN_ERROR_The_Generic_signature_encoding_is_not_a_valid_NDN_TLV_SignatureInfo:
    return      "The Generic signature encoding is not a valid NDN-TLV SignatureInfo";
  default:
    return "unrecognized ndn_Error code";
  }
}
