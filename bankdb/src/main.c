#include <stdio.h>
#include <stdlib.h>
#include "../include/transaction.h"

// Declare your function (if not in a header yet)
Transaction* loadInput(const char *filename, int *out_count);

int main(int argc, char *argv[])
{
    int global_tick = 0;

    // 1. Check for input file
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
        return 1;
    }

    // 2. Load transactions
    int tx_count = 0;
    Transaction *transactions = loadInput(argv[1], &tx_count);

    if (!transactions) {
        fprintf(stderr, "Failed to load transactions\n");
        return 1;
    }

    // 3. Debug print (VERY IMPORTANT for validation)
    printf("Loaded %d transactions:\n\n", tx_count);

    for (int i = 0; i < tx_count; i++) {
        Transaction *t = &transactions[i];

        printf("T%d | start_tick=%d | ops=%d\n",
               t->tx_id, t->start_tick, t->num_ops);

        for (int j = 0; j < t->num_ops; j++) {
            Operation *op = &t->ops[j];

            switch (op->type) {
                case OP_DEPOSIT:
                    printf("  DEPOSIT acc=%d amt=%d\n",
                           op->account_id, op->amount_centavos);
                    break;

                case OP_WITHDRAW:
                    printf("  WITHDRAW acc=%d amt=%d\n",
                           op->account_id, op->amount_centavos);
                    break;

                case OP_TRANSFER:
                    printf("  TRANSFER %d -> %d amt=%d\n",
                           op->account_id,
                           op->target_account,
                           op->amount_centavos);
                    break;

                case OP_BALANCE:
                    printf("  BALANCE acc=%d\n",
                           op->account_id);
                    break;
            }
        }

        printf("\n");
    }

    // 4. Cleanup
    free(transactions);

    return 0;
}