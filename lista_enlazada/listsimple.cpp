#include<iostream>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <chrono>
#include <iomanip>
#include <time.h>
/*
compiler: g++ -pthread -o list listsimple.cpp
execute: ./list
*/
using namespace std; 
struct list_node_s {
    int data;
    struct list_node_s *next;
};
/*****************************************************************************************/
pthread_mutex_t mutex;
pthread_mutex_t count_mutex;
int count_member = 0;
int count_insert = 0;
int count_delete = 0;
int max_key=100000;
int size_;
pthread_t *threads;
struct list_node_s* head=NULL;
/*shared */
int N;//cantidad de elementos
float porcent_insert;
float porcent_member;
float porcent_delete;
/**********************************************************************************/
/*function*/
void Usage(char* prog_name) {
   fprintf(stderr, "usage: %s <size_>\n", prog_name);
   exit(0);
} 
int Member(int value, struct list_node_s*head) {
    struct list_node_s *curr_p = head;
    while (curr_p!= NULL && curr_p->data<value)
        curr_p = curr_p->next;
    if (curr_p == NULL ||curr_p->data > value) {
        return 0;} 
    else {
        return 1;
    }
} /* Member */
int Insert(int value, struct list_node_s** head) {
    struct list_node_s* curr_p = *head;
    struct list_node_s* pred_p = NULL;
    struct list_node_s* temp;
    while (curr_p != NULL && curr_p->data < value) {
        pred_p = curr_p;
        curr_p = curr_p->next;
    }

    if (curr_p == NULL || curr_p->data > value) {
        temp = new list_node_s();//malloc(sizeof(struct list_node_s)); 
        temp->data = value;
        temp->next = curr_p;
        
        if (pred_p == NULL) /* New first node */
            *head = temp;
        else
            pred_p->next = temp;
        return 1;
    } 
    else 
        return 0;
    
} /* Insert */
int Delete(int value, struct list_node_s** head) {
    struct list_node_s* curr_p = *head;
    struct list_node_s* pred_p = NULL;

    while (curr_p != NULL && curr_p->data<value) {
        pred_p = curr_p;
        curr_p = curr_p->next;
    }
    if (curr_p != NULL && curr_p->data == value) {
        if (pred_p == NULL) { /* Deleting first node in list */
            *head = curr_p->next;
            free(curr_p);
        } else {
            pred_p->next = curr_p->next;
            free(curr_p);
        }
        return 1;
    } else { /* Value isn’t in list */
         return 0;
    }
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
            //cout<<"Thread Searching for "<<my_rank<<"  "<< value;
            pthread_mutex_lock(&mutex);
            Member(value, head);
            pthread_mutex_unlock(&mutex);
            operation_member++;
        } else if (select_operation < porcent_member + porcent_insert) {
            //cout<<"Thread  to insert "<<my_rank<<"  "<< value;
            pthread_mutex_lock(&mutex);
            Insert(value, &head);
            pthread_mutex_unlock(&mutex);
            operation_insert++;
        } else { /* delete */
            //cout<<"Thread  to delete "<< my_rank<<"  "<< value;
            pthread_mutex_lock(&mutex);
            Delete(value, &head);
            pthread_mutex_unlock(&mutex);
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

int main(int argc, char* argv[]){
    
    /*Insert(2,&head);
    Insert(4,&head);
    Insert(10,&head);
    print(&head);*/
    int data_n;
    cout<<"*ingrese la cantidad e threads\n * Ingrese  el numero de key para inicializar,\n * inserta la cantidad de operaciones \n* ingresa los porcentajes de insert, member, delete "<<endl;
    cin>>size_>>data_n>>N>>porcent_insert>>porcent_member>>porcent_delete;
    if(N<=0 ){
        cout<<"valores invalidos "<<endl;
        exit(0);
    }
    for(int i=0;i<data_n;i++){
        int r= rand()%max_key;
        if(!Insert(r,&head)){
            i--;
        }
    }
    threads = new pthread_t[size_];
    pthread_mutex_init(&mutex, NULL);
	pthread_mutex_init(&count_mutex, NULL);
    auto start_time= chrono::high_resolution_clock::now();
    //auto start_time=clock();
    for(int i=0;i<size_;i++){
        pthread_create(&threads[i],NULL,myfunction,(void *)i);
    }
    for(int i=0;i<size_;i++){
        pthread_join(threads[i],NULL);
    }
    auto end_time=chrono::high_resolution_clock::now();
    //auto end_time=clock();
    pthread_mutex_destroy(&mutex);
	pthread_mutex_destroy(&count_mutex);
    cout<<setprecision(5)<<endl;
    cout<<"time: "<<chrono::duration_cast<std::chrono::seconds>(end_time-start_time).count()<<endl;
    //cout<<"time: "<<(end_time-start_time)/CLOCKS_PER_SEC<<endl;
    free(threads);
    return 0;
}
