#include <mpi.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

// 1st etape: def le struct
struct agent
{
    char status;
    int time_in_status;
    double dE;
    double dI;
    double dR;
    int x;
    int y;
};

int is_valid_status(char status)
{
    return status == 'S' || status == 'E' || status == 'I' || status == 'R';
}

void set_status(struct agent *a, char new_status)
{
    if (is_valid_status(new_status))
    {
        a->status = new_status;
        a->time_in_status = 0;
    }
}

// Pour générer un nombre réel entre A et B, suivant une loi uniforme :
double random_between(double A, double B)
{
    return A + (B - A) * (rand() / (double)RAND_MAX);
}
// Pour générer un nombre entier entre A et B, suivant une loi uniforme :
int random_int_between(int A, int B)
{
    return A + rand() % (B - A + 1);
}

double negExp(double inMean)
{
    return -inMean * log(1 - rand() / (double)RAND_MAX);
}

void print_Jour(int day, struct agent *world, int nbr_tot_agents, int width_grille, int Higth_grille)
{
    int S = 0;
    int E = 0;
    int I = 0;
    int R = 0;

    for (int i = 0; i < nbr_tot_agents; i++)
    {
        if (world[i].status == 'S')
            S++;
        else if (world[i].status == 'E')
            E++;
        else if (world[i].status == 'I')
        {
            I++;
        }
        else if (world[i].status == 'R')
            R++;
    }

    printf("Jour %d //////// S=%d E=%d I=%d R=%d\n", day, S, E, I, R);
}

int main(int argc, char *argv[])
{

    int rank;
    int size;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Initialisation du générateur avec une graine
    srand(rank + 2002);
    // const du model
    int nbr_tot_agents = 20000;
    int width_grille = 300;
    int Higth_grille = 300;
    int init_nbr_S = 19980;
    int init_nbr_I = 20;
    int param_dE = 3;
    int param_dI = 7;
    int param_dR = 365;
    int time = 25;

    // ajout vars mpi
    int nbr_agent_rank = nbr_tot_agents / size;
    int nbr_rest_agent = nbr_tot_agents % size;
    int local_n = nbr_agent_rank + (rank < nbr_rest_agent ? 1 : 0);
    int start = rank * nbr_agent_rank + (rank < nbr_rest_agent ? rank : nbr_rest_agent);
    int end = start + local_n;
    int grid_size = width_grille * Higth_grille;

    int *local_inf = calloc(grid_size, sizeof(int));
    int *global_inf = calloc(grid_size, sizeof(int));


    // step2: init le monde
    struct agent *world = malloc(nbr_tot_agents * sizeof(struct agent));
    for (int i = start; i < end; i++)
    {
        if (i < init_nbr_S)
        {
            set_status(&world[i], 'S');
        }
        else
        {
            set_status(&world[i], 'I');
        }
        world[i].x = random_int_between(0, width_grille - 1);
        world[i].y = random_int_between(0, Higth_grille - 1);
        world[i].dE = negExp(param_dE);
        world[i].dR = negExp(param_dR);
        world[i].dI = negExp(param_dI);
    }

    // step3:deplacement et time
    for (int i = 0; i < time; i++)
    {
        for (int j = start; j < end; j++)
        {
            world[j].x = random_int_between(0, width_grille - 1);
            world[j].y = random_int_between(0, Higth_grille - 1);
        }
        //step5:remettre a 0 puis clc les i
        for (int idx = 0; idx < grid_size; idx++)
        {
            local_inf[idx] = 0;
        }
        for (int j = start; j < end; j++)
        {
            if (world[j].status == 'I')
            {
                int idx = world[j].y * width_grille + world[j].x;
                local_inf[idx]++;
            }
        }
        MPI_Allreduce(local_inf, global_inf, grid_size, MPI_INT, MPI_SUM, MPI_COMM_WORLD);

        for (int j = start; j < end; j++)
        {
            int agent_x = world[j].x;
            int agent_y = world[j].y;
            if (world[j].status == 'S')
            {
                int Ni = 0;

                int x0 = agent_x;
                int y0 = agent_y;

                int x_left = (x0 - 1 + width_grille) % width_grille;
                int x_right = (x0 + 1) % width_grille;
                int y_down = (y0 - 1 + Higth_grille) % Higth_grille;
                int y_up = (y0 + 1) % Higth_grille;

                Ni += global_inf[y0 * width_grille + x0];
                Ni += global_inf[y0 * width_grille + x_left];
                Ni += global_inf[y0 * width_grille + x_right];
                Ni += global_inf[y_down * width_grille + x0];
                Ni += global_inf[y_down * width_grille + x_left];
                Ni += global_inf[y_down * width_grille + x_right];
                Ni += global_inf[y_up * width_grille + x0];
                Ni += global_inf[y_up * width_grille + x_left];
                Ni += global_inf[y_up * width_grille + x_right];

                if (Ni > 0)
                {
                    double p = 1.0 - exp(-0.5 * Ni);
                    double rand_between = random_between(0.0, 1.0);
                    if (rand_between < p)
                    {
                        set_status(&world[j], 'E');
                    }
                }
            }
        }

        for (int j = start; j < end; j++)
        {
            world[j].time_in_status++;
            if (world[j].status == 'E' && world[j].time_in_status > world[j].dE)
                set_status(&world[j], 'I');
            else if (world[j].status == 'I' && world[j].time_in_status > world[j].dI)
                set_status(&world[j], 'R');
            else if (world[j].status == 'R' && world[j].time_in_status > world[j].dR)
                set_status(&world[j], 'S');
        }

        int local_S = 0;
        int local_E = 0;
        int local_I = 0;
        int local_R = 0;
        for (int j = start; j < end; j++)
        {
            if (world[j].status == 'S') local_S++;
            else if (world[j].status == 'E') local_E++;
            else if (world[j].status == 'I') local_I++;
            else if (world[j].status == 'R') local_R++;
        }

        int global_S = 0;
        int global_E = 0;
        int global_I = 0;
        int global_R = 0;
        MPI_Reduce(&local_S, &global_S, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
        MPI_Reduce(&local_E, &global_E, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
        MPI_Reduce(&local_I, &global_I, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
        MPI_Reduce(&local_R, &global_R, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

        if (rank == 0)
        {
            printf("Jour %d //////// S=%d E=%d I=%d R=%d\n", i + 1, global_S, global_E, global_I, global_R);
        }
    }

    free(local_inf);
    free(global_inf);
    free(world);
    MPI_Finalize();

    return 0;
}