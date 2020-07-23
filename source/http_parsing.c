#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "parsing.h"
#include "parsing_list.h"
#include "mem_string.h"

int file_read(char* path, _http_header* http, _node_t* head, _qp_t* qp_head)
{
	int fd = 0, close_state = 0, rv = 0, full_size = 0;
	char* buffer = NULL;

	if (path == NULL) {
		printf("경로 값이 존재하지 않습니다.\n");
		return -1;
	}

	rv = buffer_full_size(path, &full_size);
	if (rv < 0) return -1;

	fd = open(path, O_RDONLY);
	if (fd == -1) {
		printf("파일을 열 수 없습니다.\n");
		return -1;
	}

	/* Full buffer */
	rv = http_buffer_gen(http, buffer, full_size, BUFFER);
	if (rv < 0) {
		close(fd);
		return -1;
	}

	rv = read(fd, http->buff.buffer, full_size);
	if (rv < 0) {
		printf("파일을 읽지 못 했습니다.\n");
		close(fd);
		return -1;
	}

	/* buffer devide */
	rv = buffer_devide(http->buff.buffer, full_size, http, head, qp_head);
	if (rv < 0) {
		close(fd);
		return -1;
	}

	close_state = close(fd);
	if (close_state == -1) {
		printf("파일을 닫을 수 없습니다.\n");
		return -1;
	}

	return 0;
}

int buffer_full_size(char* path, int* full_size)
{
	struct stat file_size;
	int rv = 0;

	rv = stat(path, &file_size);
	if(rv < 0) {
		printf("파일 사이즈를 읽는데 실패했습니다.\n");
		return -1;
	}
	*full_size = file_size.st_size;
	if (full_size < 0) {
		printf("잘못된 파일 사이즈 입니다.\n");
		return -1;
	}

	return 0;
}

int buffer_devide(char* buffer, int full_size, _http_header* http, _node_t* head, _qp_t* qp_head)
{
	char* crlf = NULL;
	char* crlfcrlf = NULL;
	int req_size = 0, head_size = 0;
	int body_size = 0, message_size = 0, garb_size = 0, c_length = 0;
	int rv = 0;
	int check = 0, flag_type = 0, flag_length = 0, qp_option = 0, option = 0;
	char* req_line = NULL;
	char* head_line = NULL;
	char* message_line = NULL;
	char* body_line = NULL;
	char* garb_line = NULL;

	if (buffer == NULL) {
		printf("버퍼 매개 값이 잘못되었습니다.\n");
		return -1;
	}

	/* request line */
	crlf = memstr(buffer, CRLF, full_size);
	if (crlf == NULL) {
		printf("buffer devide error(crlf)\n");
		return -1;
	}

	check = memcmp(crlf + 1, CRLF, CRLF_SIZE);
	if (check == 0) {
		printf("crlf next crlf\n");
		return -1;
	}

	req_size = crlf - buffer;
	req_line = buffer;
	rv = http_buffer_gen(http, buffer, req_size, REQ);
	if (rv < 0) return -1;

	/* request extractor */
	rv = request_extractor(http->buff.req_line, req_size, http, qp_head);
	if (rv < 0) return -1;

	/* head line list */
	crlfcrlf = memstr(buffer, CRLFCRLF, full_size);
	if (crlfcrlf == NULL) {
		printf("buffer devide error(crlfcrlf)\n");
		return -1;
	}

	head_size = (crlfcrlf + CRLF_SIZE) - (crlf + CRLF_SIZE);
	head_line = crlf + CRLF_SIZE;
	rv = http_buffer_gen(http, head_line, head_size, HEAD);
	if (rv < 0) return -1;

	rv = head_extractor(http->buff.head_line, head_size, head);
	if (rv < 0) return -1;

	message_size = full_size - (req_size + head_size + CRLFCRLF_SIZE);
	message_line = crlfcrlf + CRLFCRLF_SIZE;	

	/* query payload add */
	flag_type = content_type_get(head);
	flag_length = content_length_get(head, &c_length);
	/* Content-Length, Content-Type Classify (0, 0) (1, 0) => 0: exist, 1: not exist */
	if (((flag_type == 0) && (flag_length == 0)) || ((flag_type == 1) && (flag_length == 0))) {
		qp_option = OP_BODY_PAYLOAD;
		option = OP_PAYLOAD;
		if (flag_type == 1) {
			qp_option = OP_BODY;
			option = BODY;
		}

		body_size = c_length;
		if (message_size < c_length) {
			body_size = message_size;
		}
		body_line = message_line;
		garb_size = message_size - c_length;
		if (garb_size < 0) garb_size = 0;
		garb_line = message_line + (c_length + SINGLE);	
	}
	else {
		body_size = 0;
		body_line = NULL;
		garb_size = message_size;
		garb_line = message_line;	
	}

	/* payload_line */
	if (body_line != NULL) {
		rv = query_payload_extractor(body_line, body_size, qp_option, option, http, qp_head);
		if (rv < 0) return -1;
	}

	/* garb_line */
	rv =garb_extractor(garb_line, garb_size, http);
	if (rv < 0) return -1;


	return 0;
}

