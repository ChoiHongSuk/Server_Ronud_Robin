#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parsing.h"

int http_buffer_gen(_http_header* http, char* buffer, int size, int type)
{
	if ((http == NULL) || (size < 0) || (type < 0)) {
		printf("buffer gen error\n");
		return -1;
	}

	if (type == BUFFER) {
		if (size == 0) {
			if (http->buff.buffer == NULL) {
				printf("Buffer is NULL\n");
				return -1;
			}
		}
		else {
			http->buff.buffer = (char*)calloc(size + 1, 1);
			if (http->buff.buffer == NULL) {
				printf("Buffer calloc NULL\n");
				return -1;
			}
		}
	}
	else if (type == REQ) {
		if (size == 0) {
			if (http->buff.req_line == NULL) {
				printf("request_line is NULL\n");
				return -1;
			}
		}
		else {
			http->buff.req_line = (char*)calloc(size + 1, 1);
			if (http->buff.req_line == NULL) {
				printf("request_line calloc NULL\n");
				return -1;
			}
			memcpy(http->buff.req_line, buffer, size);
		}
	}
	else if (type == HEAD) {
		if (size == 0) {
			if (http->buff.head_line == NULL) {
				printf("head_line is NULL\n");
				return -1;
			}
		}
		else {
			http->buff.head_line = (char*)calloc(size + 1, 1);
			if (http->buff.head_line == NULL) {
				printf("head_line calloc NULL\n");
				return -1;
			}
			memcpy(http->buff.head_line, buffer, size);
		}
	}
	else if (type == METHOD) {
		if (size == 0) {
			http->method = (char*)calloc(NONE_SIZE + 1, 1);
			if (http->method == NULL) {
				printf("method calloc NULL\n");
				return -1;
			}
			memcpy(http->method, NONE, NONE_SIZE);
		}
		else {
			http->method = (char*)calloc(size + 1, 1);
			if (http->method == NULL) {
				printf("method calloc NULL\n");
				return -1;
			}
			memcpy(http->method, buffer, size);
		}
	}
	else if (type == PATH){
		if (size == 0) {
			http->uri.path = (char*)calloc(NONE_SIZE + 1, 1);
			if (http->uri.path == NULL) {
				printf("path calloc NULL\n");
				return -1;
			}
			memcpy(http->uri.path, NONE, NONE_SIZE);
		}
		else {
			http->uri.path = (char*)calloc(size + 1, 1);
			if (http->uri.path == NULL) {
				printf("path calloc NULL\n");
				return -1;
			}
			memcpy(http->uri.path, buffer, size);
		}
	}
	else if (type == QUERY){
		if (size == 0) {
			http->uri.query = (char*)calloc(NONE_SIZE + 1, 1);
			if (http->uri.query == NULL) {
				printf("query calloc NULL\n");
				return -1;
			}
			memcpy(http->uri.query, NONE, NONE_SIZE);
		}
		else {
			http->uri.query = (char*)calloc(size + 1, 1);
			if (http->uri.query == NULL) {
				printf("query calloc NULL\n");
				return -1;
			}
			memcpy(http->uri.query, buffer, size);
		}
	}
	else if (type == FRAG){
		if (size == 0) {
			http->uri.frag = (char*)calloc(NONE_SIZE + 1, 1);
			if (http->uri.frag == NULL) {
				printf("fragment calloc NULL\n");
				return -1;
			}
			memcpy(http->uri.frag, NONE, NONE_SIZE);
		}
		else {
			http->uri.frag = (char*)calloc(size + 1, 1);
			if (http->uri.frag == NULL) {
				printf("fragment calloc NULL\n");
				return -1;
			}
			memcpy(http->uri.frag, buffer, size);
		}
	}
	else if (type == VERSION){
		if (size == 0) {
			http->version = (char*)calloc(NONE_SIZE + 1, 1);
			if (http->version == NULL) {
				printf("version calloc NULL\n");
				return -1;
			}
			memcpy(http->version, NONE, NONE_SIZE);
		}
		else {
			http->version = (char*)calloc(size + 1, 1);
			if (http->version == NULL) {
				printf("version calloc NULL\n");
				return -1;
			}
			memcpy(http->version, buffer, size);
		}
	}
	else if (type == BODY){
		if (size == 0) {
			http->body = (char*)calloc(NONE_SIZE + 1, 1);
			if (http->body == NULL) {
				printf("body calloc NULL\n");
				return -1;
			}
			memcpy(http->body, NONE, NONE_SIZE);
		}
		else {
			http->body = (char*)calloc(size + 1, 1);
			if (http->body == NULL) {
				printf("body calloc NULL\n");
				return -1;
			}
			memcpy(http->body, buffer, size);
		}
	}
	else if (type == GARBAGE){
		if (size == 0) {
			http->garbage = (char*)calloc(NONE_SIZE + 1, 1);
			if (http->garbage == NULL) {
				printf("garbage calloc NULL\n");
				return -1;
			}
			memcpy(http->garbage, NONE, NONE_SIZE);
		}
		else {
			http->garbage = (char*)calloc(size + 1, 1);
			if (http->garbage == NULL) {
				printf("garbage calloc NULL\n");
				return -1;
			}
			memcpy(http->garbage, buffer, size);
		}
	}
	else {
		printf("alloc error\n");
		return -1;
	}

	return 0;
}

