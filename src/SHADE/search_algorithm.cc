/*
  SHADE 1.1 implemented by C++ for Special Session & Competition on Real-Parameter Single Objective Optimization at CEC-2014

  Version: 1.1   Date: 9/Jun/2014
  Written by Ryoji Tanabe (rt.ryoji.tanabe [at] gmail.com)
*/

#include"de.h"

searchAlgorithm::searchAlgorithm(CEvaluation<CRealCoding>* evaluation)
{
  this->evaluation = evaluation;
  evaluation_fenotype = evaluation->pcCreateSampleFenotype();
}

searchAlgorithm::~searchAlgorithm()
{
  delete evaluation_fenotype;
}

void searchAlgorithm::initializeParameters() {
  problem_size = (int)evaluation->iGetNumberOfElements();
  max_num_evaluations = INT_MAX;
  initializeFitnessFunctionParameters();
}

void searchAlgorithm::evaluatePopulation(const vector<Individual> &pop, vector<Fitness> &fitness) {
  for (int i = 0; i < pop_size; i++) {
	evaluation_fenotype->vSetValues(pop[i]);
	fitness[i] = -evaluation->dEvaluate(evaluation_fenotype);
  }
}

void searchAlgorithm::initializeFitnessFunctionParameters() {
  //epsilon is an acceptable error value.
  epsilon = pow(10.0, -8);

  max_region = *evaluation_fenotype->pdGetMaxValues();
  min_region = *evaluation_fenotype->pdGetMinValues();

  optimum = -evaluation->dGetMaxValue();
}

//set best solution (bsf_solution) and its fitness value (bsf_fitness) in the initial population
void searchAlgorithm::setBestSolution(const vector<Individual> &pop, const vector<Fitness> &fitness, Individual &bsf_solution, Fitness &bsf_fitness) {
  int current_best_individual = 0;

  for (int i = 1; i < pop_size; i++) {
    if (fitness[current_best_individual] > fitness[i]) {
      current_best_individual = i;
    }
  }

  bsf_fitness = fitness[current_best_individual];
  for (int i = 0; i < problem_size; i++) {
    bsf_solution[i] = pop[current_best_individual][i];
  }
}

// make new individual randomly
Individual searchAlgorithm::makeNewIndividual() {
  Individual individual = (variable*)malloc(sizeof(variable) * problem_size);

  for (int i = 0; i < problem_size; i++) {
    individual[i] = ((max_region - min_region) * randDouble()) + min_region;
  }

  return individual;
}

/*
  For each dimension j, if the mutant vector element v_j is outside the boundaries [x_min , x_max], we applied this bound handling method
  If you'd like to know that precisely, please read:
  J. Zhang and A. C. Sanderson, "JADE: Adaptive differential evolution with optional external archive,"
  IEEE Tran. Evol. Comput., vol. 13, no. 5, pp. 945–958, 2009.
 */
void searchAlgorithm::modifySolutionWithParentMedium(Individual child, Individual parent) {
  int l_problem_size = problem_size;
  variable l_min_region = min_region;
  variable l_max_region = max_region;

  for (int j = 0; j < l_problem_size; j++) {
    if (child[j] < l_min_region) {
      child[j]= (l_min_region + parent[j]) / 2.0;
    }
    else if (child[j] > l_max_region) {
      child[j]= (l_max_region + parent[j]) / 2.0;
    }
  }
}

