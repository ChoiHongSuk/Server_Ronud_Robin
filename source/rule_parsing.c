#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parsing.h"
#include "parsing_list.h"

int config_file_read(_rule_node_t* rule_head, char* rule_path)
{
	FILE* file = NULL;
	int close_state = 0, rv = 0;
	size_t buffer_size = 0;
	ssize_t	line_size = 0;
	char* buffer = NULL;
	_addr_node_t* addr_head = NULL;

	if (rule_path == NULL) {
		printf("config 파일이 존재하지 않습니다.\n");
		return -1;
	}

	file = fopen(rule_path, "r");
	if (file == NULL) {
		printf("파일을 열 수 없습니다.\n");
		return -1; 
	}

	line_size = getline(&buffer, &buffer_size, file);
	while (line_size != -1) {
		/* parsing */
		rv = config_parsing(rule_head, addr_head, buffer, line_size);
		if (rv < 0) {
			delete_list_rule(rule_head);
			free(buffer);
			fclose(file);
			return -1;
		}
		line_size = getline(&buffer, &buffer_size, file);
	}
	free(buffer);

	close_state = fclose(file);
	if (close_state != 0) {
		printf("파일을 닫을 수 없습니다.\n");
		return -1;
	}

	return 0;
}

int config_parsing(_rule_node_t* rule_head, _addr_node_t* addr_head, char* buffer, int line_size)
{
	char* offset_type = NULL;
	char* offset_match = NULL;
	char* offset_string = NULL;
	char* string = NULL;
	char* ip_addr = NULL;
	int offset_type_size = 0, offset_match_size = 0, string_size = 0, ip_addr_size = 0;
	int type = 0, match = 0, rv = 0;

	if ((buffer == NULL) || (line_size < 0)) {
		printf("buffer, line_size error\n");
		return -1;
	}

	addr_head = NULL;
	addr_head = (_addr_node_t*)calloc(1, sizeof(_addr_node_t));
	if (addr_head == NULL) {
		printf("addr_node is NULL\n");
		return -1;
	}
	addr_head->next = NULL;
	addr_head->count = 0;

	/* type */
	offset_type = memchr(buffer, SP, line_size);
	if (offset_type == NULL) {
		printf("offset_type error\n");
		return -1;
	}
	offset_type_size = offset_type - buffer;
	if (offset_type_size < 0) {
		printf("offset_type_size error\n");
		return -1;
	}

	if (offset_type_size > RULE_TYPE_SIZE_MAX) {
		printf("Rule type size error\n");
		return -1;
	}

	if (memcmp(buffer, RULE_PATH, offset_type_size) == 0) {
		type = TYPE_PATH;
	}
	else if (memcmp(buffer, RULE_HOST, offset_type_size) == 0) {
		type = TYPE_HOST;
	}
	else {
		type = TYPE_GARB;
	}

	/* match */
	line_size = line_size - offset_type_size;
	if (line_size < 0) {
		printf("line_size error\n");
		return -1;
	}
	offset_match = memchr(offset_type + SINGLE, SP, line_size);
	if (offset_match == NULL) {
		printf("offset_match error\n");
		return -1;
	}

	offset_match_size = offset_match - (offset_type + SINGLE);
	if (offset_match_size < 0) {
		printf("offset_match_size error\n");
		return -1;
	}

	if (offset_match_size > RULE_MATCH_SIZE_MAX) {
		printf("Rule match size error\n");
		return -1;
	}

	if (memcmp(offset_type + SINGLE, START, offset_match_size) == 0) {
		match = MATCH_START;
	}
	else if (memcmp(offset_type + SINGLE, ANY, offset_match_size) == 0) {
		match = MATCH_ANY;
	}
	else if (memcmp(offset_type + SINGLE, END, offset_match_size) == 0) {
		match = MATCH_END;
	}
	else {
		match = MATCH_GARB;
	}

	/* string */
	line_size = line_size - offset_match_size;
	if (line_size < 0) {
		printf("line_size error\n");
		return -1;
	}

	offset_string = memchr(offset_match + SINGLE , SP, line_size);
	if (offset_string == NULL) {
		printf("offset_string is NULL\n");
		return -1;
	}

	string_size = offset_string - (offset_match + SINGLE);
	if (string_size < 0) {
		printf("string_size error\n");
		return -1;
	}
	string = offset_match + SINGLE;
	
	/* ip addr */
	if (line_size < string_size) {
		printf("ip_addr_size error\n");
		return -1;
	}
	ip_addr_size = line_size - string_size;
	ip_addr = offset_string + SINGLE;

	if ((type != TYPE_GARB) && (match != MATCH_GARB)) {
		rv = addr_parsing(addr_head, ip_addr, ip_addr_size);
		if (rv < 0) {
			delete_list_addr(addr_head);
			return -1;
		}

		rv = rule_add_list(rule_head, addr_head, type, match, string, string_size);
		if (rv < 0){
			delete_list_rule(rule_head);
			return -1;
		}
	}

	return 0;
}

