/*===============================================================*/
/*                                                               */
/*                       check_result.cpp                        */
/*                                                               */
/*      Software evaluation of training and test error rate      */
/*                                                               */
/*===============================================================*/

#include <fstream>
#include "typedefs.h"

// data structure only used in this file
typedef struct DataSet_s 
{
  DataType*    data_points;
  LabelType*   labels;
  FeatureType* parameter_vector;
  size_t num_data_points;
  size_t num_features;
} DataSet;


// sub-functions for result checking
// dot product
float dotProduct(FeatureType* param_vector, DataType* data_point_i, const size_t num_features)
{
  FeatureType result = 0.0f;

  for (int i = 0; i < num_features; i ++ )
    result += param_vector[i] * data_point_i[i];

  #ifndef SW
    return result.to_float();
  #else
    return result;
  #endif
}

// predict
LabelType getPrediction(FeatureType* parameter_vector, DataType* data_point_i, size_t num_features, const double treshold = 0) 
{
  float parameter_vector_dot_x_i = dotProduct(parameter_vector, data_point_i, num_features);
  return (parameter_vector_dot_x_i > treshold) ? 1 : 0;
}

// compute error rate
double computeErrorRate(
    DataSet data_set,
    double& cumulative_true_positive_rate,
    double& cumulative_false_positive_rate,
    double& cumulative_error)
{

  size_t true_positives = 0, true_negatives = 0, false_positives = 0, false_negatives = 0;

  for (size_t i = 0; i < data_set.num_data_points; i++) 
  {
    LabelType prediction = getPrediction(data_set.parameter_vector, &data_set.data_points[i * data_set.num_features], data_set.num_features);
    if (prediction != data_set.labels[i])
    {
      if (prediction == 1)
        false_positives++;
      else
        false_negatives++;
    } 
    else 
    {
      if (prediction == 1)
        true_positives++;
      else
        true_negatives++;
    }
  }

  double error_rate = (double)(false_positives + false_negatives) / data_set.num_data_points;

  cumulative_true_positive_rate += (double)true_positives / (true_positives + false_negatives);
  cumulative_false_positive_rate += (double)false_positives / (false_positives + true_negatives);
  cumulative_error += error_rate;

  return error_rate;
}

// check results
void check_results(FeatureType* param_vector, DataType* data_points, LabelType* labels)
{
  std::ofstream ofile;
  ofile.open("output.txt");
  if (ofile.is_open())
  {
    ofile << "\nmain parameter vector: \n";
    for(int i = 0; i < 30; i ++ )
    #ifndef SW
      ofile << "m[" << i << "]: " << param_vector[i].to_float() << " | ";
    #else
      ofile << "m[" << i << "]: " << param_vector[i] << " | ";
    #endif
    ofile << std::endl;

    // Initialize benchmark variables
    double training_tpr = 0.0;
    double training_fpr = 0.0;
    double training_error = 0.0;
    double testing_tpr = 0.0;
    double testing_fpr = 0.0;
    double testing_error = 0.0;

    // Get Training error
    DataSet training_set;
    training_set.data_points = data_points;
    training_set.labels = labels;
    training_set.num_data_points = NUM_TRAINING;
    training_set.num_features = NUM_FEATURES;
    training_set.parameter_vector = param_vector;
    computeErrorRate(training_set, training_tpr, training_fpr, training_error);

    // Get Testing error
    DataSet testing_set;
    testing_set.data_points = &data_points[NUM_FEATURES * NUM_TRAINING];
    testing_set.labels = &labels[NUM_TRAINING];
    testing_set.num_data_points = NUM_TESTING;
    testing_set.num_features = NUM_FEATURES;
    testing_set.parameter_vector = param_vector;
    computeErrorRate(testing_set, testing_tpr, testing_fpr, testing_error);

    training_tpr *= 100.0;
    training_fpr *= 100.0;
    training_error *= 100.0;
    testing_tpr *= 100.0;
    testing_fpr *= 100.0;
    testing_error *= 100.0;

    ofile << "Training TPR: " << training_tpr << std::endl; 
    ofile << "Training FPR: " << training_fpr << std::endl; 
    ofile << "Training Error: " << training_error << std::endl; 
    ofile << "Testing TPR: " << testing_tpr << std::endl; 
    ofile << "Testing FPR: " << testing_fpr << std::endl; 
    ofile << "Testing Error: " << testing_error << std::endl; 
  }
  else
  {
    std::cout << "Failed to create output file!" << std::endl;
  }
}
