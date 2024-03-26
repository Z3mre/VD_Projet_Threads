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

int main(int argc, char* argv[])
{
    int i;

    ouvrirFenetreGraphique();

    pthread_t threadFenetreGraphique;
    pthread_t threadEvenements;
    pthread_t threadStanley;


    // Création du thread fctThreadFenetreGraphique
    int res = pthread_create(&threadFenetreGraphique, NULL, fctThreadFenetreGraphique, NULL);
    if (res != 0) {
        perror("Erreur lors de la création de threadFenetreGraphique");
        exit(EXIT_FAILURE);
    }

    res = pthread_create(&threadEvenements, NULL, fctThreadEvenements, NULL);
    if (res != 0) {
        perror("Erreur lors de la création de threadEvenements");
        exit(EXIT_FAILURE);
    }

    res = pthread_create(&threadStanley, NULL, fctThreadStanley, NULL);
    if (res != 0) {
        perror("Erreur lors de la création de threadStanley");
        exit(EXIT_FAILURE);
    }


    pthread_join(threadStanley,NULL);
    pthread_join(threadEvenements,NULL);

    /*pthread_mutex_lock(&mutexEvenement);

    pthread_cond_wait(&condEvenement,&mutexEvenement);
    pthread_mutex_unlock(&mutexEvenement);*/


    /*for(i = 0; i < 6; i++)
    {
        afficherStanley(HAUT, i, NORMAL);
        afficherStanley(HAUT, i, SPRAY);
    }

    afficherStanley(ECHELLE, 0);
    afficherStanley(ECHELLE, 1);

    for(i = 0; i < 4; i++)
    {
        afficherStanley(BAS, i, NORMAL);
        afficherStanley(BAS, i, SPRAY);
    }

    for(i = 0; i < 5; i++)
    {
        afficherAmi(i, NORMAL);
        afficherAmi(i, TOUCHE);
    }

    for(i = 0; i < 5; i++)
        afficherChenilleG(i);

    for(i = 0; i < 7; i++)
        afficherChenilleD(i);

    for (i = 0; i < 5; i++)
    {
        afficherAraigneeG(i);
        afficherAraigneeD(i);
    }

    for(i = 0; i < 4; i++)
    {
        afficherInsecticideG(i);
        afficherInsecticideD(i + 1);
    }

    afficherGuepe(0);
    afficherGuepe(1);

    afficherEchecs(3);

    afficherScore(0);

    actualiserFenetreGraphique();
    while(1)
    {
        int event = lireEvenement();

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
        }
    }*/

    return 0;
}

//**************************************************************************************
void* fctThreadFenetreGraphique(void*)
{
    while(true)
    {
        restaurerImageInterne(); // Restaure l'image initiale du jeu en mémoire
        
        // Affichage des personnages, du score et des échecs dans l'image interne du jeu
        for(int i = 0; i < 5; i++)
        {
            afficherAmi(i, etatJeu.etatAmis[i]);
        }

        afficherStanley(BAS, etatJeu.etatStanley, NORMAL); 

        afficherEchecs(etatJeu.nbEchecs);
        afficherScore(etatJeu.score);

        actualiserFenetreGraphique(); // Rendre visible l'image interne du jeu

        // Attendre 0,1 seconde
        struct timespec attente = {0, 100000000}; // 0,1 seconde en nanosecondes
        nanosleep(&attente, NULL);
    }
    pthread_exit(0);
}

void *fctThreadEvenements(void *)
{
    while(1)
    {
        evenement = lireEvenement();

        printf("Mise en attente de ThreadEvenements\n");
        pthread_mutex_lock(&mutexEvenement);

        printf("ThreadEvenements signale un événement à ThreadStanley...\n");
        pthread_cond_signal(&condEvenement); // Réveiller ThreadStanley

        printf("Réveil de ThreadEvenements\n");
        pthread_mutex_unlock(&mutexEvenement);

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
    } 

    struct timespec attente = {0, 100000000}; // 0,1 seconde en nanosecondes
    nanosleep(&attente, NULL);

    return NULL;
}


/*void *fctThreadStanley(void *)
{
    pthread_mutex_lock(&mutexEvenement);
    printf("Thread Stanley attend un événement...\n");
    pthread_cond_wait(&condEvenement, &mutexEvenement); // Mettre en attente sur la variable de condition
    printf("Thread Stanley réveillé par ThreadEvenements, événement reçu : %d\n", evenement);
    pthread_mutex_unlock(&mutexEvenement);

    return NULL;
}*/

void* fctThreadStanley(void*)
{
    while(true)
    {
printf("\n\t pos stnl = %d\n",etatJeu.positionStanley);
        pthread_mutex_lock(&mutexEvenement);
        pthread_cond_wait(&condEvenement, &mutexEvenement); // Mettre en attente sur la variable de condition
        pthread_mutex_lock(&mutexEtatJeu);  
        switch(etatJeu.etatStanley)
        {
            case BAS:
                switch(evenement)
                {
                    case SDLK_SPACE:
                        if(etatJeu.positionStanley == 0)
                        {
                            etatJeu.actionStanley = SPRAY;
                            pthread_mutex_unlock(&mutexEtatJeu);
                            
                            struct timespec attente = {0, 200000000}; // 0,1 seconde en nanosecondes
                            nanosleep(&attente, NULL);

                            pthread_mutex_lock(&mutexEtatJeu);
                            etatJeu.actionStanley = NORMAL;
                        }
                
                        break;

                    case SDLK_LEFT:
                        if(etatJeu.positionStanley > 0)
                            etatJeu.positionStanley--;
                            afficherStanley(BAS, etatJeu.positionStanley, NORMAL);
                        break;
                    
                    case SDLK_RIGHT:
                        if(etatJeu.positionStanley < 3)
                            etatJeu.positionStanley++;
                            afficherStanley(BAS, etatJeu.positionStanley, NORMAL);
                        break;
                }
                break;

            case ECHELLE:
                break;

            case HAUT:
                break;
        }

        pthread_mutex_unlock(&mutexEtatJeu);

        evenement = AUCUN;

        pthread_mutex_unlock(&mutexEvenement);
    }

    pthread_exit(0);
}