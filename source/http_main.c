#include <stdio.h>
#include "parsing.h"

int arg_valid(int argc, char** argv);

int main(int argc, char* argv[])
{
	if (arg_valid(argc, argv) < 0) {
		return -1;
	}
	
	start_http_header_parsing(argv);

	return 0;
}

int arg_valid(int argc, char** argv)
{
	if (argc != 2) {
		printf("인자 수가 맞지 않습니다.\n");
		return -1;
	} 
	if ((argv[1]) == NULL) {
		printf("인자 값이 NULL입니다.\n");
		return -1;
	}

	return 0;
}
