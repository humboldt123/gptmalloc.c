## gptmalloc.c

AI powered memory management.

## Example

```c
#include <stdio.h>
#include <stdlib.h>

#include "gptmalloc.h"

int main()
{
    // allocate your memory using natural language 👇
    int* gpt_ptr = (int*)malloc_with_ai("Enough memory to store up to five integers.");

    for (int i = 0; i < 5; i++)
    {
        gpt_ptr[i] = i + 1;
    }

    // returns a pointer just like malloc(), make sure to free it when you're done 😉
    free(gpt_ptr);
    return 0;
}
```

## Setup
Set your OpenAI API Key as an enviroment variable
```
export LLM_API_KEY=sk_openai_api_key_goes_here;
```

## Endorsed by [ElanMakesGames](https://github.com/Elan-Rubin)
<img width="758" alt="image" src="https://github.com/user-attachments/assets/e6d64ce5-a00f-4f44-a566-2e0ce9fc913f">