int request_extractor(char* req_line, int req_size, _http_header* http, _qp_t* qp_head)
{
	char* offset_1 = NULL;
	char* offset_2 = NULL;
	char* mem = NULL;
	int method_size = 0, uri_size = 0, version_size = 0, mem_size = 0, rv = 0;
	char* method_line = NULL;
	char* version_line = NULL;
	char* uri_line = NULL;

	if ((req_line == NULL) || (req_size < 0)) {
		printf("request 버퍼 매개 값이 잘못 되었습니다.\n");
		return -1;
	}

	/* method */
	offset_1 = memchr(req_line, SP, req_size);
	if (offset_1 == NULL) {
		printf("method_line devide error\n");
		return -1;
	}
	method_size = offset_1 - req_line;
	method_line = req_line;
	rv = http_buffer_gen(http, method_line, method_size, METHOD);
	if (rv < 0) return -1;

	/* uri */
	mem_size = req_size - method_size;
	if (mem_size < 0) {
		printf("mem_size error\n");
		return -1;
	}
	mem = offset_1 + SINGLE;
	offset_2 = memchr(mem, SP, mem_size);
	if (offset_2 == NULL) {
		printf("uri_line devide error\n");
		return -1;
	}
	uri_size = offset_2 - (offset_1 + SINGLE);
	uri_line = offset_1 + SINGLE;
	rv = uri_extractor(uri_line, uri_size, http, qp_head);
	if (rv < 0) return -1;

	/* version */
	version_size = req_size - (method_size + uri_size + SINGLE + SINGLE);
	version_line = offset_2 + SINGLE;
	rv = http_buffer_gen(http, version_line, version_size, VERSION);
	if (rv < 0) return -1;

	return 0;
}

