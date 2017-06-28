#include <huix_str.h>
#include <stdio.h>

int main(int argc,char **argv)
{
	char **str_list;
	int count;
	if (argc != 3) {
		printf("usage value tokens\n");
		return 0;
	}
	count = split(argv[1],argv[2],&str_list);
	if (count == 0)
		return 0;
	else {
		int i = 0;
		for (i = 0;i < count; i++) {
			printf("%s\n",str_list[i]);
		}
	}
}
