#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>

#define POP_SIZE 10
#define MAX_GENERATIONS 40
#define TOURNAMENT_SIZE 3 
#define MUTATION_RATE 0.0
#define MAX_CAPACITY 20 // kg
#define INST_SIZE 10

typedef struct {
   int crom[INST_SIZE];
   int f;
   int w;
}Individual;

Individual *  create_pop(int ** inst){
   int lista[INST_SIZE],aux,pos;
   Individual * pop = malloc(sizeof(Individual)*POP_SIZE);

   for (int i = 0; i < INST_SIZE; i++)  lista[i] = i;

   for(int i = 0; i < POP_SIZE; i++){
      pop[i].f = 0;
      pop[i].w= 0;
      for(int j = 0; j < INST_SIZE; j++){
         pos = rand()%(INST_SIZE-j);
         pop[i].crom[lista[pos]] = 0;
         if( rand()%2 == 0){
            if (pop[i].w+ inst[1][lista[pos]] <= MAX_CAPACITY){
               pop[i].crom[lista[pos]] = 1;
               pop[i].f +=  inst[0][lista[pos]]; 
               pop[i].w+= inst[1][lista[pos]];
            }
         } 
         aux = lista[pos];
         lista[pos] = lista[INST_SIZE-j-1];
         lista[INST_SIZE-j-1] = aux;
     }
   }
   return pop;
}

int ** readInstance(){
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

   return inst;
   }

void printPop(Individual *pop, int ** inst){
   for(int i = 0;  i < POP_SIZE; i++){
      int w = 0; 
      printf("Indivíduo %d = ", i);
      for(int j = 0; j < INST_SIZE; j++){
           printf("%d ",pop[i].crom[j]);
           w += pop[i].crom[j] * inst[1][j];
      }     
      printf("   fitness = %d   peso = %d \n",pop[i].f,w);     
   }

   }

void selection(Individual *pop, int *parents){   // Tournament
   int parent, pos; 

   for(int k = 0;  k < POP_SIZE;  k++){
      if (k%2==1){ 
         parents[k]  = rand() % (POP_SIZE-1);
         if(parents[k] >= parents[k-1])
            parents[k]+=1;
         }else
            parents[k]  = rand() % POP_SIZE;
            

      for(int i = 0; i < TOURNAMENT_SIZE-1; i++){
         if (k%2==1){ 
            pos  = rand() % (POP_SIZE-1);
            if(pos >= parents[k-1])
               pos+=1;
            }else
               pos  = rand() % POP_SIZE;
         
         if (pop[pos].f > pop[parents[k]].f)
            parents[k] = pos;
         }
      }
   }
      
void repairOffspring(Individual *unfea, Individual *fea, int ** inst){

   int idleCap = MAX_CAPACITY - fea->w;
   int g;
   
   printf("%d %d\n",fea->w,unfea->w);

   do{
      for(g = 0;  (g < INST_SIZE) && (!unfea->crom[g] || inst[1][g] > idleCap); g++); 

      if( g < INST_SIZE){
         for(int i = g+1; i < INST_SIZE; i++)
            if(inst[1][i] > inst[1][g] && inst[1][i] < idleCap)
               g = i;
      
   
         unfea->crom[g] = 0;
         unfea->f -= inst[0][g];
         unfea->w -= inst[1][g];

         fea->crom[g] = 1;
         fea->f += inst[0][g];
         fea->w += inst[1][g];
         }
   }while (g < INST_SIZE && unfea->w > MAX_CAPACITY);

   while (unfea->w > MAX_CAPACITY){
      for(g = 0; !unfea->crom[g]; g++);
      int cheap = g;
      for(g++; g < INST_SIZE; g++){
            if(unfea->crom[g] &&  inst[0][g] < inst[0][cheap])
               cheap = g;
         }
      unfea->crom[g] = 0;
      unfea->f -= inst[0][cheap];
      unfea->w -= inst[1][cheap];
      }
   printf("%d %d\n\n",fea->w,unfea->w);

}


void crossover(Individual *pop, int *parents, Individual * offspring, int ** inst){
   int pcut,k;

   for(int i = 0; i < POP_SIZE; i+=2){
      offspring[i].f = 0;
      offspring[i].w = 0;
      offspring[i+1].f = 0;
      offspring[i+1].w = 0;
      
      pcut = rand()%(INST_SIZE-1);

      for(k =0; k <= pcut; k++){
         offspring[i].crom[k] = pop[parents[i]].crom[k];
         offspring[i].f += offspring[i].crom[k] ? inst[0][k] : 0;
         offspring[i].w += offspring[i].crom[k] ? inst[1][k] : 0;

         offspring[i+1].crom[k] = pop[parents[i+1]].crom[k];
         offspring[i+1].f += offspring[i+1].crom[k] ? inst[0][k] : 0;
         offspring[i+1].w += offspring[i+1].crom[k] ? inst[1][k] : 0;
         }
      for(; k <  INST_SIZE; k++){
         offspring[i].crom[k] = pop[parents[i+1]].crom[k];
         offspring[i].f += offspring[i].crom[k] ? inst[0][k] : 0;
         offspring[i].w += offspring[i].crom[k] ? inst[1][k] : 0;

         offspring[i+1].crom[k] = pop[parents[i]].crom[k];
         offspring[i+1].f += offspring[i+1].crom[k] ? inst[0][k] : 0;
         offspring[i+1].w += offspring[i+1].crom[k] ? inst[1][k] : 0;
         }

      if(offspring[i].w > MAX_CAPACITY) 
         repairOffspring(&offspring[i],&offspring[i+1],inst);
      else if(offspring[i+1].w > MAX_CAPACITY)
         repairOffspring(&offspring[i+1],&offspring[i],inst);
      }
   }


int main(){

   int ** inst = readInstance();
   int parents[POP_SIZE]; 
   Individual * offspring = malloc(sizeof(Individual)*POP_SIZE);

  
   //=========================== Genetic Algorithm =====================
   Individual * pop = create_pop(inst);
   
   for (int i = 0; i < MAX_GENERATIONS; i++){
      selection(pop,parents);
      crossover(pop,parents,offspring,inst);
      }

}
