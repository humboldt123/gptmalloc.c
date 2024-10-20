#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include "gptmalloc.h"

struct MemoryStruct
{
	char *memory;
	size_t size;
};

static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
	size_t realsize = size * nmemb;
	struct MemoryStruct *mem = (struct MemoryStruct *)userp;

	char *ptr = realloc(mem->memory, mem->size + realsize + 1);
	if(!ptr)
	{
		printf("not enough memory (realloc returned NULL)\n");
		return 0;
	}

	mem->memory = ptr;
	memcpy(&(mem->memory[mem->size]), contents, realsize);
	mem->size += realsize;
	mem->memory[mem->size] = 0;

	return realsize;
}

char* strip_content_from_json(const char* json)
{
	const char* content_key = "\"content\": \"";
	char* content_start = strstr(json, content_key);
	if (content_start == NULL)
	{
		return NULL;
	}
	content_start += strlen(content_key);

	char* content_end = strchr(content_start, '"');
	if (content_end == NULL)
	{
		return NULL;
	}

	size_t content_length = content_end - content_start;
	char* content = malloc(content_length + 1);
	if (content == NULL)
	{
		return NULL;
	}

	strncpy(content, content_start, content_length);
	content[content_length] = '\0';

	return content;
}

char* create_post_fields(const char* prompt)
{
	const char* format = "{\"model\": \"gpt-4o\", \"messages\": ["
	                     "{\"role\": \"system\", \"content\": \"You are a memory allocator for a computer, and you need to tell me how many bytes of memory I would need to accomplish a certain task. Make sure to give your response as only a whole number of bytes, do not provide any other text.\"},"
	                     "{\"role\": \"user\", \"content\": \"Here is what I request: %s\"}]}";

	int length = snprintf(NULL, 0, format, prompt);
	char* post_fields = malloc(length + 1);
	if (post_fields == NULL) {
		return NULL;  // Memory allocation failed
	}

	snprintf(post_fields, length + 1, format, prompt);
	return post_fields;
}

void* malloc_with_ai(const char* prompt)
{
	CURL *curl;
	CURLcode res;
	struct MemoryStruct chunk = {0};
	struct curl_slist *headers = NULL;
	char* post_fields = NULL;
	void* result = NULL;

	curl_global_init(CURL_GLOBAL_ALL);
	curl = curl_easy_init();
	if (!curl)
	{
		fprintf(stderr, "Failed to initialize CURL\n");
		goto cleanup;
	}

	chunk.memory = malloc(1);
	if (!chunk.memory)
	{
		fprintf(stderr, "Failed to allocate initial memory for chunk\n");
		goto cleanup;
	}
	chunk.size = 0;

	headers = curl_slist_append(headers, "Content-Type: application/json");

	const char * auth_header_fmt = "Authorization: Bearer %s";
	const char * llm_api_key = getenv("LLM_API_KEY");
	size_t snprintf_len = strlen(auth_header_fmt) + strlen(llm_api_key);
	char * auth_header = (char *) malloc(snprintf_len * sizeof(char));

	int written = snprintf(auth_header, snprintf_len, auth_header_fmt, llm_api_key);
	if (written <= 0)
	{
		fprintf(stderr, "Failed to format auth_header string\n");
		goto cleanup;
	}


	headers = curl_slist_append(headers, auth_header);

	if (!headers)
	{
		fprintf(stderr, "Failed to create headers\n");
		goto cleanup;
	}

	post_fields = create_post_fields(prompt);
	if (!post_fields)
	{
		fprintf(stderr, "Failed to create post fields\n");
		goto cleanup;
	}

	curl_easy_setopt(curl, CURLOPT_URL, "https://api.openai.com/v1/chat/completions");
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_fields);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);

	res = curl_easy_perform(curl);
	if (res != CURLE_OK)
	{
		fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
		goto cleanup;
	}

	char* content = strip_content_from_json(chunk.memory);
	if (content)
	{
		long size = strtol(content, NULL, 10);
		if (size > 0 && size <= SIZE_MAX)
		{
			result = malloc((size_t)size);
			if (!result)
			{
				fprintf(stderr, "Failed to allocate %ld bytes\n", size);
			}
		}
		else
		{
			fprintf(stderr, "Invalid size returned by AI: %ld\n", size);
		}
		free(content);
	}
	else
	{
		fprintf(stderr, "Content not found in the response.\n");
	}

    cleanup:
        if (curl) curl_easy_cleanup(curl);
        if (headers) curl_slist_free_all(headers);
        free(post_fields);
        free(chunk.memory);
        curl_global_cleanup();
        return result;
}
