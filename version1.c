#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>




int nb_philo;
int nb_Grain_De_Riz = 8;
int * baguettes_prises;
//Si on doit prioriser un thread
int doitPrioriser;
//Le thread a prioriser
int aPrioriser;
//Nombre de fois ou un philosophe a été réveillé sans pouvoir manger
int * toursSansManger;
//int nbAMange;
pthread_mutex_t * mutexs;
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
	baguettes_prises = malloc(nb_philo*sizeof(int));
	toursSansManger = malloc(nb_philo*sizeof(int));
	mutexs = malloc(nb_philo*sizeof(pthread_mutex_t));
	
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
        
        printf("Le philosophe %d philosophe ! :)\n",id);
	//sleep(rand() % 5);
}

void prendre_baguettes(int id){
        //prendre le mutex global
        pthread_mutex_lock(&global);
        //boucle pour savoir si les baguettes sont libres ou si on doit laisser la main a notre voisin
        while( baguettes_prises[id] || baguettes_prises[(id+1)%nb_philo] || (doitPrioriser && (mod(id-1,nb_philo)== aPrioriser || (id+1%nb_philo)== aPrioriser ))/*|| aMange[id]*/){
            pthread_cond_wait(&cond,&global);

	    //Si le philosophe n'a pas pu manger ses baguettes pendant X tours : le prioriser
            if(!doitPrioriser && ++toursSansManger[id] == 2){
                doitPrioriser = 1;
                aPrioriser = id;
                printf("Famine detectee le philosophe %d a faim: remedions a cela !\n",id);
		
            }
	    //Signal pour éviter d'avoir un interblocage
	    pthread_cond_signal(&cond);
        }
        toursSansManger[id] =0;
        //Si nous etions le prioritaire
        if(id == aPrioriser){
            doitPrioriser =0;
            aPrioriser = -1;
        }
        printf("Le philosophe %d prend les baguettes %d et %d\n",id, id,(id+1)%nb_philo);
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
        printf("Le philosophe %d mange ! Avec les baguettes %d et %d\n", id, id, (id+1)%nb_philo);
	//sleep(rand() % 5);
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
        printf("Le philosophe %d a rendu les baguettes %d et %d\n", id, id, (id+1)%nb_philo);
        //Relacher le mutex
        pthread_mutex_unlock(&global);
        //Signaler que des baguettes sont de nouveau libre
        pthread_cond_signal(&cond);
}

//Thread d'un philosophe
void philosophe(void * args){
        struct philo* info = (struct philo*) args;
        int identifiant = info->identifiant;
        for (int i=0;i<nb_Grain_De_Riz ; i++){
                parler(identifiant);
                prendre_baguettes(identifiant);
                manger(identifiant);
                rendre_baguettes(identifiant);
        }
}




int main (int argc, char **argv){

	if(argc != 2){
		printf("Utilisation : ./version1 <nb_philosophes>\n");
		return 1;
	}
	nb_philo = atoi(argv[1]);
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
