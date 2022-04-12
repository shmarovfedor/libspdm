/**
    Copyright Notice:
    Copyright 2021 DMTF. All rights reserved.
    License: BSD 3-Clause License. For full text see link: https://github.com/DMTF/libspdm/blob/main/LICENSE.md
**/
//#define libspdm_append_message_a(a,b,c) libspdm_append_message_a_empty(a,b,c)

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

    return_status status;
    return status;
}

return_status spdm_requester_get_version_test_receive_message(
    IN void *spdm_context, IN OUT uintn *response_size,
    IN OUT void *response, IN uint64_t timeout)
{
    spdm_version_response_mine_t *spdm_response = (spdm_version_response_mine_t *) response;

    // TODO: symbolize the next 4 lines
    // __ESBMC_init_var(spdm_response->header);

    spdm_response->header.spdm_version = SPDM_MESSAGE_VERSION_10;
    spdm_response->header.request_response_code = SPDM_VERSION;
    spdm_response->header.param1 = 0;
    spdm_response->header.param2 = 0;

    // setting from 1 to LIBSPDM_MAX_VERSION_COUNT symbolically defined versions on the responder side
    unsigned int local_ver_count = __VERIFIER_nondet_uint();
    __ESBMC_assume(local_ver_count > 0);
    __ESBMC_assume(local_ver_count <= LIBSPDM_MAX_VERSION_COUNT);
    spdm_response->version_number_entry_count = local_ver_count;
    for(unsigned int i = 0; i < local_ver_count; i++)
    {
      spdm_response->version_number_entry[i].major_version = __VERIFIER_nondet_uint();
      spdm_response->version_number_entry[i].minor_version = __VERIFIER_nondet_uint();
    }

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
  }
  return FALSE;
}


void test_resetting()
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

    // Setting a pointer at the beginning of message_a inside the spdm_context
    uint8_t *ptr = &spdm_context->transcript.message_a.buffer;

    // Resetting message_a 
    libspdm_reset_message_a(spdm_context);

    // Every byte inside message_a must be set to 0
    {
      for(unsigned int i = 0; i < spdm_context->transcript.message_a.max_buffer_size; i++)
        assert(*(ptr++) == 0);
    }
}


void test_caching() {
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

    // Set a message size to some value in the range [0, LIBSPDM_MAX_MESSAGE_SMALL_BUFFER_SIZE + 1],
    // where LIBSPDM_MAX_MESSAGE_SMALL_BUFFER_SIZE is the size of message_a buffer in spdm_context
    uintn message_size = __VERIFIER_nondet_uint();
    __ESBMC_assume(message_size >= 0);
    __ESBMC_assume(message_size <= LIBSPDM_MAX_MESSAGE_SMALL_BUFFER_SIZE + 1);
    
    // Create a symbolic message of size message_size
    uint8_t *message = malloc(message_size);
    
    // Saving the size of message_a cache before appending a new message
    uintn buffer_size_before = spdm_context->transcript.message_a.buffer_size;
    // Copying the content of message_a cache before appending a new message
    uint8_t *old_message_a = malloc(buffer_size_before);
    {
      uint8_t *ptr = &spdm_context->transcript.message_a.buffer;
      for(unsigned int i = 0; i < buffer_size_before; i++)
        old_message_a[i] = *(ptr++);
    }

    // Set a pointer to the free part of message_a  
    //uint8_t *ptr = &spdm_context->transcript.message_a.buffer + 
    //                spdm_context->transcript.message_a.buffer_size;
   
    // Appending the message to message_a inside the spdm_context 
    return_status status = libspdm_append_message_a(spdm_context, message, message_size);
    
    uint8_t *ptr = &spdm_context->transcript.message_a.buffer;
    // If the appending is successfull ...
    if(status == RETURN_SUCCESS)
    {
      // ... then the appending should not affect anything that was in the message_a buffer
      // before the appended message and ...
      for(unsigned int i = 0; i < buffer_size_before; i++)
        assert(*(ptr++) == old_message_a[i]);

      uint8_t *ptr_2 = message;
      // ... all the bytes of the message must  have been appended to the message_a buffer and ...
      for(unsigned int i = 0; i < message_size; i++)
        assert(*(ptr++) == *(ptr_2++));
      
      // ... everything else past the appended message in the buffer must be 0
      for(unsigned int i = 0; i < spdm_context->transcript.message_a.max_buffer_size - 
                                    spdm_context->transcript.message_a.buffer_size; i++)
        assert(*(ptr++) == 0);
    }
    // If the appending failed ...
    else
    {
      // ... then the appending should not affect anything that was in the message_a buffer
      // before the appended message and ...
      for(unsigned int i = 0; i < buffer_size_before; i++)
        assert(*(ptr++) == old_message_a[i]);
      
      // ... the size of the message_a content should remain unchanged and ...
      assert(buffer_size_before == spdm_context->transcript.message_a.buffer_size);

      // ... nothing else should be appended (everything else past the point of 
      // attempted appendage must be 0)
      for(unsigned int i = 0; i < spdm_context->transcript.message_a.max_buffer_size - 
                                    spdm_context->transcript.message_a.buffer_size; i++)
        assert(*(ptr++) == 0);
    }

    return;
}


void test_negotiate_connection_version() {
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
    if(status == RETURN_SUCCESS) {
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
    else {
      assert(status == (uint32_t)RETURN_DEVICE_ERROR);
      //assert( status == (uint32_t)RETURN_DEVICE_ERROR ||
      //        status == (uint32_t)RETURN_SECURITY_VIOLATION);
    }
}

void test_spdm_version_number_sort() {
  const int ver_num = LIBSPDM_MAX_VERSION_COUNT;
  spdm_version_number_t ver_set[ver_num];
  spdm_version_number_sort(ver_set, ver_num);

  unsigned int i = nondet_uint();
  __ESBMC_assume(i < ver_num - 1);
  assert(!spdm_version_number_compare(ver_set[i+1], ver_set[i]));
}

int main(void)
{
  test_resetting();
  test_caching();
  test_negotiate_connection_version();
  return 0;
}
