/**
    Copyright Notice:
    Copyright 2021 DMTF. All rights reserved.
    License: BSD 3-Clause License. For full text see link: https://github.com/DMTF/libspdm/blob/main/LICENSE.md
**/

#include "spdm_unit_test.h"
#include "internal/libspdm_requester_lib.h"

 
#pragma pack(1)
typedef struct {
    spdm_message_header_t header;
    uint8_t reserved;
    uint8_t version_number_entry_count;
    spdm_version_number_t version_number_entry[LIBSPDM_MAX_VERSION_COUNT];
} spdm_version_response_mine_t;
#pragma pack()

///////////////////////////////////////////////////////////////////

// declare global vairables that can be used in the asserions

spdm_version_response_mine_t spdm_response_global;
spdm_context_t my_context;

return_status spdm_requester_get_version_test_send_message(
    IN void *context, IN uintn request_size, IN void *request,
    IN uint64_t timeout)
{
    spdm_context_t *spdm_context = (spdm_context_t *)context;
    spdm_get_version_request_t *spdm_request = (spdm_get_version_request_t *)request;

    //assert(spdm_context->connection_info.connection_state ==
    //    LIBSPDM_CONNECTION_STATE_NOT_STARTED);
    //assert(spdm_request->header.spdm_version == SPDM_MESSAGE_VERSION_10);
    //assert(spdm_request->header.request_response_code == SPDM_GET_VERSION);
    //assert(spdm_request->header.param1 == 0);
    //assert(spdm_request->header.param2 == 0);

    return_status status;
    return status;
}

return_status spdm_requester_get_version_test_receive_message(
    IN void *spdm_context, IN OUT uintn *response_size,
    IN OUT void *response, IN uint64_t timeout)
{
    spdm_version_response_mine_t *spdm_response = (spdm_version_response_mine_t *) response;

    spdm_response->header.spdm_version = SPDM_MESSAGE_VERSION_10;
    spdm_response->header.request_response_code = SPDM_VERSION;
    spdm_response->header.param1 = 0;
    spdm_response->header.param2 = 0;
    
    // setting from 1 to 5 symbolically defined versions on the responder side
    unsigned int local_ver_count = __VERIFIER_nondet_uint();
    __ESBMC_assume(local_ver_count > 0);
    __ESBMC_assume(local_ver_count <= LIBSPDM_MAX_VERSION_COUNT);
    spdm_response->version_number_entry_count = local_ver_count;
    for(unsigned int i = 0; i < local_ver_count; i++)
    {
      spdm_response->version_number_entry[i].major_version = __VERIFIER_nondet_uint();
      spdm_response->version_number_entry[i].minor_version = __VERIFIER_nondet_uint();
    }

    //spdm_response->version_number_entry_count = 2;
    //spdm_response->version_number_entry[0].major_version = 1;
    //spdm_response->version_number_entry[0].minor_version = 0;
    //spdm_response->version_number_entry[1].major_version = 5;
    //spdm_response->version_number_entry[1].minor_version = 5;

    // Saving a global version of spdm_response
    spdm_response_global.header.spdm_version = spdm_response->header.spdm_version;
    spdm_response_global.header.request_response_code = spdm_response->header.request_response_code;
    spdm_response_global.header.param1 = spdm_response->header.param1;
    spdm_response_global.header.param2 = spdm_response->header.param2;
    spdm_response_global.version_number_entry_count = spdm_response->version_number_entry_count;
    for(unsigned int i = 0; i < spdm_response->version_number_entry_count; i++)
    {
      spdm_response_global.version_number_entry[i].major_version = spdm_response->version_number_entry[i].major_version;
      spdm_response_global.version_number_entry[i].minor_version = spdm_response->version_number_entry[i].minor_version;
    }

    return_status status;
    return status;
}

boolean is_member(spdm_version_number_t *ver_set, uintn ver_num, spdm_version_number_t ver) {
  for (int i=0; i<ver_num; i++) {
    if(ver_set[i].major_version == ver.major_version &&
       ver_set[i].minor_version == ver.minor_version)
        return TRUE;
    //if (const_compare_mem(&ver_set[i], &ver, sizeof(spdm_version_number_t)) == 0)
    //  return TRUE;
  }
  return FALSE;
}

