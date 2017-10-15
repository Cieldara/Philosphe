#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "semaphore.h"
#include <stdio.h>


int nb_philo = 5;
int nb_Grain_De_Riz = 5;
pthread_mutex_t mutexs[5];
sem_t sem;

struct philo
   {
   int identifiant;
};

void initialisation(){
        //Initialisation du semaphore
        srand (time (NULL));
        sem_init(&sem,0,nb_philo-1);
        //Initialisation du tableau de mutexs
        for(int i = 0; i<nb_philo;i++){
                pthread_mutex_init(&mutexs[i], NULL);
        }
}

void parler(int id){
        //sleep(rand() % 5);
        printf("Le philosophe %d philosophe ! :)\n",id);
}

void prendre_baguettes(int id){
        //wait sur le semaphore
        sem_wait(&sem);
        int i;
        sem_getvalue(&sem,&i);
        printf("Value %d\n",i);
        //lock des mutexs;
        pthread_mutex_lock(&mutexs[id]);
        pthread_mutex_lock(&mutexs[(id+1)%nb_philo]);
        printf("Le philosophe %d prend les baguettes %d\n",id, (id+1)%nb_philo);
}

void manger(int id){
        printf("Le philosophe %d mange ! Avec les baguettes %d\n",id, (id+1)%nb_philo);
}


void rendre_baguettes(int id){
        //Unlock des mutexs;
        printf("Le philosophe %d a rendu les baguettes %d\n",id, (id+1)%nb_philo);

        pthread_mutex_unlock(&mutexs[id]);
        pthread_mutex_unlock(&mutexs[(id+1)%nb_philo]);
        sem_post(&sem);
}

//Thread d'un philosophe
void philosophe(void * args){
        struct philo* info = (struct philo*) args;
        int identifiant = info->identifiant;
        for (int i=0;i<nb_Grain_De_Riz; i++){
                parler(identifiant);
                prendre_baguettes(identifiant);
                manger(identifiant);
                rendre_baguettes(identifiant);
        }
}
int main (int argc, char **argv){
        initialisation();
        struct philo* philo;
        pthread_t *tids = malloc (nb_philo * sizeof(pthread_t));
        int i;

        for(i = 0; i < nb_philo; i++){
                philo = (struct philo *) malloc(sizeof(struct philo));
                philo->identifiant = i;
                pthread_create(&tids[i], NULL, (void*)philosophe, philo);
        }

        for(i = 0;i< nb_philo;i++)
                pthread_join(tids[i],NULL);
        return 0;
}
