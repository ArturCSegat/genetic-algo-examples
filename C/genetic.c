#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>

#define POP_SIZE 100
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

Individual *  create_pop(int ** inst, Individual **bestPop){
    int lista[INST_SIZE],aux,pos;
    Individual * pop = malloc(sizeof(Individual)*POP_SIZE);

    *bestPop = &pop[0];

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
        if((*bestPop)->f < pop[i].f)
            *bestPop = &pop[i];
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

int checSolution(Individual *ind){

   if(ind->w > MAX_CAPACITY){
      return 0;
      } 

   return 1;
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
    int g = 0;

//    printf("%d %d\n",fea->w,unfea->w);

    do{
        // sets g to the index to the first 1 who's weight could fit in fea, if there is no 1 who could fit in fea g = INST_SIZE
        for(g= 0; g < INST_SIZE && ((!unfea->crom[g] && fea->crom[g]) || inst[1][g] > idleCap); g++); 
        
        // this check translates to if there is a valid gene in unfea
        if (g < INST_SIZE) {
            // now that its know a transfer is possile, finds the actual best gene to transfer
            for(int i = g+1; i < INST_SIZE; i++) {
                if((inst[1][i] > inst[1][g] && inst[1][i] <= idleCap) && (unfea->crom[i] == 1 && fea->crom[i] == 0)) {
                    g = i;
                    }
            }

//            printf("g %d, gw %d, gv: %d, gv2: %d, ic: %d\n", g, inst[1][g], unfea->crom[g], fea->crom[g], idleCap);
            unfea->crom[g] = 0;
            unfea->f -= inst[0][g];
            unfea->w -= inst[1][g];

            fea->crom[g] = 1;
            fea->f += inst[0][g];
            fea->w += inst[1][g];
        }
    }while (g < INST_SIZE && unfea->w > MAX_CAPACITY);
    
    while (unfea->w > MAX_CAPACITY){
//        printf("give up\n");
        for(g = 0; !(unfea->crom[g]); g++);
        int cheap = g;
        for(g++; g < INST_SIZE; g++){
            if(unfea->crom[g] &&  inst[0][g] < inst[0][cheap])
                cheap = g;
        }
        unfea->crom[cheap] = 0;
        unfea->f -= inst[0][cheap];
        unfea->w -= inst[1][cheap];
    }
//    printf("%d %d\n",fea->w,unfea->w);

}


void crossover(Individual *pop, int *parents, Individual * offspring, Individual **bestOffspring, Individual **worstOffspring,  int ** inst){
    int pcut,k;

    *bestOffspring = &offspring[0];
    *worstOffspring = &offspring[0];

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
    
//        printf("%d %d\n",offspring[i].f,offspring[i+1].f);

        if((*bestOffspring)->f < offspring[i].f)
           *bestOffspring = &offspring[i];
       
        if((*bestOffspring)->f < offspring[i+1].f)
           *bestOffspring = &offspring[i+1];

        if((*worstOffspring)->f > offspring[i].f)
           *worstOffspring = &offspring[i];
       
        if((*worstOffspring)->f > offspring[i+1].f)
           *worstOffspring = &offspring[i+1];
//        printf("%d \n",(*worstOffspring)->w);
    }
}

void updatedPop(Individual **pop, Individual **offspring){
      Individual * aux;
      aux = *pop;
      *pop = *offspring;
      *offspring = aux;

   }

int main(){

    int ** inst = readInstance();
    int parents[POP_SIZE]; 
    Individual * offspring = malloc(sizeof(Individual)*POP_SIZE);
    Individual *bestPop, *bestOffspring, *worstOffspring;

    unsigned seed = time(NULL);
//    srand(seed);
    srand(1694034314);

    printf("Seed : %d\n",seed);

    //=========================== Genetic Algorithm =====================
    Individual * pop = create_pop(inst,&bestPop);
    printPop(pop,inst);
    printf("Best Individual:  fitness -> %d   weight -> %d\n",bestPop->f,bestPop->w);

    for (int i = 0; i < MAX_GENERATIONS; i++){
        selection(pop,parents);
        crossover(pop,parents,offspring,&bestOffspring,&worstOffspring,inst);
        printf("Best Individual:  fitness -> %d   weight -> %d\n",bestOffspring->f,bestOffspring->w);
        printf("Worst Individual: fitness -> %d   weight -> %d\n\n",worstOffspring->f,worstOffspring->w);
        updatedPop(&pop,&offspring);
    }

}