int http_print_buffer(_http_header* http, _node_t* head, _qp_t* qp_head)
{
	if ((http == NULL) || (head == NULL)) {
		printf("request structure is NULL\n");
		return -1;
	}

	printf("=================================================\n");

	if (http->method == NULL) {
		printf("method: none\n");
	}
	else {
		printf("method : %s\n", http->method);
	}

	if (http->uri.path == NULL) {
		printf("path: none\n");
	}
	else {
		printf("path : %s\n", http->uri.path);
	}

#if 0
	/* query payload list */
	if (http->uri.query == NULL) {
		printf("none\n");
	}
	else {
		printf("query : %s\n", http->uri.query);
	}
#endif
	printf("query: \n");
	print_list_query_payload(qp_head, OP_QUERY);
	printf("\n");

	printf("payload: \n");
	print_list_query_payload(qp_head, OP_PAYLOAD);
	printf("\n");

	if (http->uri.frag == NULL) {
		printf("fragment: none\n");
	}
	else {
		printf("fragment : %s\n", http->uri.frag);
	}

	if (http->version == NULL) {
		printf("version: none\n");
	}
	else {
		printf("version : %s\n", http->version);
	}

	/* header list */
	printf("head: \n");
	print_list_header(head);
	printf("\n");

	if (http->body == NULL) {
		printf("body: none\n");
	}
	else {
		printf("body : %s\n", http->body);
	}
	if (http->garbage == NULL) {
		printf("none\n");
	}
	else {
		printf("garbage : %s\n", http->garbage);
	}

	return 0;
}

void http_buffer_free(_http_header* http)
{
	if (http->buff.buffer != NULL) {
		free(http->buff.buffer);
	}
	if (http->buff.req_line != NULL) {
		free(http->buff.req_line);
	}
	if (http->buff.head_line != NULL) {
		free(http->buff.head_line);
	}
	if (http->method != NULL) {
		free(http->method);
	}
	if (http->version != NULL) {
		free(http->version);
	}
	if (http->uri.path != NULL) {
		free(http->uri.path);
	}
	if (http->uri.query != NULL) {
		free(http->uri.query);
	}
	if (http->uri.frag != NULL) {
		free(http->uri.frag);
	}
	if (http->body != NULL) {
		free(http->body);
	}
	if (http->garbage != NULL) {
		free(http->garbage);
	}
	if (http != NULL) {
		free(http);
	}
}
