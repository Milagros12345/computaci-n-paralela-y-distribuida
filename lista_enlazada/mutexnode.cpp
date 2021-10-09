#include<iostream>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <chrono>
#include <iomanip>
using namespace std;
struct list_node_s {
    int data;
    struct list_node_s *next;
    pthread_mutex_t mutex;
};
/********************************************************************************/
pthread_mutex_t head_mutex;
pthread_mutex_t count_mutex;
int count_member = 0;
int count_insert = 0;
int count_delete = 0;
int max_key=100000;
int size_;
pthread_t *threads;
struct list_node_s* head=NULL;
int N;//cantidad de elementos
float porcent_insert;
float porcent_member;
float porcent_delete;
/*********************************************************************************/
//compile g++ -std=c++0x -pthread -o thread thread.cpp;./thread
int Member(int value) {
    struct list_node_s* temp;
    pthread_mutex_lock(&head_mutex);
    temp = head;
    while (temp!= NULL && temp->data < value) {
        if (temp->next != NULL)
            pthread_mutex_lock(&(temp->next->mutex));
        if (temp == head)
            pthread_mutex_unlock(&head_mutex);
        pthread_mutex_unlock(&(temp->mutex));
        temp= temp->next;
    }
    if (temp== NULL || temp->data > value) {
        if (temp== head)
            pthread_mutex_unlock(&head_mutex);
        if (temp!= NULL)
            pthread_mutex_unlock(&(temp->mutex));
        return 0;
    } else {
        if (temp == head)
            pthread_mutex_unlock(&head_mutex);
        pthread_mutex_unlock(&(temp->mutex));
    return 1;
    }
} /*Member */
int Advance_ptrs(struct list_node_s** curr_pp, struct list_node_s** pred_pp) {
   int rv = 1;
   struct list_node_s* curr_p = *curr_pp;
   struct list_node_s* pred_p = *pred_pp;
   if (curr_p == NULL)
      if (pred_p == NULL)
         return -1;
      else 
         return 0;
   else { // *curr_pp != NULL
      if (curr_p->next != NULL)
         pthread_mutex_lock(&(curr_p->next->mutex));
      else
         rv = 0;
      if (pred_p != NULL)
         pthread_mutex_unlock(&(pred_p->mutex));
      else
         pthread_mutex_unlock(&head_mutex);
      *pred_pp = curr_p;
      *curr_pp = curr_p->next;
      return rv;
   }
}  /* Advance_ptrs */
int Insert(int value) {
    struct list_node_s* curr_p=head;
    struct list_node_s* pred_p = NULL;
    struct list_node_s* temp;
    pthread_mutex_lock(&head_mutex);
    if (head!= NULL)
      pthread_mutex_lock(&(head->mutex));
    int rv=1;
    while (curr_p != NULL && curr_p->data < value) {
        Advance_ptrs(&curr_p, &pred_p);
    }
    if (curr_p == NULL || curr_p->data > value) {
        temp = new list_node_s();//malloc(sizeof(struct list_node_s)); 
        pthread_mutex_init(&(temp->mutex), NULL);
        temp->data = value;
        temp->next = curr_p;
        if (curr_p!= NULL) 
            pthread_mutex_unlock(&(curr_p->mutex));
        if (pred_p == NULL){ /* New first node */
            head = temp;
            pthread_mutex_unlock(&head_mutex);
        }
        else{
            pred_p->next = temp;
            pthread_mutex_unlock(&(pred_p->mutex));
        }
    } 
    else {
        if (curr_p!= NULL) 
            pthread_mutex_unlock(&(curr_p->mutex));
      if (pred_p!= NULL)
            pthread_mutex_unlock(&(pred_p->mutex));
      else
            pthread_mutex_unlock(&head_mutex);
      rv = 0;
    }
    return rv;
} /* Insert */
int Delete(int value) {
    struct list_node_s* curr_p = head;
    struct list_node_s* pred_p = NULL;
    pthread_mutex_lock(&head_mutex);
    if (head!= NULL)
        pthread_mutex_lock(&(head->mutex));
    int rv = 1;
    while (curr_p != NULL && curr_p->data<value) {
        Advance_ptrs(&curr_p, &pred_p);
    }
    if (curr_p != NULL && curr_p->data == value) {
        if (pred_p == NULL) { /* Deleting first node in list */
            head = curr_p->next;
            pthread_mutex_unlock(&head_mutex);
            pthread_mutex_unlock(&(curr_p->mutex));
            pthread_mutex_destroy(&(curr_p->mutex));
            free(curr_p);
        } else {
            pred_p->next = curr_p->next;
            pthread_mutex_unlock(&(pred_p->mutex));
            pthread_mutex_unlock(&(curr_p->mutex));
            pthread_mutex_destroy(&(curr_p->mutex));
            free(curr_p);
        }
    } else { /* Value isn’t in list */
        if (pred_p!= NULL)
            pthread_mutex_unlock(&(pred_p->mutex));
        if (curr_p!= NULL)
            pthread_mutex_unlock(&(curr_p->mutex));
        if (curr_p== head)
            pthread_mutex_unlock(&head_mutex);
      rv = 0;
    }
    return rv;
 } /* Delete */
