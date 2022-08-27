/*
  SHADE 1.1 implemented by C++ for Special Session & Competition on Real-Parameter Single Objective Optimization at CEC-2014

  Version: 1.1   Date: 9/Jun/2014
  Written by Ryoji Tanabe (rt.ryoji.tanabe [at] gmail.com)
*/

#ifndef _HEADER_H_
#define _HEADER_H_

#define PI 3.1415926535897932384626433832795029

#include <stdlib.h>
#include<iostream>
#include <iomanip>
#include <string.h>
#include <vector>
#include<math.h>

#include "../EATester/Evaluation.h"
#include "../EATester/RealCoding.h"

#include "../EATester/GenePattern.h"

using namespace std;

typedef  double variable;
typedef variable *Individual;
typedef  double Fitness;

class searchAlgorithm {
public:
  searchAlgorithm(CEvaluation<CRealCoding> *evaluation);

  virtual ~searchAlgorithm();

  virtual void initialize() = 0;
  virtual void runIteration(vector<CGenePattern*> *patterns) = 0;

  virtual Fitness run() = 0;

  int getPopSize() { return pop_size; }

  void setPopSize(int pop_size) { this->pop_size = pop_size; }

protected:
  void evaluatePopulation(const vector<Individual> &pop, vector<Fitness> &fitness);
  void initializeFitnessFunctionParameters();

  void initializeParameters();
  Individual makeNewIndividual();
  void modifySolutionWithParentMedium(Individual child, Individual parent);
  void setBestSolution(const vector<Individual> &pop, const vector<Fitness> &fitness, Individual &bsf_solution, Fitness &bsf_fitness);

  //Return random value with uniform distribution [0, 1)
  inline double randDouble() {
    return (double)rand() / (double) RAND_MAX;
  }

  /*
    Return random value from Cauchy distribution with mean "mu" and variance "gamma"
    http://www.sat.t.u-tokyo.ac.jp/~omi/random_variables_generation.html#Cauchy
  */
  inline double cauchy_g(double mu, double gamma) {
    return mu + gamma * tan(PI*(randDouble() - 0.5));
  }

  /*
    Return random value from normal distribution with mean "mu" and variance "gamma"
    http://www.sat.t.u-tokyo.ac.jp/~omi/random_variables_generation.html#Gauss
  */
  inline double gauss(double mu, double sigma){
    return mu + sigma * sqrt(-2.0 * log(randDouble())) * sin(2.0 * PI * randDouble());
  }

  //Recursive quick sort with index array
  template<class VarType>
    void sortIndexWithQuickSort(VarType array[], int first, int last, int index[]) {
    VarType x = array[(first + last) / 2];
    int i = first;
    int j = last;
    VarType temp_var = 0;
    int temp_num = 0;

    while (true) {
      while (array[i] < x) i++;    
      while (x < array[j]) j--;      
      if (i >= j) break;

      temp_var = array[i];
      array[i] = array[j];
      array[j] = temp_var;

      temp_num = index[i];
      index[i] = index[j];
      index[j] = temp_num;

      i++;
      j--;
    }

    if (first < (i -1)) sortIndexWithQuickSort(array, first, i - 1, index);  
    if ((j + 1) < last) sortIndexWithQuickSort(array, j + 1, last, index);    
  }
  
  int problem_size;
  variable max_region;
  variable min_region;
  int max_num_evaluations;
  Fitness optimum;
  // acceptable error value
  Fitness epsilon;
  int pop_size;

  CEvaluation<CRealCoding> *evaluation;
  CRealCoding *evaluation_fenotype;
};

class SHADE: public searchAlgorithm {
public:
  SHADE(CEvaluation<CRealCoding> *evaluation);
  virtual ~SHADE();

  virtual void initialize();
  virtual void runIteration(vector<CGenePattern*> *patterns);

  virtual Fitness run();
  void setSHADEParameters();
  void operateCurrentToPBest1BinWithArchive(const vector<Individual> &pop, Individual child, int &target, int &p_best_individual, variable &scaling_factor, variable &cross_rate, const vector<Individual> &archive, int &arc_ind_count, CGenePattern *pattern);

  double getArcRate() { return arc_rate; }
  variable getPBestRate() { return p_best_rate; }
  int getMemorySize() { return memory_size; }

  void setArcRate(double arc_rate) { this->arc_rate = arc_rate; }
  void setPBestRate(variable p_best_rate) { this->p_best_rate = p_best_rate; }
  void setMemorySize(int memory_size) { this->memory_size = memory_size; }

  Individual getBSFSolution() { return bsf_solution; }
  Fitness getBSFFitness() { return bsf_fitness; }

private:
  void clear();

  int arc_size;
  double arc_rate;
  variable p_best_rate;
  int memory_size;

  vector <Individual> pop;
  vector <Fitness> fitness;
  vector <Individual> children;
  vector <Fitness> children_fitness;

  Individual bsf_solution;
  Fitness bsf_fitness;

  //for external archive
  int arc_ind_count;
  int random_selected_arc_ind;
  vector <Individual> archive;

  int num_success_params;
  vector <variable> success_sf;
  vector <variable> success_cr;
  vector <variable> dif_fitness;

  vector <variable> memory_sf;
  vector <variable> memory_cr;

  variable temp_sum_sf;
  variable temp_sum_cr;
  variable sum;
  variable weight;

  //memory index counter
  int memory_pos;

  //for new parameters sampling
  variable mu_sf, mu_cr;
  int random_selected_period;
  variable *pop_sf;
  variable *pop_cr;

  //for current-to-pbest/1
  int p_best_ind;
  int p_num;
  int *sorted_array;
  Fitness *temp_fit;
};

#endif


