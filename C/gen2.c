#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>

#define POP_SIZE 5000
#define GENERATION_COUNT 400
#define TOURNAMENT_SIZE 8
#define MUTATION_RATE 0.0
#define MAX_CAPACITY 20 // kg
#define INST_SIZE 10


typedef struct {
   int crom[INST_SIZE];
   int f;
}Individual;

void print_individual(Individual ind){
    printf("[ ");
    for (int i = 0; i < INST_SIZE; i++) {
        printf("%d ", ind.crom[i]);
    }
    printf("]\n");
}

void avaliate_individual(Individual * ind, int inst[2][INST_SIZE]) {
    int lista[INST_SIZE];
    for (int i = 0; i < INST_SIZE; i++)  lista[i] = i;
    ind->f = 0;

    int weight = 0;
    for (int i = 0; i < INST_SIZE; i++)  {
        int pos = rand()%(INST_SIZE-i);
        int byte = (*ind).crom[lista[pos]];
        if (byte == 1) {
            if(weight + inst[1][lista[pos]] > MAX_CAPACITY) {
                continue;
            } else {
                ind->f += inst[0][lista[pos]];
                weight += inst[1][lista[pos]];
            }
        }
        int aux = lista[pos];
        lista[pos] = lista[INST_SIZE-i-1];
        lista[INST_SIZE-i-1] = aux;
    }
}

void create_pop(Individual * pop, int inst[2][INST_SIZE]){
   int lista[INST_SIZE],aux,weight,pos;
   for (int i = 0; i < INST_SIZE; i++)  lista[i] = i;

   for(int i = 0; i < POP_SIZE; i++){
      pop[i].f = 0;
      weight = 0;
      for(int j = 0; j < INST_SIZE; j++){
         pos = rand()%(INST_SIZE-j);
         pop[i].crom[lista[pos]] = 0;
         if( rand()%2 == 0){
            if (weight + inst[1][lista[pos]] <= MAX_CAPACITY){
               pop[i].crom[lista[pos]] = 1;
               pop[i].f +=  inst[0][lista[pos]]; 
               weight += inst[1][lista[pos]];
            }
         } 
         aux = lista[pos];
         lista[pos] = lista[INST_SIZE-j-1];
         lista[INST_SIZE-j-1] = aux;
     }
   }
}

int run_tournament (Individual * pop){
    int best_idx = 0;
    int best_val = 0;
    for (int i = 0; i < TOURNAMENT_SIZE; i++) {
        int idx = rand() % POP_SIZE;
        Individual participant = pop[idx];

        if (participant.f > best_val) {
            best_val = participant.f;
            best_idx = idx;
        }
    }
    return best_idx;
}

void crossover (Individual * p1, Individual * p2) {
    for (int i = INST_SIZE / 2; i < INST_SIZE; i++) {
        int temp = (*p1).crom[i];
        (*p1).crom[i] = (*p2).crom[i];
        (*p2).crom[i] = temp;
    }
}

void copy_cromo(Individual * src, Individual * dst) {
    for (int i = 0; i < INST_SIZE; i++) {
        dst->crom[i] = src->crom[i];
    }
}

void reproduce_pop(Individual * pop, Individual ** buffer_pop, int inst[2][INST_SIZE]) {
    for (int i = 0; i < POP_SIZE / 2; i++) {
        int p1_idx = run_tournament(pop);
        int p2_idx = run_tournament(pop);

        copy_cromo(&(pop[p1_idx]), &((*buffer_pop)[i]));
        copy_cromo(&(pop[p2_idx]), &((*buffer_pop)[i + POP_SIZE / 2]));

        crossover(&((*buffer_pop)[i]), &((*buffer_pop)[i + POP_SIZE / 2]));
    }
}


float average(Individual * pop) {
    float sum = 0;
    int count = 0;

    for (int i = 0; i < POP_SIZE; i++) {
        sum += pop[i].f;
        count++;
    }
    return sum / count;
}


int main(){
    srand(time(NULL));

    // ==========================   cria instancia  ============================
    int inst[2][INST_SIZE];

    inst[0][0] =  7;  inst[1][0] = 10;
    inst[0][1] =  3;  inst[1][1] =  5;
    inst[0][2] =  1;  inst[1][2] =  2;
    inst[0][3] =  9;  inst[1][3] = 11;
    inst[0][4] = 10;  inst[1][4] = 15; 
    inst[0][5] =  5;  inst[1][5] =  7;
    inst[0][6] =  7;  inst[1][6] =  7;
    inst[0][7] =  9;  inst[1][7] =  6;
    inst[0][8] = 10;  inst[1][8] =  8; 
    inst[0][9] =  7;  inst[1][9] = 10;

    // for(int i = 0;  i < POP_SIZE; i++){
    //     int w = 0; 
    //     printf("Indivíduo %d = ", i+1);
    //     for(int j = 0; j < INST_SIZE; j++){
    //         printf("%d ",pop[i].crom[j]);
    //         w += pop[i].crom[j] * inst[1][j];
    //     }     
    //     printf("   fitness = %d   peso = %d \n",pop[i].f,w);     
    // }
    // ==========================   cria populacao ============================
    Individual * pop = malloc(sizeof(Individual)*POP_SIZE);
    create_pop(pop, inst);


    printf("Average 0: %f\n", average(pop));

    Individual * buffer_pop = malloc(sizeof(Individual)*POP_SIZE);

    for (int i = 0; i< GENERATION_COUNT; i++) {
        reproduce_pop(pop, &buffer_pop, inst);
        Individual * temp = &(*pop);
        pop = buffer_pop;
        buffer_pop = temp;

        for (int j = 0; j < POP_SIZE; j++) {
            avaliate_individual(&(pop[j]), inst);
        }
    }
    printf("\n");
    printf("Average %d: %f\n", GENERATION_COUNT, average(pop));
    free(pop);
    free(buffer_pop);
}

