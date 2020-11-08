#include <semaphore.h>
#include  <stdio.h>
#include <errno.h>
#include  <stdlib.h>
#include <unistd.h>
#include  <sys/types.h>

#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

#include  <sys/ipc.h>
#include  <sys/shm.h>
#include <sys/wait.h>

void depositMoney(int* bank_acc);
void withdrawMoney(int* bank_acc);

int  main(int  argc, char *argv[])
{
  int ShmID;
  int *ShmPTR;
  pid_t  pid;
  int status;
  int fd;
  int i; 
  int nloop=5; 
  int zero=0; 
  int *counter_ptr;
  sem_t *mutex; 
  
  fd = open("log.txt",O_RDWR|O_CREAT,S_IRWXU);
  write(fd,&zero,sizeof(int));
  counter_ptr = mmap(NULL,sizeof(int),PROT_READ |PROT_WRITE,MAP_SHARED,fd,0);
  close(fd);

  /* create, initialize semaphore */
 if ((mutex = sem_open("examplesaaaebbmaphore", O_CREAT, 0644, 1)) == SEM_FAILED) {
    perror("semaphore initilization");
    exit(1);
  }
  
    pid = fork();
    if (pid > 0) {
        
      for (i = 0; i>-1; i++){
          sleep(rand()%6);
          printf("Dear Old Dad: Attempting to Check Balance\n");\
          sem_wait(mutex);
          int rand_num = rand()%101;
          if (rand_num % 2 == 0){
            if (*counter_ptr < 100){
              depositMoney(counter_ptr);
            } else {
              printf("Dear Old Dad: Thinks Student has enough Cash ($%d)\n", *counter_ptr);
            }
          }else{
            printf("Dear Old Dad: Last Checking Balance = $%d\n", *counter_ptr);
          }
          sem_post(mutex);              
      }

      exit(1);
    }
    else if (pid == 0) {
        for (i = 0; i>-1; i++){
            sleep(rand()%6);
            printf("Poor Student: Attempting to Check Balance\n");  
            sem_wait(mutex);
            int rand_num = rand();
            if (rand_num%2 == 0){
              withdrawMoney(counter_ptr);
            }else{
              printf("Poor Student: Last Checking Balance = $%d\n", *counter_ptr);
            }
            sem_post(mutex);
            
        }
        printf("   Client is about to exit\n");
        exit(0);
    }

    wait(&status);
    printf("Server has detected the completion of its child...\n");
    printf("Server has detached its shared memory...\n");
    printf("Server has removed its shared memory...\n");
    printf("Server exits...\n");
    exit(0);
}

void depositMoney(int* bank_acc){
  int local_balance = *bank_acc;
  int amt = rand() % 101;
  if ((amt%2) == 0){
    local_balance += amt;
    printf("Dear Old Dad: Deposits $%d / Balance = $%d\n", amt, local_balance);
    *bank_acc = local_balance;
  }
  else{
    printf("Dear Old Dad: Doesn't have any money to give\n");
  }
}

void withdrawMoney(int* bank_acc){
  int local_balance = *bank_acc;
  int need = rand() % 51;
  printf("Poor Student needs $%d\n", need);
  if (need <= local_balance){
    local_balance -= need;
    printf("Poor Student: Withdraws $%d / Balance = $%d\n", need, local_balance);
    *bank_acc = local_balance;
  }else{
    ("Poor Student: Not Enough Cash ($%d)\n", local_balance);
  }
}