#include "./presentation/presentation.h"

void *fctThreadFenetreGraphique(void *);
void *fctThreadEvenements(void *);
void *fctThreadStanley(void *);
void *fctThreadEnnemis(void *);
void *fctThreadGuepe(void *);
void *fctThreadChenilleG(void *);
void *fctThreadChenilleD(void *);
void *fctThreadAraigneeG(void *);
void *fctThreadAraigneeD(void *);
void *fctThreadInsecticideG(void *);
void *fctThreadInsecticideD(void *);

void handlerSIGINT(int);
void handlerSIGALRM(int);
void handlerSIGUSR1(int);
void handlerSIGUSR2(int);
void handlerSIGQUIT(int); 

void destructeurVS(void *p);

pthread_cond_t condEvenement ;
pthread_cond_t condEchec;

pthread_key_t keySpec;

pthread_mutex_t mutexEtatJeu;
pthread_mutex_t mutexEvenement;
pthread_mutex_t mutexEchec;

typedef struct
{
    int presence;
    pthread_t tid;
} S_PRESENCE; 

typedef struct
{
    int etatStanley;
    int positionStanley;
    int actionStanley;
    int etatAmis[5];
    S_PRESENCE guepes[2];
    S_PRESENCE chenillesG[5];
    S_PRESENCE chenillesD[7];
    S_PRESENCE araigneesG[5];
    S_PRESENCE araigneesD[5];
    S_PRESENCE insecticidesG[4];
    S_PRESENCE insecticidesD[4];
    int score;
    int nbEchecs;
} S_ETAT_JEU;

S_ETAT_JEU etatJeu = 
     { BAS, 1, NORMAL,
       { NORMAL, NORMAL, NORMAL, NORMAL, NORMAL },
       { { AUCUN, 0 }, { AUCUN, 0 } },
       { { AUCUN, 0 }, { AUCUN, 0 }, { AUCUN, 0 }, { AUCUN, 0 }, { AUCUN, 0 } },
       { { AUCUN, 0 }, { AUCUN, 0 }, { AUCUN, 0 }, { AUCUN, 0 }, { AUCUN, 0 }, 
         { AUCUN, 0 }, { AUCUN, 0 } },
       { { AUCUN, 0 }, { AUCUN, 0 }, { AUCUN, 0 }, { AUCUN, 0 }, { AUCUN, 0 } },
       { { AUCUN, 0 }, { AUCUN, 0 }, { AUCUN, 0 }, { AUCUN, 0 }, { AUCUN, 0 } },
       { { AUCUN, 0 }, { AUCUN, 0 }, { AUCUN, 0 }, { AUCUN, 0 } },
       { { AUCUN, 0 }, { AUCUN, 0 }, { AUCUN, 0 }, { AUCUN, 0 } },
       0, 0 };

int evenement = AUCUN; 
int echec = AUCUN;

struct sigaction sigAct;
sigset_t sigpro;

int DELAI = 1600000000; // 1,6 seconde en nanoseconde


int main(int argc, char* argv[])
{
    int i;


    ouvrirFenetreGraphique();

    sigAct.sa_handler = handlerSIGALRM;
	sigaction(SIGALRM,&sigAct,NULL);
    
    pthread_mutex_init(&mutexEvenement,NULL);

    pthread_cond_init(&condEvenement,NULL);


    pthread_t threadFenetreGraphique;
    pthread_t threadEvenements;
    pthread_t threadStanley;
    pthread_t threadEnnemis;


    // Création des threads
    printf("main : Création du threadFenetreGraphique \n");
    int res = pthread_create(&threadFenetreGraphique, NULL, fctThreadFenetreGraphique, NULL);
    if (res != 0) {
        perror("Erreur lors de la création de threadFenetreGraphique");
        exit(EXIT_FAILURE);
    }

    printf("main : Création du threadEvenements \n");
    res = pthread_create(&threadEvenements, NULL, fctThreadEvenements, NULL);
    if (res != 0) {
        perror("Erreur lors de la création de threadEvenements");
        exit(EXIT_FAILURE);
    }

    printf("main : Création du threadStanley \n");
    res = pthread_create(&threadStanley, NULL, fctThreadStanley, NULL);
    if (res != 0) {
        perror("Erreur lors de la création de threadStanley");
        exit(EXIT_FAILURE);
    }

    pthread_key_create(&keySpec, NULL);
    //pthread_setspecific(keySpec, &DELAI);

    printf("main : Création du threadEnnemis \n");
    res = pthread_create(&threadEnnemis, NULL, fctThreadEnnemis, NULL);
    if (res != 0) {
        perror("Erreur lors de la création de threadEnnemis");
        exit(EXIT_FAILURE);
    }


    // Attente de terminaison
    printf("main : Attente du threadEvenements \n");
    pthread_join(threadEvenements,NULL);

    destructeurVS(&keySpec);

    return 0;
}



