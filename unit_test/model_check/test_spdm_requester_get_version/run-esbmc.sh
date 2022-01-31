LIBSPDM="$(cd "$( dirname "${BASH_SOURCE[0]}" )"/../../.. &> /dev/null && pwd)"

$HOME/esbmc/release/bin/esbmc \
 --force-malloc-success \
 --goto-unwind \
 --k-step 10 --unlimited-k-steps \
 --k-induction --max-inductive-step 3 \
 $LIBSPDM/unit_test/model_check/test_spdm_requester_get_version/get_version.c \
 $LIBSPDM/unit_test/spdm_unit_test_common/common.c \
 $LIBSPDM/unit_test/spdm_transport_test_lib/common.c \
 $LIBSPDM/unit_test/spdm_transport_test_lib/test.c \
 $LIBSPDM/library/spdm_requester_lib/libspdm_req_get_version.c \
 $LIBSPDM/library/spdm_common_lib/libspdm_com_context_data.c \
 $LIBSPDM/library/spdm_common_lib/libspdm_com_context_data_session.c \
 $LIBSPDM/library/spdm_secured_message_lib/libspdm_secmes_context_data.c \
 $LIBSPDM/library/spdm_secured_message_lib/libspdm_secmes_encode_decode.c \
 $LIBSPDM/library/spdm_crypt_lib/libspdm_crypt_crypt.c \
 $LIBSPDM/library/spdm_requester_lib/libspdm_req_send_receive.c \
 $LIBSPDM/library/spdm_requester_lib/libspdm_req_handle_error_response.c \
 $LIBSPDM/library/spdm_common_lib/libspdm_com_support.c \
 $LIBSPDM/os_stub/memlib/copy_mem.c \
 $LIBSPDM/os_stub/memlib/zero_mem.c \
 $LIBSPDM/os_stub/memlib/compare_mem.c \
 $LIBSPDM/os_stub/debuglib_null/debuglib.c \
 $LIBSPDM/os_stub/cryptlib_null/rand/rand.c \
 $LIBSPDM/os_stub/cryptlib_null/hash/sha.c \
 -I$LIBSPDM/unit_test/spdm_unit_test_common/ \
 -I$LIBSPDM/unit_test/include/ \
 -I$LIBSPDM/include/ \
 -I$LIBSPDM/include/hal/ \
 -I$LIBSPDM/include/hal/x64/ \
 -I$LIBSPDM/include/internal/ \
 -I$LIBSPDM/include/library/ \
 -I$LIBSPDM/os_stub/spdm_device_secret_lib_sample/ \
 -I$LIBSPDM/os_stub/cryptlib_null/ \
 -I$LIBSPDM/os_stub/include/ \
 -I$LIBSPDM/os_stub/mbedtlslib/include \
 -I/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/usr/include/ \

