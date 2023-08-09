#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>

#define POP_SIZE 10
#define GENERATION_COUNT 40
#define TOURNAMENT_SIZE 8
#define MUTATION_RATE 0.0
#define MAX_CAPACITY 20 // kg
#define INST_SIZE 10


typedef struct {
   int crom[INST_SIZE];
   int f;
}Individual;


void create_pop(Individual * pop, int ** inst){
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

int main(){
   int i;

   // ==========================   cria instancia  ============================
   int ** inst = malloc(sizeof(int *)*2);
   inst[0] = malloc(sizeof(int)*INST_SIZE);  // lucro
   inst[1] = malloc(sizeof(int)*INST_SIZE);  // peso

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

   // ==========================   cria populacao ============================
   Individual * pop = malloc(sizeof(Individual)*POP_SIZE);

   create_pop(pop,inst);


   for(int i = 0;  i < POP_SIZE; i++){
      int w = 0; 
      printf("Indivíduo %d = ", i+1);
      for(int j = 0; j < INST_SIZE; j++){
           printf("%d ",pop[i].crom[j]);
           w += pop[i].crom[j] * inst[1][j];
      }     
      printf("   fitness = %d   peso = %d \n",pop[i].f,w);     
   }
}
