#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <limits.h>

#define POP_SIZE 100
#define MAX_GENERATIONS 50
#define TOURNAMENT_SIZE 3
#define MUT_DIVIDENDO 1
#define MUT_DIVISOR 100  // jeito improvisado de representar 0.01

#define F  0
#define W  1

int INST_SIZE;
int MAX_CAPACITY;

typedef struct {
    int * crom;
    int f;
    int w;
}Individual;

int checkSolution(Individual *ind,int ** inst){
   int testW = 0;

   for(int i = 0 ; i < INST_SIZE; i++){
      testW += ind->crom[i] ? inst[W][i] : 0;
   }

   if(ind->w != testW || testW > MAX_CAPACITY){
      printf("ERRO no calculo da capacidade  = %d \n",ind->w);
      return 0;
   } 

   return 1;
}

Individual *  create_pop(int ** inst, Individual **bestPop){
    int lista[INST_SIZE],aux,pos;
    Individual * pop = malloc(sizeof(Individual)*POP_SIZE);
    for (int i = 0; i < POP_SIZE; i++) {
        pop[i].crom = malloc(INST_SIZE * sizeof(int));
    }

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

int ** readInstance(const char * file_name){
    // ==========================   cria instancia  ============================
    int ** inst = malloc(sizeof(int *)*2);
    
    FILE * f_ptr = fopen(file_name, "r");

    if (f_ptr == NULL) {
        printf("file error");
        exit(1);
    }

    char cr;

    char val_buf[6];
    int v_idx = 0;
    char wei_buf[6];
    int w_idx = 0;

    char * reading = val_buf;
    int * idx_ptr = &v_idx;
    
    int  i = 0;
    int first = 1;
    while((cr = fgetc(f_ptr)) != EOF) {
        if (*idx_ptr >= 6 || cr == '\r') {
            continue;
        }

        if (cr == ' ') {
            reading = wei_buf;
            idx_ptr = &w_idx;

            if (*idx_ptr != 0) {
                break;
            }


            continue;
        }

        if (cr == '\n') {
            if (first) {
                printf("ffffffff\n");
                INST_SIZE = atoi(val_buf);
                MAX_CAPACITY = atoi(wei_buf);
                           
                inst[F] = malloc(sizeof(int)*INST_SIZE);  // lucro
                inst[W] = malloc(sizeof(int)*INST_SIZE);  // peso
                i += 1;

                memset(val_buf, 0, sizeof(val_buf));
                memset(wei_buf, 0, sizeof(wei_buf));
                v_idx = 0;
                w_idx = 0;
                reading = val_buf;
                idx_ptr = &v_idx;
                first = 0;
                continue;
            }
            
            inst[W][i] = atoi(wei_buf);
            inst[F][i] = atoi(val_buf);
            i += 1;

            memset(val_buf, 0, sizeof(val_buf));
            memset(wei_buf, 0, sizeof(wei_buf));
            v_idx = 0;
            w_idx = 0;
            reading = val_buf;
            idx_ptr = &v_idx;
            continue;
        }

        reading[*idx_ptr] = cr;
        *idx_ptr += 1;
    }

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

void printInd(Individual ind) {
        printf("Indivíduo = ");
        for(int j = 0; j < INST_SIZE; j++){
            printf("%d ",ind.crom[j]);
        }     
        printf("   fitness = %d   peso = %d \n", ind.f, ind.w);     
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

    int idleCap; 
    int g = 0;

//    printf("%d %d\n",fea->w,unfea->w);

    do{
        idleCap = MAX_CAPACITY - fea->w;
        // sets g to the index to the first 1 who's weight could fit in fea, if there is no 1 who could fit in fea g = INST_SIZE
        for(g= 0; g < INST_SIZE && (!unfea->crom[g] || fea->crom[g] || inst[1][g] > idleCap); g++); 
        
        // this check translates to if there is a valid gene in unfea
        if (g < INST_SIZE) {
            // now that its know a transfer is possile, finds the actual best gene to transfer
            for(int i = g+1; i < INST_SIZE; i++) {
                if((inst[W][i] > inst[W][g] && inst[W][i] <= idleCap) && (unfea->crom[i] == 1 && fea->crom[i] == 0)) {
                    g = i;
                    }
            }

//            printf("g %d, gw %d, gv: %d, gv2: %d, ic: %d\n", g, inst[1][g], unfea->crom[g], fea->crom[g], idleCap);
            if(unfea->crom[g] == 0 )  printf("ERRO ERRO \n\n\n");   
            unfea->crom[g] = 0;
            unfea->f -= inst[F][g];
            unfea->w -= inst[W][g];

            fea->crom[g] = 1;
            fea->f += inst[F][g];
            fea->w += inst[W][g];
        }
    }while (g < INST_SIZE && unfea->w > MAX_CAPACITY);
  
    while (unfea->w > MAX_CAPACITY){
       for(g = 0; !(unfea->crom[g]); g++);
       int cheap = g;
       for(g++; g < INST_SIZE; g++){
          if(unfea->crom[g] &&  inst[F][g] < inst[F][cheap])
             cheap = g;
       }
       unfea->crom[cheap] = 0;
       unfea->f -= inst[F][cheap];
       unfea->w -= inst[W][cheap];
    }

}

void mutate(Individual * ind, int ** inst) {

   int muta = rand() % INST_SIZE;
   if (ind->crom[muta] ){
      ind->crom[muta] = 0;
      ind->f -= inst[F][muta];
      ind->w -= inst[W][muta];
   }else{
      ind->crom[muta] = 1;
      ind->f += inst[F][muta];
      ind->w += inst[W][muta];
   }
   while (ind->w > MAX_CAPACITY){
      int g;
      for(g = 0; !(ind->crom[g]); g++);
      int cheap = g;
      for(g++; g < INST_SIZE; g++){
         if(ind->crom[g] &&  inst[F][g] < inst[F][cheap])
            cheap = g;
      }
      ind->crom[cheap] = 0;
      ind->f -= inst[F][cheap];
      ind->w -= inst[W][cheap];
   }
}

void crossover(Individual *pop, int *parents, Individual * offspring, Individual **bestOffspring, Individual **worstOffspring,  int ** inst){
    int pcut,k,shot;

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
            offspring[i].f += offspring[i].crom[k] ? inst[F][k] : 0;
            offspring[i].w += offspring[i].crom[k] ? inst[W][k] : 0;
            
            offspring[i+1].crom[k] = pop[parents[i+1]].crom[k];
            offspring[i+1].f += offspring[i+1].crom[k] ? inst[F][k] : 0;
            offspring[i+1].w += offspring[i+1].crom[k] ? inst[W][k] : 0;
        }
        for(; k <  INST_SIZE; k++){
            offspring[i].crom[k] = pop[parents[i+1]].crom[k];
            offspring[i].f += offspring[i].crom[k] ? inst[F][k] : 0;
            offspring[i].w += offspring[i].crom[k] ? inst[W][k] : 0;

            offspring[i+1].crom[k] = pop[parents[i]].crom[k];
            offspring[i+1].f += offspring[i+1].crom[k] ? inst[F][k] : 0;
            offspring[i+1].w += offspring[i+1].crom[k] ? inst[W][k] : 0;
        }

        if(offspring[i].w > MAX_CAPACITY){ 
           repairOffspring(&offspring[i],&offspring[i+1],inst);
        }else if(offspring[i+1].w > MAX_CAPACITY){
           repairOffspring(&offspring[i+1],&offspring[i],inst);
        }    

        shot = rand() % MUT_DIVISOR;
        if (shot <= MUT_DIVIDENDO)  mutate(&offspring[i], inst);
        shot = rand() % MUT_DIVISOR;
        if (shot <= MUT_DIVIDENDO)  mutate(&offspring[i+1], inst);


        // printf("\n\n");

        if(!checkSolution(&(offspring[i]),inst))
           exit(0);
        if(!checkSolution(&(offspring[i+1]),inst))
           exit(0);


        if((*bestOffspring)->f < offspring[i].f)
           *bestOffspring = &offspring[i];
       
        if((*bestOffspring)->f < offspring[i+1].f)
           *bestOffspring = &offspring[i+1];

        if((*worstOffspring)->f > offspring[i].f)
           *worstOffspring = &offspring[i];
       
        if((*worstOffspring)->f > offspring[i+1].f)
           *worstOffspring = &offspring[i+1];
    }
}

