## gptmalloc.c

Ai powered memory management.

Set your GPT API Key as an enviroment variable
```
LLM_API_KEY=
```

```c
#include <stdio.h>
#include <stdlib.h>

#include "gptmalloc.h"

int main()
{
    int* gpt_ptr = (int*)malloc_with_ai("Enough memory to store up to five integers.");
    
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

    for (int i = 0; i < 5; i++)
    {
        printf("%d ", gpt_ptr[i]);
    }
    printf("\n");

    free(gpt_ptr);
    return 0;
}
```
