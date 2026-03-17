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

void set_status(struct agent* a, char new_status)
{
    if (is_valid_status(new_status)) {
        a->status = new_status;
        a->time_in_status = 0;
    }
}







int main(int argc,char* argv[]){


    struct agent a;
    set_status(&a, 'S');


    return 0;
}