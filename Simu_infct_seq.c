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
    // Initialisation du générateur avec une graine
    srand(2002);
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

    // step2: init le monde
    struct agent *world = malloc(nbr_tot_agents * sizeof(struct agent));
    for (int i = 0; i < nbr_tot_agents; i++)
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
        for (int j = 0; j < nbr_tot_agents; j++)
        {
            world[j].x = random_int_between(0, width_grille - 1);
            world[j].y = random_int_between(0, Higth_grille - 1);
        }

        // struct agent *world_inf = calloc(sizeof(struct agent), width_grille * width_grille);

        for (int j = 0; j < nbr_tot_agents; j++)
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

                for (int k = 0; k < nbr_tot_agents; k++)
                {
                    if (&world[j] == &world[k])
                        continue;
                    if (world[k].status != 'I')
                        continue;

                    int xk = world[k].x;
                    int yk = world[k].y;

                    int voisin_x = (xk == x0) || (xk == x_left) || (xk == x_right);
                    int voisin_y = (yk == y0) || (yk == y_down) || (yk == y_up);

                    if (voisin_x && voisin_y)
                    {
                        Ni++;
                    }
                }

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

        for (int j = 0; j < nbr_tot_agents; j++)
        {
            world[j].time_in_status++;
            if (world[j].status == 'E' && world[j].time_in_status > world[j].dE)
                set_status(&world[j], 'I');
            else if (world[j].status == 'I' && world[j].time_in_status > world[j].dI)
                set_status(&world[j], 'R');
            else if (world[j].status == 'R' && world[j].time_in_status > world[j].dR)
                set_status(&world[j], 'S');
        }

        print_Jour(i + 1, world, nbr_tot_agents, width_grille, Higth_grille);
    }

    free(world);

    return 0;
}