#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

// 1st etape: def le struct
struct agent
{
    char status;
    int time_in_status;
    int dE;
    int dI;
    int dR;
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
double loi_exp()
{
}

negExp(double inMean)
{
    return -inMean * log(1 - genrand_real2());
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
    struct agent a;
    set_status(&a, 'S');

    return 0;
}