int main(void)
{
    // Creating a pointer to an spdm_context_t variable here;
    spdm_context_t *spdm_context = malloc(libspdm_get_context_size());

    // Initializing spdm_context_here
    libspdm_init_context(spdm_context);
    assert(spdm_context->connection_info.connection_state ==
        LIBSPDM_CONNECTION_STATE_NOT_STARTED);
    
    // Registering IO functions
    libspdm_register_device_io_func(spdm_context,
                      spdm_requester_get_version_test_send_message,
                      spdm_requester_get_version_test_receive_message);
    //assert(spdm_context->send_message == &spdm_requester_get_version_test_send_message);
    //assert(spdm_context->receive_message == &spdm_requester_get_version_test_receive_message);
    
    // Registering transport functions
    //libspdm_register_transport_layer_func(spdm_context,
    //                   spdm_transport_test_encode_message,
    //                   spdm_transport_test_decode_message);
    //assert(spdm_context->transport_encode_message == &spdm_transport_test_encode_message);
    //assert(spdm_context->transport_decode_message == &spdm_transport_test_decode_message);
 
    // Setting from 1 to 5 symbolically defined versions in the local context
    unsigned int local_ver_count = __VERIFIER_nondet_uint();
    __ESBMC_assume(local_ver_count > 0);
    __ESBMC_assume(local_ver_count <= LIBSPDM_MAX_VERSION_COUNT);
    spdm_context->local_context.version.spdm_version_count = local_ver_count;
    for(unsigned int i = 0; i < local_ver_count; i++)
    {
      spdm_context->local_context.version.spdm_version[i].major_version = __VERIFIER_nondet_uint();
      spdm_context->local_context.version.spdm_version[i].minor_version = __VERIFIER_nondet_uint();
    }
     
    // Calling try_spdm_get_version_method
    return_status status = try_spdm_get_version(spdm_context);

    // Defining local variables for the versions defined on the requester and the responder sides 
    spdm_version_number_t *req_ver_set = spdm_context->local_context.version.spdm_version;
    uint8_t                req_ver_num = spdm_context->local_context.version.spdm_version_count;
    spdm_version_number_t *res_ver_set = spdm_response_global.version_number_entry;
    uint8_t                res_ver_num = spdm_response_global.version_number_entry_count;

    // Assertions for the versions negotiated by the requester and responder in case of success
    if(status == RETURN_SUCCESS)
    {
      // The connection state must change to LIBSPDM_CONNECTION_STATE_AFTER_VERSION
      assert(spdm_context->connection_info.connection_state ==
          LIBSPDM_CONNECTION_STATE_AFTER_VERSION);
    
      // The SPDM version negotiated between the requester and the responder
      spdm_version_number_t ver = spdm_context->connection_info.version;

      // The negotiated version must be in both req_ver_set and res_ver_set
      assert(is_member(req_ver_set, req_ver_num, ver));
      assert(is_member(res_ver_set, res_ver_num, ver));
      
      // Any version in req_ver_set that is higher than the negotiated version must not in res_ver_set
      unsigned int i = __VERIFIER_nondet_uint();
      __ESBMC_assume(i < req_ver_num);
      if (spdm_version_number_compare(req_ver_set[i], ver))
        assert(!is_member(res_ver_set, res_ver_num, req_ver_set[i]));

      // Any version in res_ver_set that is higher than the negotiated version must not in req_ver_set
      unsigned int j = __VERIFIER_nondet_uint();
      __ESBMC_assume(j < res_ver_num);
      if (spdm_version_number_compare(res_ver_set[j], ver))
        assert(!is_member(req_ver_set, req_ver_num, res_ver_set[j]));
    }
    else
    {
      assert(status == (uint32_t)RETURN_DEVICE_ERROR);
      //assert( status == (uint32_t)RETURN_DEVICE_ERROR || 
      //        status == (uint32_t)RETURN_SECURITY_VIOLATION);
    }

    
    
    return 0;
}