void* fctThreadFenetreGraphique(void*)
{
    while(true)
    {
        restaurerImageInterne(); // Restaure l'image initiale du jeu en mémoire
        
        afficherStanley(etatJeu.etatStanley, etatJeu.positionStanley, etatJeu.actionStanley); 

        // Affichage des personnages, du score et des échecs dans l'image interne du jeu
        for(int i = 0; i < 5; i++)
        {
            afficherAmi(i, etatJeu.etatAmis[i]);
        }

        /*for(int i = 0; i < 5; i++)
        {
            afficherChenilleG(i);
            afficherChenilleD(i);
            afficherAraigneeG(i);
            afficherAraigneeD(i);
        }

        for(int i = 0; i < 4; i++)
        {
            afficherInsecticideG(i);
            afficherInsecticideD(i + 1);
        }*/

        for(int i = 0; i < 2; i++)
        {
            afficherGuepe(i);
        }

        afficherEchecs(etatJeu.nbEchecs);
        afficherScore(etatJeu.score);
        

        actualiserFenetreGraphique(); // Rendre visible l'image interne du jeu


        // Attendre 0,1 seconde
        struct timespec attente = {0, 100000000}; // 0,1 seconde en nanosecondes
        nanosleep(&attente, NULL);
    }

    printf("fctThreadFenetreGraphique : Fin du thread \n");
    pthread_exit(0);
}

void *fctThreadEvenements(void *)
{
    while(1)
    {
        evenement = lireEvenement();

        printf("fctThreadEvenements : Verrouillage du mutexEvenement \n");
        pthread_mutex_lock(&mutexEvenement);

        switch(evenement)
        {
            case SDL_QUIT:
                printf("QUIT\n");
                exit(0);

            case SDLK_UP:
                printf("KEY_UP\n");
                break;

            case SDLK_DOWN:
                printf("KEY_DOWN\n");
                break;

            case SDLK_LEFT:
                printf("KEY_LEFT\n");
                break;

            case SDLK_RIGHT:
                printf("KEY_RIGHT\n");
                break;

            case SDLK_SPACE:
                printf("SDLK_SPACE\n");
                break;

            default:
                printf("AUCUN");
                evenement = AUCUN; 
                break;
        }

        printf("fctThreadEvenements : Envoie d'un signal à un thread en attente sur condEvenement \n");
        pthread_cond_signal(&condEvenement); // Réveiller ThreadStanley
        
        struct timespec attente = {0, 100000000}; // 0,1 seconde en nanosecondes
        nanosleep(&attente, NULL);

        printf("fctThreadEvenements : Déverrouillage du mutexEvenement \n");
        pthread_mutex_unlock(&mutexEvenement);
    } 

    printf("fctThreadEvenements : Fin du thread \n");
    pthread_exit(0);
}

