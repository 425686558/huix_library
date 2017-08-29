#include <hash_list.h>
#include <stdio.h>
#include <sys/time.h>
hash_t *ht;
int main()
{
  int ret = 0;
  int val;
  int i = 0;
  char buffer[30];
  struct timeval tpstart,tpmed;
  u64 time_tmp;
  int maxcnt = 200;
  gettimeofday(&tpstart,NULL);
  ht = hash_list_init(4, 32, &ret, HASH_LIST_SEARCH_STRING);
  for (i = 0;i < 100000;i++) {
    sprintf(buffer,"i_%04d",i);
    ht->insert(ht,buffer,(void *)i,&ret);
    if (ret < 0) {
      printf("insert error:ret:%d\n",ret);
    }
  }

  gettimeofday(&tpmed,NULL);
  time_tmp = 1000000*(tpmed.tv_sec-tpstart.tv_sec) +
    tpmed.tv_usec-tpstart.tv_usec;
  printf("insert time_tmp:%f\n",time_tmp/1000000.0);
}
