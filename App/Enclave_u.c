#include "Enclave_u.h"
#include <errno.h>

typedef struct ms_ecall_deposit_t {
	float ms_amount;
} ms_ecall_deposit_t;

typedef struct ms_ecall_pay_t {
	float ms_amount;
} ms_ecall_pay_t;

typedef struct ms_ocall_viewBalance_t {
	float ms_balance;
} ms_ocall_viewBalance_t;

typedef struct ms_ocall_checkTransaction_t {
	const struct transaction_entry* ms_entries;
	size_t ms_count;
} ms_ocall_checkTransaction_t;

static sgx_status_t SGX_CDECL Enclave_ocall_viewBalance(void* pms)
{
	ms_ocall_viewBalance_t* ms = SGX_CAST(ms_ocall_viewBalance_t*, pms);
	ocall_viewBalance(ms->ms_balance);

	return SGX_SUCCESS;
}

static sgx_status_t SGX_CDECL Enclave_ocall_checkTransaction(void* pms)
{
	ms_ocall_checkTransaction_t* ms = SGX_CAST(ms_ocall_checkTransaction_t*, pms);
	ocall_checkTransaction(ms->ms_entries, ms->ms_count);

	return SGX_SUCCESS;
}

static const struct {
	size_t nr_ocall;
	void * table[2];
} ocall_table_Enclave = {
	2,
	{
		(void*)Enclave_ocall_viewBalance,
		(void*)Enclave_ocall_checkTransaction,
	}
};
sgx_status_t ecall_deposit(sgx_enclave_id_t eid, float amount)
{
	sgx_status_t status;
	ms_ecall_deposit_t ms;
	ms.ms_amount = amount;
	status = sgx_ecall(eid, 0, &ocall_table_Enclave, &ms);
	return status;
}

sgx_status_t ecall_pay(sgx_enclave_id_t eid, float amount)
{
	sgx_status_t status;
	ms_ecall_pay_t ms;
	ms.ms_amount = amount;
	status = sgx_ecall(eid, 1, &ocall_table_Enclave, &ms);
	return status;
}

sgx_status_t ecall_viewBalance(sgx_enclave_id_t eid)
{
	sgx_status_t status;
	status = sgx_ecall(eid, 2, &ocall_table_Enclave, NULL);
	return status;
}

sgx_status_t ecall_checkTransaction(sgx_enclave_id_t eid)
{
	sgx_status_t status;
	status = sgx_ecall(eid, 3, &ocall_table_Enclave, NULL);
	return status;
}

