#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>

#define NB_PHILO 5

int nb_philo = NB_PHILO;
int nb_Grain_De_Riz = 8;
int baguettes_prises[NB_PHILO];
int doitPrioriser;
int aPrioriser;
int toursSansManger[NB_PHILO];
//int nbAMange;
pthread_mutex_t mutexs[NB_PHILO];
pthread_mutex_t global;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

struct philo
   {
   int identifiant;

};

int mod(int a, int b)
{
    int r = a % b;
    return r < 0 ? r + b : r;
}

void initialisation(){
        //Initialisation du mutex global
        pthread_mutex_init(&global, NULL);
        srand (time (NULL));
        aPrioriser = -1;
        doitPrioriser = 0;
        //Initialisation du tableau de mutex et du tableau d'état des fourchettes
        for(int i = 0; i<nb_philo;i++){
                baguettes_prises[i] = 0;
                toursSansManger[i] = 0;
                pthread_mutex_init(&mutexs[i], NULL);
        }
}

void parler(int id){
        //sleep(rand() % 5);
        printf("Le philosophe %d philosophe ! :)\n",id);
}

void prendre_baguettes(int id){
        //prendre le mutex global
        pthread_mutex_lock(&global);
        //boucle pour savoir si les baguettes sont libres
        while(baguettes_prises[id] || baguettes_prises[(id+1)%nb_philo] || (doitPrioriser && (mod(id-1,nb_philo)== aPrioriser || (id-1%nb_philo)== aPrioriser ))/*|| aMange[id]*/){

            pthread_cond_wait(&cond,&global);

            if(!doitPrioriser && ++toursSansManger[id] == 2){
                doitPrioriser = 1;
                aPrioriser = id;
                printf("Famine detectee %d : remedions a cela !\n",id);
            }
        }
        toursSansManger[id] =0;
        doitPrioriser =0;
        aPrioriser = -1;
        printf("Le philosophe %d prend les baguettes %d\n",id, (id+1)%nb_philo);
        //lock des mutexs;
        pthread_mutex_lock(&mutexs[id]);
        pthread_mutex_lock(&mutexs[(id+1)%nb_philo]);
        //MAJ des booleens
        baguettes_prises[id] = 1;
        baguettes_prises[(id+1)%nb_philo] = 1;
        //Relacher le mutex
        pthread_mutex_unlock(&global);
}

void manger(int id){
        printf("Le philosophe %d mange ! Avec les baguettes %d\n",id, (id+1)%nb_philo);
}


void rendre_baguettes(int id){
        //Prendre le mutex global
        pthread_mutex_lock(&global);
        //Unlock des mutexs;
        pthread_mutex_unlock(&mutexs[id]);
        pthread_mutex_unlock(&mutexs[(id+1)%nb_philo]);
        //MAJ des booleens
        baguettes_prises[id] = 0;
        baguettes_prises[(id+1)%nb_philo] = 0;
        printf("Le philosophe %d a rendu les baguettes %d\n",id, (id+1)%nb_philo);
        //Relacher le mutex
        pthread_mutex_unlock(&global);
        //Signaler que des baguettes sont de nouveau libre
        pthread_cond_signal(&cond);
}

//Thread d'un philosophe
void philosophe(void * args){
        struct philo* info = (struct philo*) args;
        int identifiant = info->identifiant;
        for (int i=0; i<nb_Grain_De_Riz; i++){
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


        for(int i = 0; i < nb_philo; i++){
                philo = (struct philo *) malloc(sizeof(struct philo));
                philo->identifiant = i;
                pthread_create(&tids[i], NULL, (void*)philosophe, philo);
        }

        for(int j  = 0;j< nb_philo;j++)
                pthread_join(tids[j],NULL);
        return 0;
}
