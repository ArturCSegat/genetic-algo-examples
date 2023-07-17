import random

def avaliate(cromossome):
    counter = 0
    sum = 0
    for gene in cromossome:
        if counter % 2 == 0:
            sum += gene
        counter += 1
        
    return sum / CROMOSSOME_SIZE


CROMOSSOME_SIZE = 32
POPULATION_SIZE = 100
GENERATION_COUNT = 80

def generate_population():
    pop = [[] for _ in range(POPULATION_SIZE)]
    for i in range(0, POPULATION_SIZE):
        cromossome = [random.randint(0 ,1) for _ in range(CROMOSSOME_SIZE)]
        pop[i] = cromossome
    return pop


def avaliate_population(population):
    avaliated_population = []
    for i in range(0, POPULATION_SIZE):
        avaliated_population.append((i, avaliate(population[i])))
    return avaliated_population


def tournament(population, avaliated_population, tournament_size=4):
    participants = random.sample(avaliated_population, tournament_size)
    winner = max(participants, key= lambda x: x[1])
    winner_comossome = population[winner[0]]
    return winner_comossome


def combine_parents(p1, p2):
    child1 = [0 for _ in range(CROMOSSOME_SIZE)]
    child2 = [0 for _ in range(CROMOSSOME_SIZE)]

    for i in range(0, len(p1)):
        if i < CROMOSSOME_SIZE // 2:
            child2[i] = p1[i]
        else:
            child1[i] = p1[i]

    for i in range(0, len(p2)):
        if i < CROMOSSOME_SIZE // 2:
            child1[i] = p2[i]
        else:
            child2[i] = p2[i]

    return child1, child2


def generate_generation(population, avaliated_population):
    offspring = []
    while len(offspring) < POPULATION_SIZE:
        parent1 = tournament(population, avaliated_population)
        parent2 = tournament(population, avaliated_population)
        while parent2 == parent1:
            parent2 = tournament(population, avaliated_population)
        child1, child2 = combine_parents(parent1, parent2)
        offspring.append(child1)
        offspring.append(child2)
    return offspring, avaliate_population(offspring)


def calculate_average(avaliated_population):
    return sum(value[1] for value in avaliated_population) / len(avaliated_population)


pop = generate_population()
av_pop = avaliate_population(pop)
for i in range(GENERATION_COUNT):
    print(str(i) + ": " + str(calculate_average(av_pop)))
    pop, av_pop = generate_generation(pop, av_pop)
