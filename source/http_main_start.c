#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parsing.h"
#include "parsing_list.h"

int start_http_header_parsing(char** argv)
{
	char path[HEADER_PATH_MAX];
	char* rule_path = NULL;
	int rv = 0;
	_rule_node_t* rule_head = NULL; /* rule node */
	_http_header* http = NULL;
	_node_t* head = NULL;
	_qp_t* qp_head = NULL;

	memset(path, 0, SINGLE);
	rule_head = (_rule_node_t*)calloc(1, sizeof(_rule_node_t));
	if (rule_head == NULL) {
		printf("rule head calloc NULL error\n");
		return -1;
	}
	rule_head->next = NULL;

	rule_path = argv[1];

	rv = config_file_read(rule_head, rule_path);
	if (rv < 0) {
		goto FREE_CONFIG;
	}

	while (1) {
		printf("http헤더 경로를 입력하세요.(ex. ./example.txt) [Exit: 'q'] > ");
		if (!scanf("%63s", path)) {
			printf("http 헤더 경로가 올바르지 않습니다.\n");
			continue;
		}

		if (strncmp(path, EXIT, strlen(path)) == 0) {
			printf("프로그램을 종료합니다.\n");
			break;
		}

		http = (_http_header*)calloc(1, sizeof(_http_header));
		if (http == NULL) {
			printf("http calloc NULL error\n");
			continue;
		}
		head = (_node_t*)calloc(1, sizeof(_node_t));
		if (head == NULL) {
			printf("head calloc NULL error\n");
			continue;
		}
		head->next = NULL;
		qp_head = (_qp_t*)calloc(1, sizeof(_qp_t));
		if (qp_head == NULL) {
			printf("query payload head NULL error\n");
			continue;
		}

		rv = file_read(path, http, head, qp_head);
		if (rv < 0) goto FREE_HTTP;

		rv = http_print_buffer(http, head, qp_head);
		if (rv < 0) goto FREE_HTTP;

		/* request, rule compare */
		rv = parsing_compare(rule_head, head, http);
		if (rv < 0) goto FREE_HTTP;

FREE_HTTP:
		http_buffer_free(http);
		delete_list(head);
		delete_list_query_payload(qp_head);
	}

FREE_CONFIG:
	delete_list_rule(rule_head);

	return 0;
}
