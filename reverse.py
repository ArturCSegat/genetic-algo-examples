import random
import numpy as np
import matplotlib.pyplot as plt

CROMOSSOME_SIZE = 32
POPULATION_SIZE = 500
GENERATION_COUNT = 40
MUTATION_RATE = 0.0001
DEATH_RATIO = 3.5

def avaliate(decoded_cromossome):
    absolute_cosine = np.abs(np.cos(1.5 * decoded_cromossome))
    quadratic_term = 0.001 * decoded_cromossome ** 2
    fitness_value = absolute_cosine - quadratic_term
    return fitness_value


def cromossome_to_decimal(cromossome):
    return int(''.join(map(str, cromossome)), 2)


def decode(decimal_cromossome):
    circle_ratio = 4 * np.pi
    binary_max = int('1' * CROMOSSOME_SIZE, 2)
    scaling_factor = circle_ratio / binary_max
    return scaling_factor * decimal_cromossome - (2 * np.pi)


def mutate_cromossome(cromossome):
    decimal_points = len(str(MUTATION_RATE).split(".")[1])
    for gene_i in range(len(cromossome)):
        shot = round(random.uniform(1/pow(10, decimal_points), 1), decimal_points)
        if shot <= MUTATION_RATE:
            if cromossome[gene_i] == 1:
                cromossome[gene_i] = 0
            elif cromossome[gene_i] == 0:
                cromossome[gene_i] = 1


def generate_population():
    pop = [[] for _ in range(POPULATION_SIZE)]
    for i in range(0, POPULATION_SIZE):
        cromossome = [random.randint(0 ,1) for _ in range(CROMOSSOME_SIZE)]
        pop[i] = cromossome
    return pop


def avaliate_population(population):
    avaliated_population = []
    for i in range(0, POPULATION_SIZE):
            avaliated_population.append([avaliate(decode(cromossome_to_decimal(population[i]))), i])
    return avaliated_population


def tournament(population, avaliated_population, tournament_size=4):
    participants = []
    while len(participants) < tournament_size:
        # using indexes to ignore dead individuals
        index = random.randint(0, POPULATION_SIZE - 1)
        p = avaliated_population[index]
        if p[1] == None:
            continue
        participants.append(index)

    winner = max(participants, key=lambda x: avaliated_population[x][0])
    loser = min(participants, key=lambda x: avaliated_population[x][0])
    
    # if loser's score is less than DR fractoin of the winner's kill the loser
    if avaliated_population[loser][0] * DEATH_RATIO < avaliated_population[winner][0]:
       avaliated_population[loser][1] = None

    winner_comossome = population[avaliated_population[winner][1]]
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
        child1, child2 = combine_parents(parent1, parent2)
        offspring.append(child1)
        offspring.append(child2)


    return offspring, avaliate_population(offspring)


def calculate_average(avaliated_population):
    return sum(value[0] for value in avaliated_population) / len(avaliated_population)


x = np.arange(-2 * np.pi, 2 * np.pi, 0.01)
y = avaliate(x) 

plt.ion()
plt.plot(x, y)
plt.xlabel('x')
plt.ylabel('avliated x')
plt.title('generic genetic algo')
plt.grid(True)
plt.show()


pop = generate_population()
av_pop = avaliate_population(pop)
for i in range(GENERATION_COUNT):
    average = calculate_average(av_pop)
    print(str(i) + ": " + str(average))

    plt.clf()
    plt.plot(x, y)
    for ind in pop:
        decoded_value = decode(cromossome_to_decimal(ind))
        fitness = avaliate(decoded_value)
        plt.plot(decoded_value, fitness, 'ro', markersize=5)
    plt.xlabel('x')
    plt.ylabel('Evaluated x')
    plt.title('Generic Genetic Algorithm - Generation ' + str(i) + ' - Média ' + str(average))
    plt.grid(True)
    plt.draw()
    plt.pause(0.5)

    pop, av_pop = generate_generation(pop, av_pop)

plt.ioff()
plt.show()

