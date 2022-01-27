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

spdm_version_response_mine_t spdm_response;
return_status spdm_response_ret_status;

return_status spdm_requester_get_version_test_send_message(
    IN void *spdm_context, IN uintn request_size, IN void *request,
    IN uint64_t timeout)
{
    spdm_test_context_t *spdm_test_context;
    spdm_test_context = get_spdm_test_context();
    return RETURN_SUCCESS;
}

return_status spdm_requester_get_version_test_receive_message(
    IN void *spdm_context, IN OUT uintn *response_size,
    IN OUT void *response, IN uint64_t timeout)
{
    spdm_test_context_t *spdm_test_context;
    spdm_test_context = get_spdm_test_context();
    spdm_transport_test_encode_message(spdm_context, NULL, FALSE,
                        FALSE, sizeof(spdm_response),
                        &spdm_response,
                        response_size, response);

    //return spdm_response_ret_status;
    return RETURN_SUCCESS;
}

boolean is_member(spdm_version_number_t *ver_set, uintn ver_num, spdm_version_number_t ver) {
  for (int i=0; i<ver_num; i++) {
    if (const_compare_mem(&ver_set[i], &ver, sizeof(spdm_version_number_t)) == 0)
      return TRUE;
  }
  return FALSE;
}

void test_try_spdm_get_version() {
    return_status ret;
    spdm_test_context_t *spdm_test_context;
    spdm_context_t *context;

    spdm_test_context = get_spdm_test_context();
    context = spdm_test_context->spdm_context;
    spdm_test_context->case_id = 0x1;
    
    ret = try_spdm_get_version(context);
    // 1. check return code
    if RETURN_ERROR(ret)
        assert(ret == RETURN_DEVICE_ERROR);

    uintn spdm_response_size = sizeof(spdm_response);
    // 2. check error handling
    if (spdm_response_size < sizeof(spdm_message_header_t))
      assert(ret == RETURN_DEVICE_ERROR);

    if (spdm_response.header.request_response_code == SPDM_ERROR)
      assert(ret == RETURN_DEVICE_ERROR);

    // and so on

 

    // 3. check cache state

    // make sure context.transcript.message_a.buffer has some content

    // everything is fine
    assert(context->connection_info.connection_state == LIBSPDM_CONNECTION_STATE_AFTER_VERSION);
    assert(ret == RETURN_SUCCESS);

    // needs to be stronger, we know the exact number
    assert(context->transcript.message_a.buffer_size != 0);

    spdm_version_number_t ver = context->connection_info.version;
    spdm_version_number_t *req_ver_set = context->local_context.version.spdm_version;
    uint8_t                req_ver_num = context->local_context.version.spdm_version_count;
    spdm_version_number_t *res_ver_set = spdm_response.version_number_entry;
    uint8_t                res_ver_num = spdm_response.version_number_entry_count;

    // ver must be in both req_ver_set and res_ver_set
    assert(is_member(req_ver_set, req_ver_num, ver));
    assert(is_member(res_ver_set, res_ver_num, ver));

    // any version in req_ver_set is higher than ver must not in res_ver_set
    unsigned int i = nondet_uint();
    __ESBMC_assume(i < req_ver_num);
    if (spdm_version_number_compare(req_ver_set[i], ver))
      assert(!is_member(res_ver_set, res_ver_num, req_ver_set[i]));

    // any version in res_ver_set is higher than ver must not in req_ver_set
    unsigned int j = nondet_uint();
    __ESBMC_assume(j < res_ver_num);
    if (spdm_version_number_compare(res_ver_set[j], ver))
      assert(!is_member(req_ver_set, req_ver_num, res_ver_set[j]));
}

spdm_test_context_t mSpdmRequesterGetVersionTestContext = {
    SPDM_TEST_CONTEXT_SIGNATURE,
    TRUE,
    spdm_requester_get_version_test_send_message,
    spdm_requester_get_version_test_receive_message,
};

// taken from spdm_unit_test_group_setup
int test_setup()
{
    spdm_test_context_t *spdm_test_context;
    void *spdm_context;

    spdm_test_context = get_spdm_test_context();
    spdm_test_context->spdm_context =
        (void *)malloc(libspdm_get_context_size());
    if (spdm_test_context->spdm_context == NULL) {
        return -1;
    }
    spdm_context = spdm_test_context->spdm_context;
    spdm_test_context->case_id = 0xFFFFFFFF;

    libspdm_init_context(spdm_context);
    libspdm_register_device_io_func(spdm_context,
                     spdm_test_context->send_message,
                     spdm_test_context->receive_message);
    libspdm_register_transport_layer_func(spdm_context,
                       spdm_transport_test_encode_message,
                       spdm_transport_test_decode_message);

    setup_spdm_test_context(spdm_test_context);
    return 0;
}

// taken from spdm_unit_test_group_teardown
int test_teardown()
{
    spdm_test_context_t *spdm_test_context;
    spdm_test_context = get_spdm_test_context();
    free(spdm_test_context->spdm_context);
    spdm_test_context->spdm_context = NULL;
    spdm_test_context->case_id = 0xFFFFFFFF;
    return 0;
}

int main(void)
{
    setup_spdm_test_context(&mSpdmRequesterGetVersionTestContext);
    spdm_test_context_t *spdm_test_context = get_spdm_test_context();

    // Some trivial checks just to make sure that the initialisation worked.
    // Basically we make sure that the global test context has been set up (not NULL)
    // with the our "send" and "receive" message handlers, the spdm_context has not been set up
    assert(spdm_test_context != NULL);
    assert(&mSpdmRequesterGetVersionTestContext == spdm_test_context);
    assert(spdm_test_context->send_message == &spdm_requester_get_version_test_send_message);
    assert(spdm_test_context->receive_message == &spdm_requester_get_version_test_receive_message);
    assert(spdm_test_context->signature == SPDM_TEST_CONTEXT_SIGNATURE);    
    assert(spdm_test_context->spdm_context == NULL);
    
    test_setup();
    // Some trivial checks to make sure that the spdm_test_context and spdm_context were setup correctly
    spdm_context_t *spdm_context = spdm_test_context->spdm_context;
    assert(spdm_test_context->case_id == 0xFFFFFFFF);
    assert(spdm_context != NULL);
    assert(spdm_context->send_message == &spdm_requester_get_version_test_send_message);
    assert(spdm_context->receive_message == &spdm_requester_get_version_test_receive_message);
    assert(spdm_context->transport_encode_message == &spdm_transport_test_encode_message);
    assert(spdm_context->transport_decode_message == &spdm_transport_test_decode_message);

    //test_try_spdm_get_version();
    //assert(spdm_test_context->case_id == 0x1);

//    return_status status = spdm_get_version(spdm_context);
    // some checks after the spdm_get_version call
//    assert(spdm_context->retry_times == 3);
//    assert(spdm_context->connection_info.connection_state == LIBSPDM_CONNECTION_STATE_NOT_STARTED);

    test_teardown();
    // Some trivial checks to make sure that spdm_test_context->spdm_context was freed
    assert(spdm_test_context->case_id == 0xFFFFFFFF);
    assert(spdm_test_context->spdm_context == NULL);
    
    return 0;
}
