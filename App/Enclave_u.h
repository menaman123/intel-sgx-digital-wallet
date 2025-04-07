#ifndef ENCLAVE_U_H__
#define ENCLAVE_U_H__

#include <stdint.h>
#include <wchar.h>
#include <stddef.h>
#include <string.h>
#include "sgx_edger8r.h" /* for sgx_status_t etc. */


#include <stdlib.h> /* for size_t */

#define SGX_CAST(type, item) ((type)(item))

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _transaction_entry
#define _transaction_entry
typedef struct transaction_entry {
	char description[32];
	float amount;
} transaction_entry;
#endif

#ifndef OCALL_VIEWBALANCE_DEFINED__
#define OCALL_VIEWBALANCE_DEFINED__
void SGX_UBRIDGE(SGX_NOCONVENTION, ocall_viewBalance, (float balance));
#endif
#ifndef OCALL_CHECKTRANSACTION_DEFINED__
#define OCALL_CHECKTRANSACTION_DEFINED__
void SGX_UBRIDGE(SGX_NOCONVENTION, ocall_checkTransaction, (const struct transaction_entry* entries, size_t count));
#endif

sgx_status_t ecall_deposit(sgx_enclave_id_t eid, float amount);
sgx_status_t ecall_pay(sgx_enclave_id_t eid, float amount);
sgx_status_t ecall_viewBalance(sgx_enclave_id_t eid);
sgx_status_t ecall_checkTransaction(sgx_enclave_id_t eid);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
