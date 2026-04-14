#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../include/transaction.h"
#include "../include/bank.h"

Transaction parseTransaction(char *input)
{
    Transaction t;
    memset(&t, 0, sizeof(Transaction));

    char *token = strtok(input, " \n");
    if (!token)
        return t;

    // Transaction ID
    if (token[0] == 'T')
        t.tx_id = atoi(token + 1);
    else
        t.tx_id = atoi(token);

    // Start tick
    token = strtok(NULL, " \n");
    if (!token)
        return t;
    t.start_tick = atoi(token);

    // Operation type
    token = strtok(NULL, " \n");
    if (!token)
        return t;

    Operation *op = &t.ops[0];

    if (strcmp(token, "DEPOSIT") == 0)
    {
        op->type = OP_DEPOSIT;

        token = strtok(NULL, " \n");
        if (!token)
            return t;
        op->account_id = atoi(token);

        token = strtok(NULL, " \n");
        if (!token)
            return t;
        op->amount_centavos = atoi(token);

        t.num_ops = 1;
    }
    else if (strcmp(token, "WITHDRAW") == 0)
    {
        op->type = OP_WITHDRAW;

        token = strtok(NULL, " \n");
        if (!token)
            return t;
        op->account_id = atoi(token);

        token = strtok(NULL, " \n");
        if (!token)
            return t;
        op->amount_centavos = atoi(token);

        t.num_ops = 1;
    }
    else if (strcmp(token, "TRANSFER") == 0)
    {
        op->type = OP_TRANSFER;

        token = strtok(NULL, " \n");
        if (!token)
            return t;
        op->account_id = atoi(token);

        token = strtok(NULL, " \n");
        if (!token)
            return t;
        op->target_account = atoi(token);

        token = strtok(NULL, " \n");
        if (!token)
            return t;
        op->amount_centavos = atoi(token);

        t.num_ops = 1;
    }
    else if (strcmp(token, "BALANCE") == 0)
    {
        op->type = OP_BALANCE;

        token = strtok(NULL, " \n");
        if (!token)
            return t;
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

Account parseAccount(char *input)
{
    Account acc;
    memset(&acc, 0, sizeof(Account));

    // Get Account ID
    char *token = strtok(input, " \n");
    if (!token)
        return acc;
    acc.account_id = atoi(token);

    // Get Balance
    token = strtok(NULL, " \n"); // NULL tells strtok to continue from last spot
    if (!token)
        return acc;
    acc.balance_centavos = atoi(token);

    // Initialize the lock for this specific account
    pthread_rwlock_init(&acc.lock, NULL);

    return acc;
}

Transaction *loadInput(const char *filename,
                       int *tx_count,
                       Account **out_accounts,
                       int *acc_count)
{
    int tx_capacity = 10, acc_capacity = 10;
    int t_count = 0, a_count = 0;

    Transaction *transactions = malloc(tx_capacity * sizeof(Transaction));
    Account *accounts = malloc(acc_capacity * sizeof(Account));

    if (!transactions || !accounts)
    {
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }

    FILE *file = fopen(filename, "r");
    if (!file)
    {
        perror(filename);
        free(transactions);
        free(accounts);
        return NULL;
    }

    char line[256];

    while (fgets(line, sizeof(line), file))
    {
        if (line[0] == '\n' || line[0] == '#')
            continue;

        // Make a copy because strtok modifies the string
        char line_copy[256];
        strcpy(line_copy, line);

        char *token = strtok(line_copy, " \n");
        if (!token)
            continue;

        // Detect type
        if (token[0] == 'T')
        {
            // TRANSACTION
            if (t_count >= tx_capacity)
            {
                tx_capacity *= 2;
                transactions = realloc(transactions, tx_capacity * sizeof(Transaction));
            }

            transactions[t_count++] = parseTransaction(line);
        }
        else
        {
            // ACCOUNT
            Account acc = parseAccount(line);
            if (acc.account_id >= 0 && acc.account_id < MAX_ACCOUNTS)
            {
                bank.accounts[acc.account_id] = acc;
                bank.num_accounts++;
            }
            accounts[a_count++] = acc; // still keeping your local list if needed
        }

        fclose(file);

        *tx_count = t_count;
        *acc_count = a_count;
        *out_accounts = accounts;

        return transactions;
    }
}
