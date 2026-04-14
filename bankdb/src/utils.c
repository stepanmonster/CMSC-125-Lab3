#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../include/transaction.h"

Transaction parseTransaction(char *input)
{
    Transaction t;
    memset(&t, 0, sizeof(Transaction));

    char *token = strtok(input, " \n");
    if (!token) return t;

    // Transaction ID
    if (token[0] == 'T')
        t.tx_id = atoi(token + 1);
    else
        t.tx_id = atoi(token);

    // Start tick
    token = strtok(NULL, " \n");
    if (!token) return t;
    t.start_tick = atoi(token);

    // Operation type
    token = strtok(NULL, " \n");
    if (!token) return t;

    Operation *op = &t.ops[0];

    if (strcmp(token, "DEPOSIT") == 0)
    {
        op->type = OP_DEPOSIT;

        token = strtok(NULL, " \n");
        if (!token) return t;
        op->account_id = atoi(token);

        token = strtok(NULL, " \n");
        if (!token) return t;
        op->amount_centavos = atoi(token);

        t.num_ops = 1;
    }
    else if (strcmp(token, "WITHDRAW") == 0)
    {
        op->type = OP_WITHDRAW;

        token = strtok(NULL, " \n");
        if (!token) return t;
        op->account_id = atoi(token);

        token = strtok(NULL, " \n");
        if (!token) return t;
        op->amount_centavos = atoi(token);

        t.num_ops = 1;
    }
    else if (strcmp(token, "TRANSFER") == 0)
    {
        op->type = OP_TRANSFER;

        token = strtok(NULL, " \n");
        if (!token) return t;
        op->account_id = atoi(token);

        token = strtok(NULL, " \n");
        if (!token) return t;
        op->target_account = atoi(token);

        token = strtok(NULL, " \n");
        if (!token) return t;
        op->amount_centavos = atoi(token);

        t.num_ops = 1;
    }
    else if (strcmp(token, "BALANCE") == 0)
    {
        op->type = OP_BALANCE;

        token = strtok(NULL, " \n");
        if (!token) return t;
        op->account_id = atoi(token);

        t.num_ops = 1;
    }
    else
    {
        fprintf(stderr, "Unknown operation: %s\n", token);
        return t;
    }

    t.status = TX_RUNNING;
    return t;
}

Transaction* loadInput(const char *filename, int *out_count)
{
    int capacity = 10;
    int count = 0;
    Transaction *transactions = malloc(capacity * sizeof(Transaction));
    if (!transactions) {
        fprintf(stderr, "Error: out of memory in loadtransactions\n");
        return NULL;
    }

    FILE *file = fopen(filename, "r");
    if (!file) {
        perror(filename);
        free(transactions);
        return NULL;
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        if (line[0] == '\n' || line[0] == '#') continue;

        if (count >= capacity) {
            capacity *= 2;
            Transaction *tmp = realloc(transactions, capacity * sizeof(Transaction));
            if (!tmp) {
                fprintf(stderr, "Error: out of memory in loadtransactions\n");
                fclose(file);
                free(transactions);
                return NULL;
            }
            transactions = tmp;
        }

        transactions[count++] = parseTransaction(line);
    }

    fclose(file);
    *out_count = count;
    return transactions;
}