int uri_extractor(char* uri_line, int uri_size, _http_header* http, _qp_t* qp_head)
{
	char* path = NULL;
	char* query = NULL;
	char* frag = NULL;
	int path_size = 0, query_size = 0, frag_size = 0, rv = 0;
	char* path_line = NULL;
	char* query_line = NULL;
	char* frag_line = NULL;

	if ((uri_line == NULL) || (uri_size < 0)) {
		printf("param error\n");
		return -1;
	}

	/* path | query | frag */
	path_size = uri_size;
	path = memchr(uri_line, SLIDE, uri_size);
	if (path == NULL) {
		rv = http_buffer_gen(http, uri_line, uri_size, PATH);
		if (rv < 0) return -1;

		return 0;
	}
	http->uri.path_size = path_size;
	
	query = memchr(uri_line, Q_MARK, uri_size);
	frag = memchr(uri_line, SHAP, uri_size);

	if ((query != NULL) && (frag != NULL)) {
		/* path */
		path_size = query - uri_line;
		path_line = uri_line;
		rv = http_buffer_gen(http, path_line, path_size, PATH);
		if (rv < 0) return -1;
		http->uri.path_size = path_size;

		/* query */
		query_size = frag - query - SINGLE;
		query_line = query + SINGLE;
		rv = http_buffer_gen(http, query_line, query_size, QUERY);
		if (rv < 0) return -1;
		rv = query_payload_extractor(query_line, query_size, OP_BODY_PAYLOAD, OP_QUERY, http, qp_head);
		if (rv < 0) return -1;
		
		/* frag */
		frag_size = path_size + query_size + SINGLE;
		frag_line = frag + SINGLE;
		rv = http_buffer_gen(http, frag_line, frag_size, FRAG);
		if (rv < 0) return -1;
	}
	else if ((query != NULL) && (frag == NULL)) {
		/* path */
		path_size = query - uri_line;
		path_line = uri_line;
		rv = http_buffer_gen(http, path_line, path_size, PATH);
		if (rv < 0) return -1;
		http->uri.path_size = path_size;

		/* query */
		query_size = uri_size - path_size - SINGLE;
		query_line = query + SINGLE;
		rv = http_buffer_gen(http, query_line, query_size, QUERY);
		if (rv < 0) return -1;
		rv = query_payload_extractor(query_line, query_size, OP_BODY_PAYLOAD, OP_QUERY, http, qp_head);
		if (rv < 0) return -1;

		/* frag */
		frag_size = 0;
		frag_line = NULL;
		rv = http_buffer_gen(http, frag_line, frag_size, FRAG);
		if (rv < 0) return -1;
	}
	else if ((query == NULL) && (frag != NULL)) {
		/* path */
		path_size = frag - uri_line;
		path_line = uri_line;
		rv = http_buffer_gen(http, path_line, path_size, PATH);
		if (rv < 0) return -1;
		http->uri.path_size = path_size;

		/* query */
		query_size = 0;
		query_line = NULL;
		rv = http_buffer_gen(http, query_line, query_size, QUERY);
		if (rv < 0) return -1;

		/* frag */
		frag_size = path_size + query_size + SINGLE;
		frag_line = frag + SINGLE;
		rv = http_buffer_gen(http, frag_line, frag_size, FRAG);
		if (rv < 0) return -1;
	}
	else if ((query == NULL) && (frag == NULL)) {
		/* path */
		path_size = uri_size;
		path_line = uri_line;
		rv = http_buffer_gen(http, path_line, path_size, PATH);
		if (rv < 0) return -1;
		http->uri.path_size = path_size;

		/* query */
		query_size = 0;
		query_line = NULL;
		rv = http_buffer_gen(http, query_line, query_size, QUERY);
		if (rv < 0) return -1;

		/* frag */
		frag_size = 0;
		frag_line = NULL;
		rv = http_buffer_gen(http, frag_line, frag_size, FRAG);
		if (rv < 0) return -1;
	}
	else {
		printf("형식에서 벗어난 uri입니다.\n");
		return -1;
	}


	return 0;
}

int query_payload_extractor(char* line, int size, int qp_option, int option, _http_header* http, _qp_t* qp_head)
{
	char* equal = NULL;
	char* amp = NULL;
	char* body_line = NULL;
	char* name = NULL;
	char* value = NULL;
	int name_size = 0, value_size = 0, name_value_size = 0, body_size = 0, rv = 0;

	if ((line == NULL) || (size < 0)) {
		printf("query payload error\n");
		return -1;
	}
	body_line = line;
	body_size = size;
	if ((qp_option == OP_BODY_PAYLOAD) || (option == OP_PAYLOAD)) {
		while (1) {
			amp = memchr(line, AMP, size);
			if (amp != NULL) {
				name_value_size = amp - line;
			}
			else {
				name_value_size = size;
			}

			equal = memchr(line, EQUAL, name_value_size);
			if (equal == NULL) {
				name = line;
				value = NULL;

				name_size = name_value_size;
				value_size = 0;
			}
			else {
				name = line;
				value = equal + SINGLE;

				name_size = equal - line;
				value_size = name_value_size - (name_size + SINGLE);
			}

			rv = query_payload_add_list(qp_head, name, value, name_size, value_size, option);
			if (rv < 0) return -1;

			if (amp == NULL) break;

			line = amp + SINGLE;
			size = size - (name_value_size + SINGLE);	
		}
	}
	if ((qp_option == OP_BODY) || (option == OP_PAYLOAD)) {
		rv = http_buffer_gen(http, body_line, body_size, BODY);
		if (rv < 0) return -1;
	}

	return 0;
}

