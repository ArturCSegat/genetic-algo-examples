#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define CROMOSSOME_LEN 6
#define POPULATION_SIZE 50
#define GENERATION_COUNT 40
#define TOURNAMENT_SIZE 8
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
    boxes[2] = newBox(1, 3);
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
        free((*pop)[i]);
    }
    free(pop);
}

int avaliate_cromossome(Cromossome * c){
    int value_sum = 0;
    int weight_sum = 0;
    Box * boxes = get_boxes();
    for (int i = 0; i < CROMOSSOME_LEN; i++) {
        if ((*c)[i] == 1){
            weight_sum += boxes[i].weight;
            if (weight_sum > MAX_CAPACITY){
                return 0;
            }
            value_sum += boxes[i].value;
        }
    }
    free(boxes);
    return value_sum;
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
    int best_index;
    for (int i = 0; i<TOURNAMENT_SIZE; i++) {
        participants[i] = (*av_pop)[rand() % POPULATION_SIZE];
        if (participants[i].avaliation > best_score){
            best_index = participants[i].index_in_pop;
            best_score = participants[i].avaliation;
        }
    }
    return (*pop)[best_index];
}

void crossover_parents(Cromossome * p1, Cromossome * p2){
    for (int i = 0; i < CROMOSSOME_LEN / 2; i++) {
        int temp = (*p1)[i];
        (*p1)[i] = (*p2)[i + CROMOSSOME_LEN / 2];
        (*p2)[i + CROMOSSOME_LEN / 2] =  temp;
    }
    for (int i = 0; i < CROMOSSOME_LEN / 2; i++) {
        int temp = (*p2)[i];
        (*p2)[i] = (*p1)[i + CROMOSSOME_LEN / 2];
        (*p1)[i + CROMOSSOME_LEN / 2] =  temp;
    }
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
        Cromossome * parrent1 = run_tournament(g->pop, g->av_pop);
        Cromossome * parrent2 = run_tournament(g->pop, g->av_pop);
        crossover_parents(parrent1, parrent2);
        (*offspring)[i] = parrent1;
        (*offspring)[i + POPULATION_SIZE / 2] = parrent2;
    }
    Generation * gen = malloc(sizeof(Generation));
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
    Generation * first_gen = malloc(sizeof(Generation));
    Population * p = new_population();
    first_gen->pop  = p;
    first_gen->av_pop = avaliate_population(p);
    Cromossome * best = best_individual(first_gen);
    print_cromossome(best);
    printf("Best: %d\n", avaliate_cromossome(best));
    for (int i = 0; i<GENERATION_COUNT; i++) {
        first_gen = reproduce_generation(first_gen);
    }
    Cromossome * best_2 = best_individual(first_gen);
    print_cromossome(best_2);
    printf("Best: %d\n", avaliate_cromossome(best_2));
    free(first_gen->av_pop);
    free(first_gen->pop);
    free(first_gen);
}
