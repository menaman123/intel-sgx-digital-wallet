#include "Enclave_t.h"

#include "sgx_trts.h" /* for sgx_ocalloc, sgx_is_outside_enclave */
#include "sgx_lfence.h" /* for sgx_lfence */

#include <errno.h>
#include <mbusafecrt.h> /* for memcpy_s etc */
#include <stdlib.h> /* for malloc/free etc */

#define CHECK_REF_POINTER(ptr, siz) do {	\
	if (!(ptr) || ! sgx_is_outside_enclave((ptr), (siz)))	\
		return SGX_ERROR_INVALID_PARAMETER;\
} while (0)

#define CHECK_UNIQUE_POINTER(ptr, siz) do {	\
	if ((ptr) && ! sgx_is_outside_enclave((ptr), (siz)))	\
		return SGX_ERROR_INVALID_PARAMETER;\
} while (0)

#define CHECK_ENCLAVE_POINTER(ptr, siz) do {	\
	if ((ptr) && ! sgx_is_within_enclave((ptr), (siz)))	\
		return SGX_ERROR_INVALID_PARAMETER;\
} while (0)

#define ADD_ASSIGN_OVERFLOW(a, b) (	\
	((a) += (b)) < (b)	\
)


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

static sgx_status_t SGX_CDECL sgx_ecall_deposit(void* pms)
{
	CHECK_REF_POINTER(pms, sizeof(ms_ecall_deposit_t));
	//
	// fence after pointer checks
	//
	sgx_lfence();
	ms_ecall_deposit_t* ms = SGX_CAST(ms_ecall_deposit_t*, pms);
	ms_ecall_deposit_t __in_ms;
	if (memcpy_s(&__in_ms, sizeof(ms_ecall_deposit_t), ms, sizeof(ms_ecall_deposit_t))) {
		return SGX_ERROR_UNEXPECTED;
	}
	sgx_status_t status = SGX_SUCCESS;


	ecall_deposit(__in_ms.ms_amount);


	return status;
}

static sgx_status_t SGX_CDECL sgx_ecall_pay(void* pms)
{
	CHECK_REF_POINTER(pms, sizeof(ms_ecall_pay_t));
	//
	// fence after pointer checks
	//
	sgx_lfence();
	ms_ecall_pay_t* ms = SGX_CAST(ms_ecall_pay_t*, pms);
	ms_ecall_pay_t __in_ms;
	if (memcpy_s(&__in_ms, sizeof(ms_ecall_pay_t), ms, sizeof(ms_ecall_pay_t))) {
		return SGX_ERROR_UNEXPECTED;
	}
	sgx_status_t status = SGX_SUCCESS;


	ecall_pay(__in_ms.ms_amount);


	return status;
}

static sgx_status_t SGX_CDECL sgx_ecall_viewBalance(void* pms)
{
	sgx_status_t status = SGX_SUCCESS;
	if (pms != NULL) return SGX_ERROR_INVALID_PARAMETER;
	ecall_viewBalance();
	return status;
}

static sgx_status_t SGX_CDECL sgx_ecall_checkTransaction(void* pms)
{
	sgx_status_t status = SGX_SUCCESS;
	if (pms != NULL) return SGX_ERROR_INVALID_PARAMETER;
	ecall_checkTransaction();
	return status;
}

SGX_EXTERNC const struct {
	size_t nr_ecall;
	struct {void* ecall_addr; uint8_t is_priv; uint8_t is_switchless;} ecall_table[4];
} g_ecall_table = {
	4,
	{
		{(void*)(uintptr_t)sgx_ecall_deposit, 0, 0},
		{(void*)(uintptr_t)sgx_ecall_pay, 0, 0},
		{(void*)(uintptr_t)sgx_ecall_viewBalance, 0, 0},
		{(void*)(uintptr_t)sgx_ecall_checkTransaction, 0, 0},
	}
};

SGX_EXTERNC const struct {
	size_t nr_ocall;
	uint8_t entry_table[2][4];
} g_dyn_entry_table = {
	2,
	{
		{0, 0, 0, 0, },
		{0, 0, 0, 0, },
	}
};


sgx_status_t SGX_CDECL ocall_viewBalance(float balance)
{
	sgx_status_t status = SGX_SUCCESS;

	ms_ocall_viewBalance_t* ms = NULL;
	size_t ocalloc_size = sizeof(ms_ocall_viewBalance_t);
	void *__tmp = NULL;


	__tmp = sgx_ocalloc(ocalloc_size);
	if (__tmp == NULL) {
		sgx_ocfree();
		return SGX_ERROR_UNEXPECTED;
	}
	ms = (ms_ocall_viewBalance_t*)__tmp;
	__tmp = (void *)((size_t)__tmp + sizeof(ms_ocall_viewBalance_t));
	ocalloc_size -= sizeof(ms_ocall_viewBalance_t);

	if (memcpy_verw_s(&ms->ms_balance, sizeof(ms->ms_balance), &balance, sizeof(balance))) {
		sgx_ocfree();
		return SGX_ERROR_UNEXPECTED;
	}

	status = sgx_ocall(0, ms);

	if (status == SGX_SUCCESS) {
	}
	sgx_ocfree();
	return status;
}

sgx_status_t SGX_CDECL ocall_checkTransaction(const struct transaction_entry* entries, size_t count)
{
	sgx_status_t status = SGX_SUCCESS;
	size_t _len_entries = count * sizeof(struct transaction_entry);

	ms_ocall_checkTransaction_t* ms = NULL;
	size_t ocalloc_size = sizeof(ms_ocall_checkTransaction_t);
	void *__tmp = NULL;


	CHECK_ENCLAVE_POINTER(entries, _len_entries);

	if (ADD_ASSIGN_OVERFLOW(ocalloc_size, (entries != NULL) ? _len_entries : 0))
		return SGX_ERROR_INVALID_PARAMETER;

	__tmp = sgx_ocalloc(ocalloc_size);
	if (__tmp == NULL) {
		sgx_ocfree();
		return SGX_ERROR_UNEXPECTED;
	}
	ms = (ms_ocall_checkTransaction_t*)__tmp;
	__tmp = (void *)((size_t)__tmp + sizeof(ms_ocall_checkTransaction_t));
	ocalloc_size -= sizeof(ms_ocall_checkTransaction_t);

	if (entries != NULL) {
		if (memcpy_verw_s(&ms->ms_entries, sizeof(const struct transaction_entry*), &__tmp, sizeof(const struct transaction_entry*))) {
			sgx_ocfree();
			return SGX_ERROR_UNEXPECTED;
		}
		if (memcpy_verw_s(__tmp, ocalloc_size, entries, _len_entries)) {
			sgx_ocfree();
			return SGX_ERROR_UNEXPECTED;
		}
		__tmp = (void *)((size_t)__tmp + _len_entries);
		ocalloc_size -= _len_entries;
	} else {
		ms->ms_entries = NULL;
	}

	if (memcpy_verw_s(&ms->ms_count, sizeof(ms->ms_count), &count, sizeof(count))) {
		sgx_ocfree();
		return SGX_ERROR_UNEXPECTED;
	}

	status = sgx_ocall(1, ms);

	if (status == SGX_SUCCESS) {
	}
	sgx_ocfree();
	return status;
}