int head_extractor(char* head_line, int head_size, _node_t* head)
{
	char* colon = NULL;
	char* crlf = NULL;
	int name_size = 0, value_size = 0, rv = 0, name_value_size = 0;
	char* name = NULL;
	char* value = NULL;

	if ((head_line == NULL) || (head_size < 0)) {
		printf("head line NULL error\n");
		return -1;
	}

	while (1) {
		crlf = memstr(head_line, CRLF, head_size);
		if (crlf == NULL) break;
		name_value_size = crlf - head_line;

		colon = memstr(head_line, COLON, name_value_size);
		if (colon == NULL) {
			name = head_line;
			value = NULL;

			name_size = name_value_size;
			value_size = 0;
		}
		else {
			name = head_line;
			value = colon + COLON_SIZE;

			name_size = colon - head_line;
			value_size = name_value_size - (name_size + COLON_SIZE);
		}
		rv = add_list(head, name, value, name_size, value_size);
		if (rv < 0) return -1;

		head_line = crlf + CRLF_SIZE;
		head_size = head_size - (name_value_size + CRLF_SIZE);
	}

	return 0;
}

int garb_extractor(char* garb_line, int garb_size, _http_header* http)
{
	int rv = 0;

	rv = http_buffer_gen(http, garb_line, garb_size, GARBAGE);
	if (rv < 0) return -1;

	return 0;
}

int content_length_get(_node_t* head, int* c_length)
{
	_node* curr = NULL;
	int rv = 0;

	if ((head == NULL) || (c_length < 0)) {
		printf("content_length_get: head NULL error\n");
		return -1;
	}

	curr = head->current;
	while (curr != NULL) {
		if (strcmp(curr->name, CONTENT_LENGTH) == 0) {
			if (curr->value_size < 0) {
				printf("value size error\n");
				return -1;
			}
			if (curr->value == NULL) {
				return 1;
			}
			/* strtoul */
			rv = content_length_convert(curr->value, curr->value_size, c_length);
			if (rv < 0) {
				return -1;
			}
			else {
				return 0;
			}
		}
		curr = curr->prev;
	}
	if (curr == NULL) return 1;

	return 0;
}

int content_type_get(_node_t* head)
{
	_node* curr = NULL;

	if (head == NULL) {
		printf("content_type_get: head NULL error\n");
		return -1;
	}

	curr = head->current;
	while (curr != NULL) {
		if (strcmp(curr->name, CONTENT_TYPE) == 0) {
			if ((curr->value == NULL) || (strlen(curr->value) == 0)) {
				return 1;
			}
			else if (strcmp(curr->value, CONTENT_TYPE_CON) == 0) {
				return 0;
			}
			else {
				printf("content_type context error\n");
				return -1;
			}
		}
		curr = curr->prev;
	}
	if (curr == NULL) return 1;

	return 0;
}

int content_length_convert(char* value, int value_size, int* c_length)
{
	char* check = NULL;

	*c_length = strtoul(value, &check, DEC);
	if (value == check) return 1;
	if (number_max_get(*c_length, value_size)) {
		printf("Content-Length에 문자가 섞여있습니다.(1)\n");
		return -1;
	}

	return 0;
}
