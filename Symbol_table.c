#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TABLE_SIZE 31
#define MAX_NAME_LENGTH 20

typedef struct symbol
{
    char name[MAX_NAME_LENGTH];
    int value;
    struct symbol *next;
} symbol;

typedef struct symbol_table
{
    symbol *table[TABLE_SIZE];
    struct symbol_table *prev;
} symbol_table;

symbol_table *curr = NULL;

int hash(const char *name)
{
    int hash_value = 0;
    for (int i = 0; name[i] != '\0'; i++)
    {
        hash_value += name[i];
    }
    return (hash_value % TABLE_SIZE);
}

void create_symbol(char *name, int val, symbol_table *curr)
{
    symbol *new, *nptr;
    int index;
    index = hash(name);
    if (curr->table[index] == NULL)
    {

        new = (symbol *)malloc(sizeof(symbol));
        strcpy(new->name, name);
        new->value = val;
        new->next = NULL;
        curr->table[index] = new;
    }
    else
    {
        int flag = 0;
        nptr = curr->table[index];
        if (strcmp(nptr->name, name) == 0)
        {
            nptr->value = val;
            flag = 1;
        }
        while (nptr->next != NULL && flag == 0)
        {
            if (strcmp(nptr->name, name) == 0)
            {
                nptr->value = val;
                flag = 1;
            }
            nptr = nptr->next;
        }
        if (nptr->next == NULL && flag == 0)
        {
            new = (symbol *)malloc(sizeof(symbol));
            strcpy(new->name, name);
            new->value = val;
            new->next = NULL;
            nptr->next = new;
        }
    }
}

void lookup_symbol(const char *name, symbol_table *curr)
{
    if (curr == NULL)
    {
        printf("ERROR: %s is not assigned\n", name);
        return;
    }

    int index = hash(name);
    symbol *nptr = curr->table[index];
    while (nptr != NULL)
    {
        if (strcmp(name, nptr->name) == 0)
        {
            printf("%s = %d\n", name, nptr->value);
            return;
        }
        nptr = nptr->next;
    }

    lookup_symbol(name, curr->prev);
}

symbol_table *begin_scope(symbol_table *prev)
{
    int i;
    symbol_table *new_scope = (symbol_table *)malloc(sizeof(symbol_table));
    if (new_scope != NULL)
    {
        new_scope->prev = prev;
    }
    for (i = 0; i < TABLE_SIZE; i++)
    {
        new_scope->table[i] = NULL;
    }
    return new_scope;
}

symbol_table *end_scope(symbol_table *curr)
{
    symbol_table *retval;
    if (curr != NULL)
    {
        retval = curr->prev;
        int i = TABLE_SIZE - 1;
        symbol *nptr, *temp;

        while (i >= 0)
        {
            nptr = curr->table[i];
            while (nptr != NULL)
            {
                temp = nptr;
                nptr = nptr->next;
                free(temp);
            }
            i--;
        }
        free(curr);
    }
    else
    {
        retval = NULL;
        printf("ERROR: corresponding 'begin' is not found \n");
    }
    return retval;
}


int main()
{
    FILE *fptr;
    fptr = fopen("code.txt", "r");
    if (fptr == NULL)
    {
        printf("Error: File not opened\n");
        return 1;
    }

    char operation[10], name[MAX_NAME_LENGTH];
    int val;
    symbol_table *curr = NULL;
    fscanf(fptr, "%s", operation);
    if (strcmp(operation, "begin") == 0)
    {
        curr = begin_scope(curr);

        while (!feof(fptr))
        {
            fscanf(fptr, "%s", operation);
            if (strcmp(operation, "begin") == 0)
            {
                curr = begin_scope(curr);
            }
            else if (strcmp(operation, "end") == 0)
            {
                curr = end_scope(curr);
            }
            else if (strcmp(operation, "assign") == 0)
            {
                fscanf(fptr, "%s %d", name, &val);
                create_symbol(name, val, curr);
            }
            else if (strcmp(operation, "print") == 0)
            {
                fscanf(fptr, "%s", name);
                lookup_symbol(name, curr);
            }
            else
            {
                printf("ERROR: Unrecognized command '%s'\n", operation);
            }
        }
        
    fclose(fptr);
    }
    return 0;
}
