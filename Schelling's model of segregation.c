#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

//Struttura usata per la rappresentazione delle informazioni basilari di una matrice
typedef struct
{
    int my_dim_x;
    int my_dim_y;
    int my_age_x;
    int my_age_y;
    int sod;
} info_mat;
//Strttura usata per salvare la posizione degli insodisfatti
typedef struct
{
    int x;
    int y;
} insodisfatti;
//Struttura di supporto per lo scambio di agenti
typedef struct
{
    int qnt_ins;
    int celle_libere;
} richiesta;
//Questa funzione è usata per trovare una cella casuale all'intenro di una matrice passata
void assegna(int **mat, int agente, int dim_x, int dim_y)
{
    int assegnato = 0;
    do
    {
        int i = rand() % dim_x, j = rand() % dim_y;
        if (mat[i][j] == 1000)
        {
            mat[i][j] = agente;
            assegnato = 1;
        }
    } while (assegnato != 1);
}
//Funzione di stampa della matrice dove i valori usati per la rappresentazione vengono trasformati nei dati del problema
void stampa_matrice(int **mat, int dim_x, int dim_y, int myrank)
{
    for (int i = 0; i < dim_x; i++)
    {
        printf("sono %d ->", myrank);
        for (int j = 0; j < dim_y; j++)
        {
            if (mat[i][j] == 1000)
                printf(" v ");
            else
                printf(" %d ", mat[i][j]);
        }
        printf("\n");
    }
}
//Funzione per il calcolo della soddisfazione del margine di una matrice, dove passando il lato di riferimento è
//in grado di calcolare la soddisfazione
int margine(int **mat, int i, int j, char cond)
{
    int sod = 0;
    switch (cond)
    {
    case 's':
        if (mat[i - 1][j] != !mat[i][j])
            sod = sod + 20;
        if (mat[i - 1][j + 1] != !mat[i][j])
            sod = sod + 20;
        if (mat[i][j + 1] != !mat[i][j])
            sod = sod + 20;
        if (mat[i + 1][j] != !mat[i][j])
            sod = sod + 20;
        if (mat[i + 1][j + 1] != !mat[i][j])
            sod = sod + 20;
        break;
    case 'd':
        if (mat[i - 1][j] != !mat[i][j])
            sod = sod + 20;
        if (mat[i - 1][j - 1] != !mat[i][j])
            sod = sod + 20;
        if (mat[i][j - 1] != !mat[i][j])
            sod = sod + 20;
        if (mat[i + 1][j - 1] != !mat[i][j])
            sod = sod + 20;
        if (mat[i + 1][j] != !mat[i][j])
            sod = sod + 20;
        break;
    case 'a':
        if (mat[i][j - 1] != !mat[i][j])
            sod = sod + 20;
        if (mat[i][j + 1] != !mat[i][j])
            sod = sod + 20;
        if (mat[i + 1][j - 1] != !mat[i][j])
            sod = sod + 20;
        if (mat[i + 1][j] != !mat[i][j])
            sod = sod + 20;
        if (mat[i + 1][j - 1] != !mat[i][j])
            sod = sod + 20;
        break;
    case 'b':
        if (mat[i - 1][j] != !mat[i][j])
            sod = sod + 20;
        if (mat[i - 1][j + 1] != !mat[i][j])
            sod = sod + 20;
        if (mat[i - 1][j - 1] != !mat[i][j])
            sod = sod + 20;
        if (mat[i][j - 1] != !mat[i][j])
            sod = sod + 20;
        if (mat[i][j + 1] != !mat[i][j])
            sod = sod + 20;
        break;
    default:
        break;
    }

    return sod;
}
//Funzione per il calcolo della soddisfazione delle celle centrali della matrice
double centro(int **mat, int i, int j)
{
    double sod = 0;
    if (mat[i - 1][j - 1] != !mat[i][j])
        sod = sod + 12.5;
    if (mat[i - 1][j] != !mat[i][j])
        sod = sod + 12.5;
    if (mat[i - 1][j + 1] != !mat[i][j])
        sod = sod + 12.5;
    if (mat[i][j - 1] != !mat[i][j])
        sod = sod + 12.5;
    if (mat[i][j + 1] != !mat[i][j])
        sod = sod + 12.5;
    if (mat[i + 1][j - 1] != !mat[i][j])
        sod = sod + 12.5;
    if (mat[i + 1][j] != !mat[i][j])
        sod = sod + 12.5;
    if (mat[i + 1][j + 1] != !mat[i][j])
        sod = sod + 12.5;
    return sod;
}
//Funzione per il calcolo della soddisfazione del margine sinistro alto
int angolo_sinistro_alto(int **mat, const int *vet, int i, int j)
{
    int sod = 0;
    if (mat[i][j + 1] != !mat[i][j])
        sod = sod + 20;
    if (mat[i + 1][j] != !mat[i][j])
        sod = sod + 20;
    if (mat[i + 1][j + 1] != !mat[i][j])
        sod = sod + 20;
    if (vet[j] != !mat[i][j])
        sod = sod + 20;
    if (vet[j + 1] != !mat[i][j])
        sod = sod + 20;
    return sod;
}
//Funzione per il calcolo della soddisfazione del margine detro alto
int angolo_destro_alto(int **mat, const int *vet, int i, int j)
{
    int sod = 0;
    if (mat[i][j - 1] != !mat[i][j])
        sod = sod + 20;
    if (mat[i + 1][j - 1] != !mat[i][j])
        sod = sod + 20;
    if (mat[i + 1][j] != !mat[i][j])
        sod = sod + 20;
    if (vet[j] != !mat[i][j])
        sod = sod + 20;
    if (vet[j - 1] != !mat[i][j])
        sod = sod + 20;
    return sod;
}
//Funzione per il calcolo della soddisfazione del margine sinistro basso
int angolo_sinistro_basso(int **mat, const int *vet, int i, int j)
{
    int sod = 0;
    if (mat[i - 1][j] != !mat[i][j])
        sod = sod + 20;
    if (mat[i - 1][j + 1] != !mat[i][j])
        sod = sod + 20;
    if (mat[i][j + 1] != !mat[i][j])
        sod = sod + 20;
    if (vet[j] != !mat[i][j])
        sod = sod + 20;
    if (vet[j + 1] != !mat[i][j])
        sod = sod + 20;
    return sod;
}
//Funzione per il calcolo della soddisfazione del margine destro basso
int angolo_destro_basso(int **mat, const int *vet, int i, int j)
{
    int sod = 0;
    if (mat[i - 1][j - 1] != !mat[i][j])
        sod = sod + 20;
    if (mat[i - 1][j] != !mat[i][j])
        sod = sod + 20;
    if (mat[i][j - 1] != !mat[i][j])
        sod = sod + 20;
    if (vet[j] != !mat[i][j])
        sod = sod + 20;
    if (vet[j - 1] != !mat[i][j])
        sod = sod + 20;
    return sod;
}
//Funzione per il calcolo della soddisfazione del margine alto nel caso in cui non ci troviamo nei margini effettuvi della matrice ma ci troviamo
//nel margine alto di una sottomatrice
double margine_alto(int **mat, const int *vet, int i, int j)
{
    double sod = 0;
    if (mat[i][j - 1] != !mat[i][j])
        sod = sod + 12.5;
    if (mat[i][j + 1] != !mat[i][j])
        sod = sod + 12.5;
    if (mat[i + 1][j - 1] != !mat[i][j])
        sod = sod + 12.5;
    if (mat[i + 1][j] != !mat[i][j])
        sod = sod + 12.5;
    if (mat[i + 1][j + 1] != !mat[i][j])
        sod = sod + 12.5;
    if (vet[j] != !mat[i][j])
        sod = sod + 12.5;
    if (vet[j - 1] != !mat[i][j])
        sod = sod + 12.5;
    if (vet[j + 1] != !mat[i][j])
        sod = sod + 12.5;
    return sod;
}
//Funzione per il calcolo della soddisfazione del margine bassp nel caso in cui non ci troviamo nei margini effettuvi della matrice ma ci troviamo
//nel margine bassp di una sottomatrice
double margine_basso(int **mat, const int *vet, int i, int j)
{
    double sod = 0;
    if (mat[i][j - 1] != !mat[i][j])
        sod = sod + 12.5;
    if (mat[i][j + 1] != !mat[i][j])
        sod = sod + 12.5;
    if (mat[i - 1][j - 1] != !mat[i][j])
        sod = sod + 12.5;
    if (mat[i - 1][j] != !mat[i][j])
        sod = sod + 12.5;
    if (mat[i - 1][j + 1] != !mat[i][j])
        sod = sod + 12.5;
    if (vet[j] != !mat[i][j])
        sod = sod + 12.5;
    if (vet[j - 1] != !mat[i][j])
        sod = sod + 12.5;
    if (vet[j + 1] != !mat[i][j])
        sod = sod + 12.5;
    return sod;
}
//Il compito di questa funzione passato un agente indosfatto di salvare la sua posizione all'interno del corrispettivo vettore
void aggiungi_insodisfatto(int agente, insodisfatti *array_ins_x, insodisfatti *array_ins_y, int *qnt_ins_x, int *qnt_ins_y, int i, int j)
{
    if (agente == 1)
    {
        array_ins_x[*qnt_ins_x].x = i;
        array_ins_x[*qnt_ins_x].y = j;
        *qnt_ins_x += 1;
    }
    else
    {
        array_ins_y[*qnt_ins_y].x = i;
        array_ins_y[*qnt_ins_y].y = j;
        *qnt_ins_y += 1;
    }
}
//Funzione per il calcolo degli insodisfatti
void calcolo_insodisfatti(int **mymat, int common_size, int *celle_vuote, int *qnt_ins_x, int *qnt_ins_y, info_mat info_generali, insodisfatti *array_ins_x, insodisfatti *array_ins_y, int *primariga, int *ultimariga, int myrank)
{
    *celle_vuote = 0;
    *qnt_ins_x = 0;
    *qnt_ins_y = 0;

    for (int i = 0; i < info_generali.my_dim_x; i++)
    {
        for (int j = 0; j < info_generali.my_dim_y; j++)
        {
            //Inanzitutto andiamoa controllare se ci troviamo in corrispondeza di una cella vuota, in tal caso non procediamo in nesssun controllo
            double support_sod = 0;
            if (mymat[i][j] == 1000)
                *celle_vuote += 1;
            /** Successivamente andiamo a controllare in che posizione della matrice ci troviamo in modo tale da prendere in considerazione i giusti vicini.
            Per effettuare il controllo andiamo a controllare che il mio vicino sia diverso dal mio opposto, in questo modo siamo in grado di rilevare 
            sia che il mio vicino sia del mio stesso seme o sia una cella vuota
            Infine calcolata la soddisfazione vediamo se questa basti per renderlo soddisfatto altrimenti lo aggiungiamo agli insodisfatti chiamando 
            la funzione designata **/
            else if (i == 0 && j == 0) // angolo in alto a sinistra
            {
                if (myrank == 0)
                {
                    if (mymat[i][j + 1] != !mymat[i][j])
                        support_sod = support_sod + 33.3;
                    if (mymat[i + 1][j] != !mymat[i][j])
                        support_sod = support_sod + 33.3;
                    if (mymat[i + 1][j + 1] != !mymat[i][j])
                        support_sod = support_sod + 33.3;
                }
                else
                {
                    support_sod = angolo_sinistro_alto(mymat, primariga, i, j);
                }

                if (support_sod < info_generali.sod)
                    aggiungi_insodisfatto(mymat[i][j], array_ins_x, array_ins_y, qnt_ins_x, qnt_ins_y, i, j);
            }
            else if (i == 0 && j == info_generali.my_dim_y - 1) // angolo in alto a destra
            {
                if (myrank == 0)
                {
                    if (mymat[i][j - 1] != !mymat[i][j])
                        support_sod = support_sod + 33.3;
                    if (mymat[i + 1][j - 1] != !mymat[i][j])
                        support_sod = support_sod + 33.3;
                    if (mymat[i + 1][j] != !mymat[i][j])
                        support_sod = support_sod + 33.3;
                }
                else
                {
                    support_sod = angolo_destro_alto(mymat, primariga, i, j);
                }

                if (support_sod < info_generali.sod)
                    aggiungi_insodisfatto(mymat[i][j], array_ins_x, array_ins_y, qnt_ins_x, qnt_ins_y, i, j);
            }
            else if (i == info_generali.my_dim_x - 1 && j == 0) // angolo in basso a sinistra
            {
                if (myrank != common_size - 1)
                {
                    support_sod = angolo_sinistro_basso(mymat, ultimariga, i, j);
                }
                else
                {
                    if (mymat[i - 1][j] != !mymat[i][j])
                        support_sod = support_sod + 33.3;
                    if (mymat[i - 1][j + 1] != !mymat[i][j])
                        support_sod = support_sod + 33.3;
                    if (mymat[i][j + 1] != !mymat[i][j])
                        support_sod = support_sod + 33.3;
                }

                if (support_sod < info_generali.sod)
                    aggiungi_insodisfatto(mymat[i][j], array_ins_x, array_ins_y, qnt_ins_x, qnt_ins_y, i, j);
            }
            else if (i == info_generali.my_dim_x - 1 && j == info_generali.my_dim_y - 1) // angolo in basso a destra
            {
                if (myrank != common_size - 1)
                {
                    support_sod = angolo_destro_basso(mymat, ultimariga, i, j);
                }
                else
                {
                    if (mymat[i - 1][j] != !mymat[i][j])
                        support_sod = support_sod + 33.3;
                    if (mymat[i - 1][j - 1] != !mymat[i][j])
                        support_sod = support_sod + 33.3;
                    if (mymat[i][j - 1] != !mymat[i][j])
                        support_sod = support_sod + 33.3;
                }

                if (support_sod < info_generali.sod)
                    aggiungi_insodisfatto(mymat[i][j], array_ins_x, array_ins_y, qnt_ins_x, qnt_ins_y, i, j);
            }
            else if (i != 0 && i != info_generali.my_dim_x - 1 && j == 0) //margine sinistro
            {

                support_sod = margine(mymat, i, j, 's');
                if (support_sod < info_generali.sod)
                    aggiungi_insodisfatto(mymat[i][j], array_ins_x, array_ins_y, qnt_ins_x, qnt_ins_y, i, j);
            }
            else if (i != 0 && i != info_generali.my_dim_x - 1 && j == common_size - 1) //margine destro
            {
                support_sod = margine(mymat, i, j, 'd');
                if (support_sod < info_generali.sod)
                    aggiungi_insodisfatto(mymat[i][j], array_ins_x, array_ins_y, qnt_ins_x, qnt_ins_y, i, j);
            }
            else if (i == info_generali.my_dim_x - 1) //margine basso
            {
                if (myrank != common_size - 1)
                    support_sod = margine_basso(mymat, ultimariga, i, j);
                else
                    support_sod = margine(mymat, i, j, 'b');
                if (support_sod < info_generali.sod)
                    aggiungi_insodisfatto(mymat[i][j], array_ins_x, array_ins_y, qnt_ins_x, qnt_ins_y, i, j);
            }
            else if (i == 0) // margine alto
            {
                if (myrank == 0)
                    support_sod = margine(mymat, i, j, 'a');
                else
                    support_sod = margine_alto(mymat, primariga, i, j);
                if (support_sod < info_generali.sod)
                    aggiungi_insodisfatto(mymat[i][j], array_ins_x, array_ins_y, qnt_ins_x, qnt_ins_y, i, j);
            }
            else // centro
            {
                support_sod = centro(mymat, i, j);
                if (support_sod < info_generali.sod)
                    aggiungi_insodisfatto(mymat[i][j], array_ins_x, array_ins_y, qnt_ins_x, qnt_ins_y, i, j);
            }
        }
    }
}
//Funzione per lo scambio tra processori
void change_for_process(int myrank, int common_size, int **mymat, richiesta *array, insodisfatti *array_ins, int agente, info_mat *info_generali, richiesta *risposta, MPI_Datatype type)
{
    int qnt_ins = risposta->qnt_ins;
    int celle_vuote = risposta->celle_libere;
    /**
    Il compito di effettuare gli scambi è demanato al master il quale randomicamente scegle su quale processore spostare gli insoddisfati. 
    Al processore scelto gli verranno ascegnati tutti gli agenti che può prendere in base alle celle libere, mentre gli agenti che non verranno 
    soddisfatti o dovranno aspettare il ciclo successivo o probabilmente dopo questo scambio saranno soddisfatti
    **/
    if (myrank == 0)
        for (int i = 0; i < common_size; i++)
        {
            int procs, supp_pos = 0;
            if (array[i].qnt_ins > 0)
            {
                procs = rand() % common_size;
                if (array[procs].celle_libere > 0)
                {
                    while (array[procs].celle_libere > 0 && array[i].qnt_ins > 0)
                    {
                        array[procs].celle_libere -= 1;
                        array[i].qnt_ins -= 1;
                    }
                }
            }
        }

    MPI_Scatter(array, 1, type, risposta, 1, type, 0, MPI_COMM_WORLD);
    /**Dopo gli scambi vengono comunicati ai processori che procedono nel libere e assegnare le celle**/
    for (int i = qnt_ins; i > risposta->qnt_ins; i--)
    {
        mymat[array_ins[i - 1].x][array_ins[i - 1].y] = 1000;
        if (agente == 1)
            info_generali->my_age_x -= 1;
        else
            info_generali->my_age_y -= 1;
    }

    for (int i = celle_vuote; i > risposta->celle_libere; i--)
    {
        assegna(mymat, agente, info_generali->my_dim_x, info_generali->my_dim_y);
        if (agente == 1)
            info_generali->my_age_x += 1;
        else
            info_generali->my_age_y += 1;
    }
}
int main(int argc, char *argv[])
{
    int **mymat, soddisfazione_totale = 100, *primariga, *ultimariga, ciclo = 0, insodisfatti_totali = 0;
    int *global_soddisfazione_x, *global_soddisfazione_y;
    double start, end;
    int myrank, common_size;
    MPI_Status status;

    insodisfatti *array_ins_x, *array_ins_y;

    info_mat info_generali;
    info_mat *info_x_proc;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    MPI_Comm_size(MPI_COMM_WORLD, &common_size);
    /** la rand è iniziallizata a 0 per effettuare i test**/
    //srand(time(NULL));
    srand(0);

    MPI_Datatype info_mat_type;
    MPI_Datatype info_mat_types[5] = {MPI_INT, MPI_INT, MPI_INT, MPI_INT, MPI_INT};
    MPI_Aint info_offsets[5];
    int info_blocklengths[5] = {1, 1, 1, 1, 1};
    info_offsets[0] = offsetof(info_mat, my_dim_x);
    info_offsets[1] = offsetof(info_mat, my_dim_y);
    info_offsets[2] = offsetof(info_mat, my_age_x);
    info_offsets[3] = offsetof(info_mat, my_age_y);
    info_offsets[4] = offsetof(info_mat, sod);
    MPI_Type_create_struct(5, info_blocklengths, info_offsets, info_mat_types, &info_mat_type);
    MPI_Type_commit(&info_mat_type);

    MPI_Datatype richiesta_type;
    MPI_Datatype richiesta_types[2] = {MPI_INT, MPI_INT};
    MPI_Aint offsets[2];
    int blocklengths[2] = {1, 1};
    offsets[0] = offsetof(richiesta, qnt_ins);
    offsets[1] = offsetof(richiesta, celle_libere);
    MPI_Type_create_struct(2, blocklengths, offsets, richiesta_types, &richiesta_type);
    MPI_Type_commit(&richiesta_type);
    // lettura e validazione di tutti i campi gestita dal master
    if (myrank == 0)
    {
        int dim, age_x, age_y, let_sod, quo_mat, r_mat, quo_age_x, r_age_x, quo_age_y, r_age_y;
        // controlla che ogni proc abbia almeno una riga
        do
        {
            printf("Inserisci la grandezza della matrice \n");
            scanf("%d", &dim);
        } while (dim / common_size < 1);

        int support, spazio_disp = (dim * dim) - ((dim * dim) * 30 / 100);
        //controlla ci sia abbastanza spazio libero per effettuare successivamente gli scambi
        do
        {
            printf("Inserisci quanti agenti di tipo x vuoi \n");
            scanf("%d", &age_x);
            printf("Inserisci quanti agenti di tipo y vuoi \n");
            scanf("%d", &age_y);
            support = age_x + age_y;

        } while (support > spazio_disp);
        printf("dimmi il grado di soddisfazione da raggiungere \n");
        scanf("%d", &let_sod);
        quo_mat = dim / common_size;
        r_mat = dim % common_size;
        quo_age_x = age_x / common_size;
        r_age_x = age_x % common_size;
        quo_age_y = age_y / common_size;
        r_age_y = age_y % common_size;
        //assegna ad ogni processore le corrispettive informazioni
        info_x_proc = (info_mat *)malloc(common_size * sizeof(info_mat));
        for (int i = 0; i < common_size; i++)
        {
            if (r_mat > 0)
            {
                info_x_proc[i].my_dim_x = quo_mat + 1;
                info_x_proc[i].my_dim_y = dim;
                r_mat -= 1;
            }
            else
            {
                info_x_proc[i].my_dim_x = quo_mat;
                info_x_proc[i].my_dim_y = dim;
            }
            if (r_age_x > 0)
            {
                info_x_proc[i].my_age_x = quo_age_x + 1;
                r_age_x -= 1;
            }
            else
            {
                info_x_proc[i].my_age_x = quo_age_x;
            }
            if (r_age_y > 0)
            {
                info_x_proc[i].my_age_y = quo_age_y + 1;
                r_age_y -= 1;
            }
            else
            {
                info_x_proc[i].my_age_y = quo_age_y;
            }
            info_x_proc[i].sod = let_sod;
        }
        start = MPI_Wtime();
    }

    MPI_Scatter(info_x_proc, 1, info_mat_type, &info_generali, 1, info_mat_type, 0, MPI_COMM_WORLD);

    printf("Ciao sono %d la dimensione della mia matrice è x=%d y=%d gli agenti sono x=%d y=%d la sod -> %d \n", myrank, info_generali.my_dim_x, info_generali.my_dim_x, info_generali.my_age_x, info_generali.my_age_y, info_generali.sod);
    //valorizza la matrice con i dati ricevuti dal master
    mymat = (int **)malloc(info_generali.my_dim_x * sizeof(int *));
    for (int i = 0; i < info_generali.my_dim_x; i++)
        mymat[i] = (int *)malloc(info_generali.my_dim_y * sizeof(int));
    for (int i = 0; i < info_generali.my_dim_x; i++)
        for (int j = 0; j < info_generali.my_dim_y; j++)
            mymat[i][j] = 1000;
    for (int i = 0; i < info_generali.my_age_x; i++)
        assegna(mymat, 1, info_generali.my_dim_x, info_generali.my_dim_y);
    for (int i = 0; i < info_generali.my_age_y; i++)
        assegna(mymat, 0, info_generali.my_dim_x, info_generali.my_dim_y);

    MPI_Barrier(MPI_COMM_WORLD);

    do
    {
        // allocazione del nuovo tipo cioè di un vettore continuo di tipo int usato per comunicare l'intera riga di vicini
        MPI_Datatype rowtype;
        MPI_Type_contiguous(info_generali.my_dim_y, MPI_INT, &rowtype);
        MPI_Type_commit(&rowtype);

        if (myrank != 0)
            MPI_Send(&mymat[0][0], 1, rowtype, myrank - 1, 21, MPI_COMM_WORLD);
        if (myrank != common_size - 1)
            MPI_Send(&mymat[info_generali.my_dim_x - 1][0], 1, rowtype, myrank + 1, 20, MPI_COMM_WORLD);
        if (myrank != common_size - 1)
        {
            ultimariga = (int *)malloc(info_generali.my_dim_y * sizeof(int));
            MPI_Recv(ultimariga, info_generali.my_dim_y, MPI_INT, myrank + 1, 21, MPI_COMM_WORLD, &status);
        }
        if (myrank != 0)
        {
            primariga = (int *)malloc(info_generali.my_dim_y * sizeof(int));
            MPI_Recv(primariga, info_generali.my_dim_y, MPI_INT, myrank - 1, 20, MPI_COMM_WORLD, &status);
        }

        MPI_Type_free(&rowtype); //de-allocazione tipo nuovo;

        if (myrank == 0)
        {
            //questi due vettori che sono gestiti dal master servono per salvare gli insoddisfatti di tutti i processori
            global_soddisfazione_x = (int *)malloc(common_size * sizeof(int));
            global_soddisfazione_y = (int *)malloc(common_size * sizeof(int));
        }
        //questi due vettori tengono traccia delle posizoni degli insodisfatti, abbiamo un vettore per l'agente x e uno per y per semplicita di gestione
        richiesta risposta_x, risposta_y;

        array_ins_x = (insodisfatti *)malloc(info_generali.my_age_x * sizeof(insodisfatti));
        array_ins_y = (insodisfatti *)malloc(info_generali.my_age_y * sizeof(insodisfatti));

        calcolo_insodisfatti(mymat, common_size, &risposta_x.celle_libere, &risposta_x.qnt_ins, &risposta_y.qnt_ins, info_generali, array_ins_x, array_ins_y, primariga, ultimariga, myrank);
        MPI_Barrier(MPI_COMM_WORLD);

        MPI_Gather(&risposta_x.qnt_ins, 1, MPI_INT, global_soddisfazione_x, 1, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Gather(&risposta_y.qnt_ins, 1, MPI_INT, global_soddisfazione_y, 1, MPI_INT, 0, MPI_COMM_WORLD);

        if (myrank != common_size - 1)
            free(ultimariga);
        if (myrank != 0)
            free(primariga);
        /**Una volta calcolati gli insodisfatti le informazioni vengono passata al master il quale controlla se esistono degli insodisfatti e in base a ciò
            decide se continuare a ciclare o terminare il programma**/
        if (myrank == 0)
        {
            insodisfatti_totali = 0;
            for (int i = 0; i < common_size; i++)
            {
                insodisfatti_totali += global_soddisfazione_x[i];
                insodisfatti_totali += global_soddisfazione_y[i];
            }

            if (insodisfatti_totali > 0)
            {
                ciclo += 1;
            }
            else
            {
                printf("-----------------ciclo:%d----------------------------------TERMINATO CON SUCCESSO-------------------------------------- \n", ciclo);
                ciclo = -5;
            }
            free(global_soddisfazione_x);
            free(global_soddisfazione_y);
        }

        MPI_Bcast(&ciclo, 1, MPI_INT, 0, MPI_COMM_WORLD);
        /**Nel caso in cui è stato comunicato di continuare e quindi ci troviamo nel caso in cui c'è almeno un insoddisfatto si passa alla fase di scambio**/
        if (ciclo != -5)
        {
            richiesta *global_qnt_insod = NULL;
            if (myrank == 0)
                global_qnt_insod = (richiesta *)malloc(common_size * sizeof(richiesta));
            //viene effettuato lo scambio per l'agente X
            MPI_Gather(&risposta_x, 1, richiesta_type, global_qnt_insod, 1, richiesta_type, 0, MPI_COMM_WORLD);
            change_for_process(myrank, common_size, mymat, global_qnt_insod, array_ins_x, 1, &info_generali, &risposta_x, richiesta_type);
            //viene aggiornato il numero di celle libere
            risposta_y.celle_libere = risposta_x.celle_libere;
            //viene effettauto lo scambio per l'agente Y
            MPI_Gather(&risposta_y, 1, richiesta_type, global_qnt_insod, 1, richiesta_type, 0, MPI_COMM_WORLD);
            change_for_process(myrank, common_size, mymat, global_qnt_insod, array_ins_y, 0, &info_generali, &risposta_y, richiesta_type);

            MPI_Barrier(MPI_COMM_WORLD);
            if (myrank == 0)
                free(global_qnt_insod);
        }
        free(array_ins_x);
        free(array_ins_y);
    //è stampo impostato che al massimo posso essere fatti 2mln di controlli/scambio
    } while (ciclo != -5 && ciclo < 2000000);

    MPI_Barrier(MPI_COMM_WORLD);
    if (ciclo != -5) //nel caso in cui ciclo non è uguale a -5 significa che non è riuscito a trovare un allocazione ottimale agli agenti
        printf("-----------------ciclo:%d----------------------------------Non terminato con successo--------------------------- \n", ciclo);

    for (int i = 0; i < info_generali.my_dim_x; i++)
        free(mymat[i]);
    free(mymat);

    MPI_Type_free(&info_mat_type);
    MPI_Type_free(&richiesta_type);

    if (myrank == 0)
        free(info_x_proc);

    MPI_Barrier(MPI_COMM_WORLD);
    end = MPI_Wtime();

    if (myrank == 0)
        printf("Time in ms = %f\n", end - start);

    MPI_Finalize();
    return 0;
}