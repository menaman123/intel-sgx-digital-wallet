#include <stdio.h>
#include <unistd.h>

#include "sgx_urts.h"
#include "Enclave_u.h"

// Enclave ID
sgx_enclave_id_t global_eid = 0;

/**
 * Initialize an SGX enclave.
 * @return error state
 */
int initialize_enclave(void) {
    sgx_status_t state = SGX_ERROR_UNEXPECTED;

    char enclavefile[256];
    getcwd(enclavefile, sizeof(enclavefile));
    strcat(enclavefile, "/enclave.signed.so");

    state = sgx_create_enclave(enclavefile, SGX_DEBUG_FLAG, NULL, NULL, &global_eid, NULL);

    if (state != SGX_SUCCESS) {
        printf("Can not create enclave, error-code: %d\n", state);
        return -1;
    }

    return 0;
}

void ocall_viewBalance(float balance){
    printf("Balance: %.2f\n", balance);
}

void ocall_checkTransaction(const struct transaction_entry* entries, size_t count) {
    printf("Transaction history: \n");
    for (size_t i = 0; i < count; i++) {
        printf("%s: %.2f\n", entries[i].description, entries[i].amount);
    }
}
/**
 * Main application.
 */
int SGX_CDECL main() {


    initialize_enclave();
    
    ecall_viewBalance(global_eid);
    ecall_deposit(global_eid, 500.0f);
    ecall_pay(global_eid, 200.0f);
    ecall_viewBalance(global_eid);
    ecall_checkTransaction(global_eid);
    
    sgx_destroy_enclave(global_eid);
    return 0;


}