void print(struct list_node_s** head){
    struct list_node_s* d=*head;
    while(d!=NULL){
        cout<<d->data<<" ";
        d=d->next;
    }cout<<endl;
}
void *myfunction(void* rank){
    long my_rank = (long) rank;
    int operation_member=0;
    int operation_insert=0;
    int operation_delete=0;
    int value;
    int ops_per_thread=N/size_;
    double select_operation;
    for (int i = 0; i < ops_per_thread; i++) {
        select_operation = rand()%10000/10000.0;
        value = rand()% max_key;
        if (select_operation < porcent_member) {
            //cout<<"Thread  member"<<my_rank<<"  " value<<endl;
            Member(value);
            operation_member++;
        } else if (select_operation < porcent_member + porcent_insert) {
            //cout<<"Thread to insert "<<my_rank<<value<<endl;
            Insert(value);
            operation_insert++;
        } else { /* delete */
            //cout<<"Thread to delete "<<my_rank<<"  "<< value<<endl;
            Delete(value);
            operation_delete++;
      }
   }  /* for */

   pthread_mutex_lock(&count_mutex);
   count_member += operation_member;
   count_insert += operation_insert;
   count_delete += operation_delete;
   pthread_mutex_unlock(&count_mutex);

   return NULL;
}
int main(int argc, char* argv[]) {
    int data_n;
    cout<<"*ingrese la cantidad e threads\n * Ingrese  el numero de key para inicializar,\n * inserta la cantidad de operaciones \n* ingresa los porcentajes de insert, member, delete "<<endl;
    cin>>size_>>data_n>>N>>porcent_insert>>porcent_member>>porcent_delete;
   //size_= strtol(argv[1], NULL);
   pthread_mutex_init(&head_mutex, NULL);
   for(int i=0;i<data_n;i++){
        int r= rand()%max_key;
        if(!Insert(r)){
            i--;
        }
    }
   threads = new pthread_t[size_];
   pthread_mutex_init(&count_mutex, NULL);

   auto start_time= chrono::high_resolution_clock::now();
   for (int i = 0; i < size_; i++)
        pthread_create(&threads[i],NULL,myfunction,(void *)i);

   for(int i=0;i<size_;i++){
        pthread_join(threads[i],NULL);
    }
   auto end_time=chrono::high_resolution_clock::now();
   cout<<setprecision(5)<<endl;
   cout<<"time: "<<chrono::duration_cast<std::chrono::milliseconds>(end_time-start_time).count()<<endl;
   pthread_mutex_destroy(&head_mutex);
   pthread_mutex_destroy(&count_mutex);
   free(threads);

   return 0;
}  /* main */
