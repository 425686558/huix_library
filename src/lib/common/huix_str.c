#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#define EXTEND_COUNT	5

int split(char *value,char *tokens,char ***ret)
{
	char *split_item = NULL;
	char *str_p,*tmp;
	char **str_list;
	int count = 0;

	if (!ret)
		return 0;

	str_p = strdup(value);
	tmp = str_p;
	if (!str_p) {
		return -ENOMEM;
	}

	*ret = calloc(sizeof(char *) , EXTEND_COUNT);
	str_list = *ret;
	if (!(*ret)) {
		return -ENOMEM;
	}
	while (tmp) {
		split_item = strsep(&tmp, tokens);
		if (!strlen(split_item))
			continue;
		count++;
		if (!(count % EXTEND_COUNT)) {
			if ((*ret = realloc(str_list,sizeof(char *) * (EXTEND_COUNT + count)))) {
				str_list = *ret;
			} else {
				int i = 0;
				for (i = 0;i < count-1;i++) {
					free(str_list[i]);
				}
				free(str_list);
				return -ENOMEM;
			}
		}
		str_list[count - 1] = strdup(split_item);
	}
	free(str_p);
	return count;
}