int addr_parsing(_addr_node_t* addr_head, char* ip_addr, int ip_addr_size)
{
	char* com = NULL;
	char* colon = NULL;
	char* lf = NULL;
	char* check = NULL;
	int ip_size = 0, port_size = 0, ip_port_size = 0, rv = 0;
	char* ip = NULL;
	char* port = NULL;
	int ip_t[IP_PREFIX];
	int port_t = 0;

	if ((addr_head == NULL) || (ip_addr == NULL) || (ip_addr_size < 0)) {
		printf("addr paring error\n");
		return -1;
	}

	memset(ip_t, 0, IP_PREFIX);

	while (1) {
		com = memchr(ip_addr, COM, ip_addr_size);
		if (com != NULL) {
			ip_port_size = com - ip_addr;
		}
		else {
			lf = memchr(ip_addr, LF, ip_addr_size);
			ip_port_size = lf - ip_addr;
		}

		colon = memchr(ip_addr, RULE_COLON, ip_port_size);
		if (colon == NULL) {
			printf("올바른 ip, port가 아닙니다.\n");
			return -1;
		}

		ip_size = colon - ip_addr;
		port_size = ip_port_size - (ip_size + SINGLE);

		ip = ip_addr;
		port = colon + SINGLE;

		/* ','까지 ':'이 더 들어있을 때 */
		check = memchr(colon + SINGLE, RULE_COLON, port_size);
		if (check == NULL) {
			rv = ip_port_convert(ip, port, ip_size, port_size, ip_t, &port_t);
			if (rv < 0) return -1;
			rv = addr_add_list(addr_head, ip_t, port_t);
			if (rv < 0) return -1;
		}

		if (com == NULL) {
			break;
		}

		ip_addr = com + SINGLE;
		ip_addr_size = ip_addr_size - (ip_port_size + SINGLE);
	}

	return 0;
}

int ip_port_convert(char* ip, char* port, int ip_size, int port_size, int* ip_t, int* port_t)
{
	int idx = 0, prefix_size = 0;
	char* dot = NULL;
	char* colon = NULL;
	char* check = NULL;

	if ((ip == NULL) || (port == NULL) || (ip_size < 0) || (ip_size > IP_MAX) || (ip_t == NULL) || (*port_t < 0) || (*port > PORT_MAX)) {
		printf("ip convert NULL error\n");
		return -1;
	}
	
	while (1) {
		dot = memchr(ip, DOT, ip_size);
		if (dot != NULL) {
			prefix_size = dot - ip;
		}
		else {
			colon = memchr(ip, RULE_COLON, ip_size + SINGLE);
			prefix_size = colon - ip;
		}

		ip_t[idx] = strtoul(ip, &check, DEC);
		if (ip == check) {
			printf("형식을 벗어난 ip입니다.\n");
			return -1;
		}

		if (number_max_get(ip_t[idx], prefix_size)) {
			printf("형식을 벗어난 ip입니다.\n");
			return -1;
		}

		if (dot == NULL) {
			break;
		}

		ip_size = ip_size - (prefix_size + SINGLE);
		ip = dot + SINGLE;
		idx++;
	}
	check = NULL;
	
	*port_t = strtoul(port, &check, DEC);
	if (port == check) {
		printf("형식을 벗어난 port입니다.\n");
		return -1;
	}
	if (number_max_get(*port_t, port_size)) 
	{
		printf("형식을 벗어난 port입니다.\n");
		return -1;
	}	

	return 0;
}

int number_max_get(int number, int size)
{
	int count = 0;

	if ((number < 0) || (size < 0)) {
		return 1;
	}
	do {
		count++;
		number /= 10;
	} while(number > 0);

	if (count != size) {
		return 1;
	}

	return 0;
}
