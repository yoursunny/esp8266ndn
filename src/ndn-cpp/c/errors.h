/**
 * Define error codes and ndn_getErrorString to convert to a string.
 * Copyright (C) 2013-2018 Regents of the University of California.
 * @author, Jeff Thompson <jefft0@remap.ucla.edu>
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

#ifndef NDN_ERRORS_H
#define NDN_ERRORS_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  NDN_ERROR_success = 0,
  NDN_ERROR_element_of_value_is_not_a_decimal_digit,
  NDN_ERROR_read_past_the_end_of_the_input,
  NDN_ERROR_the_first_header_octet_may_not_be_zero,
  NDN_ERROR_header_type_is_not_a_DTAG,
  NDN_ERROR_did_not_get_the_expected_DTAG,
  NDN_ERROR_did_not_get_the_expected_element_close,
  NDN_ERROR_item_is_not_UDATA,
  NDN_ERROR_header_type_is_out_of_range,
  NDN_ERROR_encodeTypeAndValue_miscalculated_N_encoding_bytes,
  NDN_ERROR_attempt_to_add_a_component_past_the_maximum_number_of_components_allowed_in_the_name,
  NDN_ERROR_cannot_add_an_entry_past_the_maximum_number_of_entries_allowed_in_the_exclude,
  NDN_ERROR_cannot_add_an_entry_past_the_maximum_number_of_entries_allowed_in_the_LpPacket,
  NDN_ERROR_findElementEnd_unexpected_close_tag,
  NDN_ERROR_cannot_store_more_header_bytes_than_the_size_of_headerBuffer,
  NDN_ERROR_findElementEnd_cannot_read_header_type_and_value,
  NDN_ERROR_findElementEnd_unrecognized_header_type,
  NDN_ERROR_findElementEnd_unrecognized_state,
  NDN_ERROR_DynamicUInt8Array_realloc_function_pointer_not_supplied,
  NDN_ERROR_DynamicUInt8Array_realloc_failed,
  NDN_ERROR_unrecognized_ndn_ExcludeType,
  NDN_ERROR_unrecognized_ndn_ContentType,
  NDN_ERROR_unrecognized_ndn_KeyLocatorType,
  NDN_ERROR_unrecognized_ndn_KeyNameType,
  NDN_ERROR_decodeKeyLocator_unrecognized_key_locator_type,
  NDN_ERROR_unrecognized_ndn_SocketTransport,
  NDN_ERROR_SocketTransport_error_in_getaddrinfo,
  NDN_ERROR_SocketTransport_cannot_connect_to_socket,
  NDN_ERROR_SocketTransport_socket_is_not_open,
  NDN_ERROR_SocketTransport_error_in_send,
  NDN_ERROR_SocketTransport_error_in_poll,
  NDN_ERROR_SocketTransport_error_in_recv,
  NDN_ERROR_SocketTransport_error_in_close,
  NDN_ERROR_Name_component_does_not_begin_with_the_expected_marker,
  NDN_ERROR_Time_functions_are_not_supported_by_the_standard_library,
  NDN_ERROR_Calendar_time_value_out_of_range,
  NDN_ERROR_Invalid_length_for_nonNegativeInteger,
  NDN_ERROR_did_not_get_the_expected_TLV_type,
  NDN_ERROR_TLV_length_exceeds_buffer_length,
  NDN_ERROR_TLV_length_does_not_equal_total_length_of_nested_TLVs,
  NDN_ERROR_decodeSignatureInfo_unrecognized_SignatureInfo_type,
  NDN_ERROR_encodeSignatureInfo_unrecognized_SignatureType,
  NDN_ERROR_Unimplemented_operation,
  NDN_ERROR_ElementReader_ElementListener_is_not_specified,
  NDN_ERROR_ElementReader_The_incoming_packet_exceeds_the_maximum_limit_getMaxNdnPacketSize,
  NDN_ERROR_Interest_has_a_selected_delegation_but_no_link_object,
  NDN_ERROR_The_Generic_signature_encoding_is_not_a_valid_NDN_TLV_SignatureInfo,
  NDN_ERROR_Error_encoding_key,
  NDN_ERROR_Error_decoding_key,
  NDN_ERROR_Error_in_sign_operation,
  NDN_ERROR_Unsupported_algorithm_type,
  NDN_ERROR_Error_in_encrypt_operation,
  NDN_ERROR_Error_in_decrypt_operation,
  NDN_ERROR_Error_in_generate_operation,
  NDN_ERROR_Incorrect_key_size,
  NDN_ERROR_Incorrect_initial_vector_size,
  NDN_ERROR_Incorrect_digest_size,
  NDN_ERROR_An_Interest_may_not_have_a_selected_delegation_when_encoding_a_forwarding_hint,
  NDN_ERROR_An_Interest_may_not_have_a_link_object_when_encoding_a_forwarding_hint
} ndn_Error;

/**
 * Convert the error code to its string.
 * @param error the error code
 * @return the error string or "unrecognized ndn_Error code"
 */
const char *
ndn_getErrorString(int error);

#ifdef __cplusplus
}
#endif

#endif
