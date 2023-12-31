#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>

#define CROMOSSOME_LEN 6
#define POPULATION_SIZE 500
#define GENERATION_COUNT 40
#define TOURNAMENT_SIZE 8
#define MUTATION_RATE 0.0
#define MAX_CAPACITY 20 // kg

typedef struct Box{
    int value;
    int weight;
}Box;


Box newBox(int v, int w){
    Box b;
    b.value = v;
    b.weight = w;
    return b;
}

Box * get_boxes(){
    Box * boxes = malloc(sizeof(Box) * 6);
    boxes[0] = newBox(7, 10);
    boxes[1] = newBox(3, 5);
    boxes[2] = newBox(1, 2);
    boxes[3] = newBox(9, 11);
    boxes[4] = newBox(10, 15);
    boxes[5] = newBox(5, 7);
    return boxes;
} 

typedef int Cromossome[CROMOSSOME_LEN];

Cromossome * new_random_cromossome(){
    Cromossome * c = malloc(sizeof(Cromossome));
    for (int i = 0; i < CROMOSSOME_LEN; i++) {
        // random number either 1 or 0
        (*c)[i] = rand() % 2;
    }
    return c;
}

void print_cromossome(Cromossome * c){
    printf("[");
    for (int i = 0; i < CROMOSSOME_LEN; i++) {
        printf("% d", (*c)[i]);
    }
    printf(" ]\n");
}

int avaliate_cromossome(Cromossome * c){
    int value_sum = 0;
    int weight_sum = 0;
    Box * boxes = get_boxes();
    for (int i = 0; i < CROMOSSOME_LEN; i++) {
        if ((*c)[i] == 1){
            weight_sum += boxes[i].weight;
            if (weight_sum > MAX_CAPACITY){
                free(boxes);
                return 0;
            }
            value_sum += boxes[i].value;
        }
    }
    free(boxes);
    return value_sum;
}


void mutate_cromossome(Cromossome * c){
    for (int i = 0; i < CROMOSSOME_LEN; i++){
        // random float between 0 and 1
        float shot = (float)rand() / RAND_MAX;
        if (shot <= MUTATION_RATE) {
            if ((*c)[i] == 1){
                (*c)[i] = 0;
            } 
            else if ((*c)[i] == 0){
                (*c)[i] = 1;
            } 
        }
    }
    
}

void free_comossome(Cromossome *c){
    free(c);
}

typedef Cromossome* Population[POPULATION_SIZE];

Population * new_population(){
    Population * pop = malloc(sizeof(Population));
    for (int i = 0; i < POPULATION_SIZE; i++) {
        Cromossome * c = new_random_cromossome();
        (*pop)[i] = c;
    }
    return pop;
}

void free_population(Population * pop){
    for (int i = 0; i < POPULATION_SIZE; i++){
        free_comossome((*pop)[i]);
    }
    free(*pop);
}



typedef struct avaliated_cromossome {
    int avaliation;
    int index_in_pop;
}AV_Cromossome;

typedef AV_Cromossome AV_Population[POPULATION_SIZE];

AV_Population * avaliate_population(Population * pop){
    AV_Population * av_pop = malloc(sizeof(AV_Population));

    for (int i = 0; i < POPULATION_SIZE; i++) {
        int fitness = avaliate_cromossome((*pop)[i]);
        (*av_pop)[i].avaliation = fitness;
        (*av_pop)[i].index_in_pop = i;
    }
    return av_pop;
}

Cromossome * run_tournament(Population * pop, AV_Population * av_pop){
    AV_Cromossome participants[TOURNAMENT_SIZE];
    int best_score = 0;
    int best_index = 0;
    for (int i = 0; i<TOURNAMENT_SIZE; i++) {
        participants[i] = (*av_pop)[rand() % POPULATION_SIZE];
        if (participants[i].avaliation > best_score){
            best_index = participants[i].index_in_pop;
            best_score = participants[i].avaliation;
        }
    }
    return (*pop)[best_index];
}

Cromossome ** crossover_parents(Cromossome * p1, Cromossome * p2){
    Cromossome ** pair = malloc(sizeof(Cromossome*) * 2);
    Cromossome * child1 = malloc(sizeof(Cromossome));
    Cromossome * child2 = malloc(sizeof(Cromossome));
    for (int i = 0; i < CROMOSSOME_LEN / 2; i++) {
        (*child1)[i] = (*p2)[i + CROMOSSOME_LEN / 2];
        (*child2)[i] = (*p1)[i + CROMOSSOME_LEN / 2];

    }
    for (int i = 0; i < CROMOSSOME_LEN / 2; i++) {
        (*child1)[i + CROMOSSOME_LEN / 2] = (*p1)[i];
        (*child2)[i + CROMOSSOME_LEN /2] = (*p2)[i];
    }
    // p1, p2 are freed by the caller when g->pop is freed
    pair[0] = child1;
    pair[1] = child2;
    mutate_cromossome(pair[0]);
    mutate_cromossome(pair[1]);
    return pair;
}

typedef struct generation{
    Population * pop;
    AV_Population * av_pop;
}Generation;

void free_generation(Generation * g){
    free(g->av_pop);
    free_population(g->pop);
    free(g);
}

Generation * reproduce_generation(Generation * g){
    Population * offspring = malloc(sizeof(Population));
    for (int i = 0; i<POPULATION_SIZE / 2; i++) {
        Cromossome * parent1 = run_tournament(g->pop,g->av_pop);
        Cromossome * parent2 = run_tournament(g->pop,g->av_pop);

        Cromossome ** children = crossover_parents(parent1,parent2);
        (*offspring)[i] = children[0];
        (*offspring)[i + POPULATION_SIZE / 2] = children[1];
        free(children);
    }
    Generation * gen = malloc(sizeof(Generation));
    free_generation(g);
    gen->pop = offspring;
    gen->av_pop = avaliate_population(offspring);
    return gen;
}

Cromossome * best_individual(Generation * g){
    int best_index = 0;
    int best_value = 0;
    AV_Population * av_pop = g->av_pop;
    Population * pop = g->pop;
    for (int i = 0; i<POPULATION_SIZE; i++) {
        if ((*av_pop)[i].avaliation > best_value) {
            best_index = (*av_pop)[i].index_in_pop;
        }
    }
    return (*pop)[best_index];
}


int main(){
    srand( time(NULL) );
    
    Generation * current = malloc(sizeof(Generation));
    Population * p = new_population();
    current->pop = p;
    current->av_pop = avaliate_population(p);

    Cromossome * b1 = best_individual(current);
    print_cromossome(b1);
    printf("Value: %d\n", avaliate_cromossome(b1));

    for (int i = 0; i < GENERATION_COUNT; i++){
        current = reproduce_generation(current);
    }

    Cromossome * b2 = best_individual(current);
    print_cromossome(b2);
    printf("Value: %d\n", avaliate_cromossome(b2));

    free_generation(current);
}
