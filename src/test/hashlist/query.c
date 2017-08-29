#include <hash_list.h>
#include <stdio.h>
#include <sys/time.h>
hash_t *ht;
int main()
{
  int ret = 0;
  int val;
  int i = 0,j = 0;
  char buffer[30];
  struct timeval tpstart,tpend,tpmed;
  u64 time_tmp;
  int maxcnt = 200;
  gettimeofday(&tpstart,NULL);
  ht = hash_list_init(4, 32, &ret, HASH_LIST_SEARCH_STRING);
  for (j = 0;j < 1000;j++) {
  for (i = 0;i < 100000;i++) {
    sprintf(buffer,"i_%04d",i);
    ht->insert(ht,buffer,(void *)i,&ret);
    if (ret < 0) {
      printf("insert error:ret:%d\n",ret);
    }

    /*
    val = (int) ht->query(ht,buffer,&ret);
    if (ret < 0) {
      printf("query error:ret:%d\n",ret);
    }*/
  }

//  hash_list_info(ht);

/*  gettimeofday(&tpmed,NULL);
  time_tmp = 1000000*(tpmed.tv_sec-tpstart.tv_sec) +
    tpmed.tv_usec-tpstart.tv_usec;
  printf("insert time_tmp:%f\n",time_tmp/1000000.0);*/
//  for (j = 0;j < 100;j++)
/*  for (i = 0;i < 100;i++) {
    sprintf(buffer,"i_%04d",i);
    val = (int) ht->query(ht,buffer,&ret);
    if (ret < 0) {
      printf("query error:ret:%d\n",ret);
    }
    printf("buffer:%s val:%d\n",buffer,val);
  }*/
  /*
  gettimeofday(&tpend,NULL);
  time_tmp = 1000000*(tpend.tv_sec-tpmed.tv_sec) +
    tpend.tv_usec-tpmed.tv_usec;
  printf("query:%f\n",time_tmp/1000000.0);*/

  //hash_list_info(ht);
  //printf("+++++++++++++++++++++\n");
  for (i = 0;i < (100000 - 1);i++) {
    sprintf(buffer,"i_%04d",i);
    val = (int)ht->delete(ht,buffer);
    //printf("delete:val:%d\n",val);
  }
  }
  hash_list_info(ht);
    gettimeofday(&tpend,NULL);
  time_tmp = 1000000*(tpend.tv_sec-tpmed.tv_sec) +
    tpend.tv_usec-tpmed.tv_usec;
  printf("delete:%f\n",time_tmp/1000000.0);
/*
  for (i = 0;i < 100;i++) {
    sprintf(buffer,"i_%04d",i);
    val = (int) ht->query(ht,buffer,&ret);
    if (ret < 0) {
      printf("query error:ret:%d\n",ret);
    }
    printf("buffer:%s val:%d\n",buffer,val);
  }*/
}