void updatedPop(Individual **pop, Individual **offspring, Individual ** worstOffspring, Individual ** bestPop){
    
    if ((*worstOffspring)->f < (*bestPop)->f) {

        //printf("copied: ");
        //printf("Indivíduo bom = ");
        //for(int j = 0; j < INST_SIZE; j++){
        //    printf("%d ", (*bestPop)->crom[j]);
        //}     
        //printf("   fitness = %d   peso = %d \n", (*bestPop)->f , (*bestPop)->w);     

        //printf("to: ");
        //printf("Indivíduo ruim = ");
        //for(int j = 0; j < INST_SIZE; j++){
        //    printf("%d ", (*worstOffspring)->crom[j]);
        //}     
        //printf("   fitness = %d   peso = %d \n", (*worstOffspring)->f , (*worstOffspring)->w);     

        // deep copy the old best into the new worst
        for (int i = 0; i < INST_SIZE; i++) {
            (*worstOffspring)->crom[i] = (*bestPop)->crom[i];
        }
        (*worstOffspring)->f = (*bestPop)->f;
        (*worstOffspring)->w = (*bestPop)->w;

       // printf("novo Indivíduo ruim = ");
       // for(int j = 0; j < INST_SIZE; j++){
       //     printf("%d ", (*worstOffspring)->crom[j]);
       // }     
       // printf("   fitness = %d   peso = %d \n", (*worstOffspring)->f , (*worstOffspring)->w);     
    }

    Individual * aux;
    aux = *pop;
    *pop = *offspring;
    *offspring = aux;

}

