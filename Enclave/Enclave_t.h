#ifndef ENCLAVE_T_H__
#define ENCLAVE_T_H__

#include <stdint.h>
#include <wchar.h>
#include <stddef.h>
#include "sgx_edger8r.h" /* for sgx_ocall etc. */


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

void ecall_deposit(float amount);
void ecall_pay(float amount);
void ecall_viewBalance(void);
void ecall_checkTransaction(void);

sgx_status_t SGX_CDECL ocall_viewBalance(float balance);
sgx_status_t SGX_CDECL ocall_checkTransaction(const struct transaction_entry* entries, size_t count);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
