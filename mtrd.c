#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define N 5

void* create_socket(void* param);

int num_pthread = 0;


int main(int argc, char **argv){

  pthread_t tid;
  char *msg1 = "1";
  pthread_create(&tid, NULL, create_socket, (void *) msg1);
  pthread_join(tid, NULL);

  if(num_pthread < N){
    printf("Error: %d\n", num_pthread);
  }else{
    printf("Value as expected: num_pthread = %d\n", num_pthread);
  }

  return 0;
}

void *create_socket(void *param){
  printf("%s\n", (char*)param);

  num_pthread++;

  pthread_t tid;
  printf("continue?\n");
  char flag[1];
  scanf("%s\n", flag);

  if(*flag == 'y'){

    char *msg2 = "2";
    printf("create new thread\n");

    pthread_create(&tid, NULL, create_socket, (void *) msg2);
    pthread_join(tid, NULL);
    return 0;
  }else{
    return 0;
  }
}
