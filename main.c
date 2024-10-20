#include <stdio.h>
#include <stdlib.h>

#include "gptmalloc.h"

int main()
{
    int* ptr = (int*) malloc(5 * sizeof(int));
    int* gpt_ptr = (int*)malloc_with_ai("Enough memory to store up to five integers.");

    // test normal ptr
    printf("testing normal ptr...\n");
    if (ptr == NULL)
    {
        printf("normal pointer allocation failed\n");
        return 1;
    }

    for (int i = 0; i < 5; i++)
    {
        ptr[i] = i + 1;
    }

    for (int i = 0; i < 5; i++) {
        printf("%d ", ptr[i]);
    }
    printf("\n");

    // test gpt ptr
    printf("testing gpt ptr...\n");
    if (gpt_ptr == NULL)
    {
        printf("gpt pointer allocation failed\n");
        return 1;
    }

    for (int i = 0; i < 5; i++)
    {
        gpt_ptr[i] = i + 1;
    }

    for (int i = 0; i < 5; i++) {
        printf("%d ", gpt_ptr[i]);
    }
     printf("\n");


    free(ptr);
    free(gpt_ptr);

    return 0;
}
