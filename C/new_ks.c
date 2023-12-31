#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>

#define CROMOSSOME_LEN 10
#define POPULATION_SIZE 5000
#define GENERATION_COUNT 400
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
    Box * boxes = malloc(sizeof(Box) * CROMOSSOME_LEN);
    boxes[0] = newBox(7, 10);
    boxes[1] = newBox(3, 5);
    boxes[2] = newBox(1, 2);
    boxes[3] = newBox(9, 11);
    boxes[4] = newBox(10, 15);
    boxes[5] = newBox(5, 7);
    boxes[6] = newBox(7, 10);
    boxes[7] = newBox(3, 5);
    boxes[8] = newBox(1, 2);
    boxes[9] = newBox(9, 11);
    return boxes;
} 

typedef int Cromossome[CROMOSSOME_LEN];

Cromossome * new_random_cromossome(){
    Cromossome * c = malloc(sizeof(Cromossome));
    for (int i = 0; i < CROMOSSOME_LEN; i++) {
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


typedef struct generation{
    Population * pop;
    AV_Population * av_pop;
}Generation;

void avaliate_generation(Generation * g){
    for (int i = 0; i < POPULATION_SIZE; i++) {
        int fitness = avaliate_cromossome((*g->pop)[i]);
        (*g->av_pop)[i].avaliation = fitness;
        (*g->av_pop)[i].index_in_pop = i;
    }
}

void free_generation(Generation * g){
    free(g->av_pop);
    free_population(g->pop);
    free(g);
}

int run_tournament(Generation * g){
    int best_score = 0;
    int best_index = 0;
    for (int i = 0; i<TOURNAMENT_SIZE; i++) {
        AV_Cromossome participant = (*g->av_pop)[rand() % POPULATION_SIZE];
        if (participant.avaliation > best_score){
            best_index = participant.index_in_pop;
            best_score = participant.avaliation;
        }
    }
    return best_index;
}

void crossover_parents(Cromossome * p1, Cromossome * p2){
    for (int i = 0; i < CROMOSSOME_LEN / 2; i++) {
        int temp = (*p1)[i + CROMOSSOME_LEN / 2];
        (*p1)[i + CROMOSSOME_LEN / 2] = (*p2)[i + CROMOSSOME_LEN / 2];
        (*p2)[i + CROMOSSOME_LEN /2] = temp;
    }
    mutate_cromossome(p1);
    mutate_cromossome(p2);
}

void copy_cromossome(Cromossome * src, Cromossome * dst) {
    for (int i = 0; i < CROMOSSOME_LEN; i++) {
        (*dst)[i] = (*src)[i];
    }
}


void reproduce_generation(Generation * g, Population * buffer, Cromossome * swap_buffer[2]){
    for (int i = 0; i<POPULATION_SIZE / 2; i++) {
        int p1_idx = run_tournament(g);
        int p2_idx = run_tournament(g);
        Cromossome * parent1 = (*g->pop)[p1_idx];
        Cromossome * parent2 = (*g->pop)[p2_idx];

        copy_cromossome(parent1, (*buffer)[i]);
        copy_cromossome(parent2, (*buffer)[i + POPULATION_SIZE / 2]);

        // modifies parents in g
        crossover_parents(parent1, parent2);  
        
        // save the modified value to use in the swap
        copy_cromossome(parent1, swap_buffer[0]);
        copy_cromossome(parent2, swap_buffer[1]);
        
        // swaps the pointers of the saved buffer and the main generation 
        // to return g to its original state
        // stores modified parents in buffer

        copy_cromossome((*buffer)[i], (*g->pop)[p1_idx]);
        copy_cromossome((*buffer)[i + POPULATION_SIZE / 2], (*g->pop)[p2_idx]);

        copy_cromossome(swap_buffer[0], (*buffer)[i]);
        copy_cromossome(swap_buffer[1], (*buffer)[i + POPULATION_SIZE / 2]);
    }
    // updates the av_pop of g with the new pop
    g->pop = buffer;
    avaliate_generation(g);
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

    // main gen stores the current individuals
    Generation * current = malloc(sizeof(Generation));
    Population * p = new_population();
    current->pop = p;
    current->av_pop = malloc(sizeof(AV_Population));
    avaliate_generation(current);

    // used as scaffolding for building the next main gen
    Population * buffer = malloc(sizeof(Population));
    for (int i = 0; i < POPULATION_SIZE; i++) {
        (*buffer)[i] = malloc(sizeof(Cromossome));
    }
    Cromossome * swap_buffer[2] = {malloc(sizeof(Cromossome)), malloc(sizeof(Cromossome))};

    Cromossome * b1 = best_individual(current);
    print_cromossome(b1);
    printf("Value: %d\n", avaliate_cromossome(b1));

    for (int i = 0; i < GENERATION_COUNT; i++){
        reproduce_generation(current, buffer, swap_buffer);
    }

    Cromossome * b2 = best_individual(current);
    print_cromossome(b2);
    printf("Value: %d\n", avaliate_cromossome(b2));

    free_generation(current);
    free(swap_buffer[0]);
    free(swap_buffer[1]);
}
