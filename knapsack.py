import random

class Box:
    def __init__(self, value, weight):
        self.value = value
        self.weight = weight

# globals
MAX_CAPACITY = 20 # kg
CROMOSSOME_SIZE = 6
POPULATION_SIZE = 50
GENERATION_COUNT = 50
MUTATION_RATE = 0.05
BOXES = [Box(7, 10), Box(3, 5), Box(1, 3), Box(9, 11), Box(10, 15), Box(5,7)]


def avaliate_cromososme(cromossome: list):
    value_sum = 0
    current_weight = 0
    for i in range(len(cromossome)):
        value = BOXES[i].value
        weight = BOXES[i].weight
        if cromossome[i] == 1:
            current_weight += weight 
            if current_weight > MAX_CAPACITY:
                return 0
            value_sum += value
    return value_sum


def random_cromossome():
    return [random.randint(0, 1) for _ in range(CROMOSSOME_SIZE)]

def create_population():
    return [random_cromossome() for _ in range(POPULATION_SIZE)]

def avaliate_population(population: list):
    return [(avaliate_cromososme(population[i]), i) for i in range(len(population))]

def mutate_cromossome(cromossome):
    decimal_points = len(str(MUTATION_RATE).split(".")[1])
    for gene_i in range(len(cromossome)):
        shot = round(random.uniform(1/pow(10, decimal_points), 1), decimal_points)
        if shot <= MUTATION_RATE:
            if cromossome[gene_i] == 1:
                cromossome[gene_i] = 0
            elif cromossome[gene_i] == 0:
                cromossome[gene_i] = 1

def tournament(pop, av_pop, tournament_size=10):
    participants = random.sample(av_pop, tournament_size)
    winner = max(participants, key= lambda x: x[0])
    winner_comossome = pop[winner[1]]
    return winner_comossome

def pair_cromossomes(p1, p2):
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
    
    mutate_cromossome(child1)
    mutate_cromossome(child2)
    return child1, child2


def generate_generation(population, avaliated_population):
    offspring = []
    while len(offspring) < POPULATION_SIZE:
        parent1 = tournament(population, avaliated_population)
        parent2 = tournament(population, avaliated_population)
        while parent2 == parent1:
            parent2 = tournament(population, avaliated_population)
        child1, child2 = pair_cromossomes(parent1, parent2)
        offspring.append(child1)
        offspring.append(child2)


    return offspring, avaliate_population(offspring)

def calculate_best():
    pop = create_population()
    av_pop = avaliate_population(pop)
    for _ in range(GENERATION_COUNT):
        pop, av_pop = generate_generation(pop, av_pop)
    best = pop[max(av_pop, key= lambda x: x[0])[1]]
    print(f"The best solution found was: {best} with a value of {avaliate_cromososme(best)}")

calculate_best()