int main(int argc, char ** argv){
    if (argc != 2) {
        printf("error: should provide 1 (ONE) file to read\nusage: %s filename", argv[0]);
        exit(1);
    }    

    int ** inst = readInstance(argv[1]);

    printf("c: %d, i: %d\n", MAX_CAPACITY, INST_SIZE);
    int parents[POP_SIZE]; 
    Individual * offspring = malloc(sizeof(Individual)*POP_SIZE);
    for (int i = 0; i < POP_SIZE; i++) {
        offspring[i].crom = malloc(INST_SIZE * sizeof(int));
    }
    // discutir o por que de ser Individual* ao invés de Individual
    Individual *bestPop, *bestOffspring, *worstOffspring;

//    int seed =  1694207772;
     unsigned seed = time(NULL);
  //  srand(seed);

    printf("Seed : %d\n",seed);

    //=========================== Genetic Algorithm =====================
    Individual * pop = create_pop(inst,&bestPop);
    //printPop(pop,inst);
    //printf("Best Individual:  fitness -> %d   weight -> %d\n",bestPop->f,bestPop->w);

    for (int i = 0; i < MAX_GENERATIONS; i++){
       printf(">>>>> %d\n",i);
       selection(pop,parents);
       crossover(pop,parents,offspring,&bestOffspring,&worstOffspring,inst);
       updatedPop(&pop,&offspring, &worstOffspring, &bestPop);
       printf("Best Individual:  fitness -> %d   weight -> %d\n",bestOffspring->f,bestOffspring->w);
       printf("Worst Individual: fitness -> %d   weight -> %d\n\n",worstOffspring->f,worstOffspring->w);
       }
    //printf("Seed : %d\n",seed);
    
    for (int i = 0; i < POP_SIZE; i++) {
        free(pop[i].crom);
        free(offspring[i].crom);
    }
    free(pop);
    free(offspring);
    free(inst[F]);
    free(inst[W]);
    free(inst);
}