void* fctThreadStanley(void*)
{
    while(true)
    {
        printf("fctThreadStanley : Verrouillage du mutexEvenement \n");
        pthread_mutex_lock(&mutexEvenement);

        printf("fctThreadStanley : Thread en attente de condEvenement et du verrouillage de mutexEvenement \n");
        pthread_cond_wait(&condEvenement, &mutexEvenement); // Mettre en attente sur la variable de condition

        printf("fctThreadStanley : Verrouillage du mutexEtatJeu \n");
        pthread_mutex_lock(&mutexEtatJeu);  

        switch(etatJeu.etatStanley)
        {
            case BAS:
                switch(evenement)
                {
                    case SDLK_SPACE:
                        if(etatJeu.positionStanley != 1)
                        {
                            etatJeu.actionStanley = SPRAY;

                            printf("fctThreadStanley : Déverrouillage du mutexEtatJeu \n");
                            pthread_mutex_unlock(&mutexEtatJeu);
                            
                            struct timespec attente = {0, 200000000}; // 0,2 seconde en nanosecondes
                            nanosleep(&attente, NULL);

                            printf("fctThreadStanley : Verrouillage du mutexEtatJeu \n");
                            pthread_mutex_lock(&mutexEtatJeu);

                            etatJeu.actionStanley = NORMAL;
                        }
                        break;

                    case SDLK_LEFT:
                        if(etatJeu.positionStanley > 0)
                        {
                            etatJeu.positionStanley--;
                        }
                        break;
                    
                    case SDLK_RIGHT:
                        if(etatJeu.positionStanley < 3)
                        {
                            etatJeu.positionStanley++;
                        }
                        break;

                    case SDLK_UP:
                        if(etatJeu.positionStanley == 1)
                        {
                            etatJeu.etatStanley = ECHELLE;
                            etatJeu.positionStanley = 1;
                        }
                        break;
                }
                break;

            case ECHELLE:
                switch(evenement)
                {
                    case SDLK_UP:
                        if(etatJeu.positionStanley == 1)
                        {
                            etatJeu.positionStanley = 0;
                        }
                        else if (etatJeu.positionStanley == 0)
                        {
                            etatJeu.etatStanley = HAUT;
                            etatJeu.positionStanley = 2;
                        }
                        break;

                    case SDLK_DOWN:
                        if(etatJeu.positionStanley == 0)
                        {
                            etatJeu.positionStanley = 1;
                        }
                        else if (etatJeu.positionStanley == 1)
                        {
                            etatJeu.etatStanley = BAS;
                            etatJeu.positionStanley = 1;
                        }
                        break;
                }
                break;

            case HAUT:
                switch(evenement)
                {
                    case SDLK_SPACE:
                        if(etatJeu.positionStanley != 2)
                        {
                            etatJeu.actionStanley = SPRAY;

                            printf("fctThreadStanley : Déverrouillage du mutexEtatJeu \n");
                            pthread_mutex_unlock(&mutexEtatJeu);
                            
                            struct timespec attente = {0, 200000000}; // 0,2 seconde en nanosecondes
                            nanosleep(&attente, NULL);

                            printf("fctThreadStanley : Verrouillage du mutexEtatJeu \n");
                            pthread_mutex_lock(&mutexEtatJeu);

                            etatJeu.actionStanley = NORMAL;
                        }
                        break;

                    case SDLK_LEFT:
                        if(etatJeu.positionStanley > 0)
                        {
                            etatJeu.positionStanley--;
                        }
                        break;
                    
                    case SDLK_RIGHT:
                        if(etatJeu.positionStanley < 5)
                        {
                            etatJeu.positionStanley++;
                        }
                        break;

                    case SDLK_DOWN:
                        if(etatJeu.positionStanley == 2)
                        {
                            etatJeu.etatStanley = ECHELLE;
                            etatJeu.positionStanley = 0;
                        }
                        break;
                }
                break;
        }

        printf("fctThreadStanley : Déverrouillage du mutexEtatJeu \n");
        pthread_mutex_unlock(&mutexEtatJeu);

        evenement = AUCUN;

        printf("fctThreadStanley : Déverrouillage du mutexEvenement \n");
        pthread_mutex_unlock(&mutexEvenement);
    }

    printf("fctThreadStanley : Fin du thread \n");
    pthread_exit(0);
}

void* fctThreadEnnemis(void*)
{
    pthread_t threadGuepe;
    pthread_t threadChenilleG;
    pthread_t threadChenilleD;
    pthread_t threadAraigneeG;
    pthread_t threadAraigneeD;

    struct timespec temps;

    int res,typeEnnemi;

    sigemptyset(&sigpro); // Initialisation de l'ensemble de signaux
    sigaddset(&sigpro, SIGALRM); // Ajout du signal SIGALRM à l'ensemble
    sigprocmask(SIG_SETMASK, &sigpro, NULL); // seul SIGALARM est autorisé par le processus

    alarm(5);

    while(true)
    {
        int* delai = &DELAI;
        pthread_setspecific(keySpec, delai);

        temps.tv_sec = *delai/1000000000;
		temps.tv_nsec =  *delai % 1000000000;
        nanosleep(&temps,NULL);
        srand(time(NULL));
        typeEnnemi = rand()%5;
        
        printf("\t DEBUG %d\n",*delai);
        

        switch(typeEnnemi)
        {
            case GUEPE:
                printf("fctThreadEnnemis : Création du threadGuepe \n");
                res = pthread_create(&threadGuepe, NULL, fctThreadGuepe, NULL);
                if (res != 0) {
                    perror("Erreur lors de la création de threadGuepe");
                    exit(EXIT_FAILURE);
                }
                break;

            /*case CHENILLE_G:
                printf("fctThreadEnnemis : Création du threadChenilleG \n");
                res = pthread_create(&threadChenilleG, NULL, fctThreadChenilleG, NULL);
                if (res != 0) {
                    perror("Erreur lors de la création de threadChenilleG");
                    exit(EXIT_FAILURE);
                }
                break;

            case CHENILLE_D:
                printf("fctThreadEnnemis : Création du threadChenilleD \n");
                res = pthread_create(&threadChenilleD, NULL, fctThreadChenilleD, NULL);
                if (res != 0) {
                    perror("Erreur lors de la création de threadChenilleD");
                    exit(EXIT_FAILURE);
                }
                break;

            case ARAIGNEE_G:
                printf("fctThreadEnnemis : Création du threadAraigneeG \n");
                res = pthread_create(&threadAraigneeG, NULL, fctThreadAraigneeG, NULL);
                if (res != 0) {
                    perror("Erreur lors de la création de threadAraigneeG");
                    exit(EXIT_FAILURE);
                }
                break;

            case ARAIGNEE_D:
                printf("fctThreadEnnemis : Création du threadAraigneeD \n");
                res = pthread_create(&threadAraigneeD, NULL, fctThreadAraigneeD, NULL);
                if (res != 0) {
                    perror("Erreur lors de la création de threadAraigneeD");
                    exit(EXIT_FAILURE);
                }
                break;*/
        }
    }

    printf("fctThreadEnnemis : Fin du thread \n");
    pthread_exit(0);
}

void* fctThreadGuepe(void*)
{
    /*struct sigaction sa;
	sa.sa_handler = HandlerSIGUSR1;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sigaction(SIGUSR1, &sa, NULL);*/

    int* position = (int*)pthread_getspecific(keySpec);
    if(position == NULL)
    {
        *position=0;
        pthread_setspecific(keySpec, position);
    }
    

    etatJeu.guepes[0].presence = NORMAL;
    etatJeu.guepes[0].tid = pthread_self();

    sleep(1);

    etatJeu.guepes[0].presence = AUCUN;
    etatJeu.guepes[0].tid = 0;

    etatJeu.guepes[1].presence = NORMAL;
    etatJeu.guepes[1].tid = pthread_self();

    printf("fctThreadGuepe : Fin du thread \n");
    pthread_exit(0);
}

void* fctThreadChenilleG(void*)
{
    printf("fctThreadChenilleG : Fin du thread \n");
    pthread_exit(0);
}

void* fctThreadChenilleD(void*)
{
    printf("fctThreadChenilleD : Fin du thread \n");
    pthread_exit(0);
}

void* fctThreadAraigneeG(void*)
{
    printf("fctThreadAraigneeG : Fin du thread \n");
    pthread_exit(0);
}

void* fctThreadAraigneeD(void*)
{
    printf("fctThreadAraigneeD : Fin du thread \n");
    pthread_exit(0);
}



void handlerSIGALRM(int sign)
{
    DELAI = rand() % 500000001 + 1100000000;

    alarm(5);
}

void destructeurVS(void *p)
{
    free(p);
}