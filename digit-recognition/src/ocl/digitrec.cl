/*===============================================================*/
/*                                                               */
/*                          digitrec.cl                          */
/*                                                               */
/*             Hardware function for digit recognition           */
/*                                                               */
/*===============================================================*/

#include "../host/typedefs.h"

// ocl wide vector type
typedef ulong4 WholeDigitType;

// 64-bit popcount function
int popcount64(unsigned long x)
{
  // most straightforward implementation
  // actually not bad on FPGA
  int cnt = 0;
  unsigned long number = x;
  for (int i = 0; i < 64; i ++ )
  {
    cnt = cnt + (number & 0x1);
    number = number >> 1;
  }

  return cnt;

}

// Given the test instance and a (new) training instance, this
// function maintains/updates an array of K minimum
// distances per training set.
void update_knn( WholeDigitType test_inst, WholeDigitType train_inst, int min_distances[K_CONST] ) 
{
  // Compute the difference using XOR
  WholeDigitType diff = test_inst ^ train_inst;

  int dist = 0;
  // call popcount functions to compute hamming distance
  dist = popcount64(diff.x) + popcount64(diff.y) + popcount64(diff.z) + popcount64(diff.w);

  int max_dist = 0;
  int max_dist_id = K_CONST+1;
  int k = 0;

  // Find the max distance
  FIND_MAX_DIST: for ( int k = 0; k < K_CONST; ++k ) 
  {
    if ( min_distances[k] > max_dist ) 
    {
      max_dist = min_distances[k];
      max_dist_id = k;
    }
  }

  // Replace the entry with the max distance
  if ( dist < max_dist )
    min_distances[max_dist_id] = dist;

  return;
}


// Given 10xK minimum distance values, this function
// finds the actual K nearest neighbors and determines the
// final output based on the most common int represented by
// these nearest neighbors (i.e., a vote among KNNs).
LabelType knn_vote( int knn_set[PAR_FACTOR * K_CONST] ) 
{
  // local buffers

  // final K nearest neighbors
  int min_distance_list[K_CONST] __attribute__((xcl_array_partition(complete, 1)));
  // labels of the final K nearest neighbors
  int label_list[K_CONST]  __attribute__((xcl_array_partition(complete, 1)));
  // voting boxes
  int vote_list[10]  __attribute__((xcl_array_partition(complete, 1)));

  int pos = 1000;

  // initialize
  __attribute__((opencl_unroll_hint))
  INIT_1: for (int i = 0;i < K_CONST; i ++ )
  {
    min_distance_list[i] = 256;
    label_list[i] = 9;
  }

  __attribute__((opencl_unroll_hint))
  INIT_2: for (int i = 0;i < 10; i ++ )
  {
    vote_list[i] = 0;
  }

  // go through all the lanes
  // do an insertion sort to keep a sorted neighbor list
  LANES: for (int i = 0; i < PAR_FACTOR; i ++ )
  {
    INSERTION_SORT_OUTER: for (int j = 0; j < K_CONST; j ++ )
    {
      pos = 1000;
      INSERTION_SORT_INNER: for (int r = 0; r < K_CONST; r ++ )
      {
        pos = ((knn_set[i*K_CONST+j] < min_distance_list[r]) && (pos > K_CONST)) ? r : pos;
      }
      INSERT: for (int r = K_CONST ;r > 0; r -- )
      {
        if(r-1 > pos)
        {
          min_distance_list[r-1] = min_distance_list[r-2];
          label_list[r-1] = label_list[r-2];
        }
        else if (r-1 == pos)
        {
          min_distance_list[r-1] = knn_set[i*K_CONST+j];
          label_list[r-1] = i / (PAR_FACTOR / 10);
        }
      }
    }
  }

  // vote
  INCREMENT: for (int i = 0;i < K_CONST; i ++ )
    vote_list[label_list[i]] += 1;

  LabelType max_vote;
  max_vote = 0;

  // find the maximum value
  __attribute__((opencl_unroll_hint))
  VOTE: for (int i = 0;i < 10; i ++ )
  {
    if(vote_list[i] >= vote_list[max_vote])
    {
      max_vote = i;
    }
  }

  return max_vote;

}

// top-level function
__attribute__ ((reqd_work_group_size(1, 1, 1)))
__kernel void DigitRec(__global WholeDigitType * global_training_set, __global WholeDigitType * global_test_set, __global LabelType* global_results) 
{
  event_t results_copy;
  event_t data_copy[2];

  // This array stores K minimum distances per training set
  int knn_set[PAR_FACTOR * K_CONST] __attribute__((xcl_array_partition(complete, 1)));

  // local copies of arrays
  __local WholeDigitType training_set [NUM_TRAINING] __attribute__((xcl_array_partition(block, PAR_FACTOR, 1)));
  __local WholeDigitType test_set     [NUM_TEST];
  __local LabelType results           [NUM_TEST];

  data_copy[0] = async_work_group_copy(training_set, global_training_set, NUM_TRAINING, 0);
  data_copy[1] = async_work_group_copy(test_set,     global_test_set,     NUM_TEST,  0);

  wait_group_events(2, data_copy);

  // loop through test set
  TEST_LOOP: for (int t = 0; t < NUM_TEST; ++t) 
  {
    // fetch one instance
    WholeDigitType test_instance = test_set[t];

    // Initialize the knn set
    __attribute__((opencl_unroll_hint))
    SET_KNN_SET: for ( int i = 0; i < K_CONST * PAR_FACTOR ; ++i ) 
    {
      // Note that the max distance is 256
      knn_set[i] = 256;
    }

    __attribute__((xcl_pipeline_loop))
    TRAINING_LOOP : for ( int i = 0; i < NUM_TRAINING / PAR_FACTOR; ++i ) 
    {
      __attribute__((opencl_unroll_hint))
      LANES1 : for ( int j = 0; j < PAR_FACTOR; j++ ) 
      {
        // Read a new instance from the training set
        WholeDigitType training_instance = training_set[j * NUM_TRAINING / PAR_FACTOR + i];

        // Update the KNN set
        update_knn( test_instance, training_instance, &knn_set[j * K_CONST] );
      }
    }

    //START INLINED KNN_VOTE
    // local buffers

    // final K nearest neighbors
    int min_distance_list[K_CONST] __attribute__((xcl_array_partition(complete, 1)));
    // labels of the final K nearest neighbors
    int label_list[K_CONST]  __attribute__((xcl_array_partition(complete, 1)));
    // voting boxes
    int vote_list[10]  __attribute__((xcl_array_partition(complete, 1)));

    int pos = 1000;

    // initialize
    __attribute__((opencl_unroll_hint))
    INIT_1: for (int i = 0;i < K_CONST; i ++ )
    {
      min_distance_list[i] = 256;
      label_list[i] = 9;
    }

    __attribute__((opencl_unroll_hint))
    INIT_2: for (int i = 0;i < 10; i ++ )
    {
      vote_list[i] = 0;
    }

    // go through all the lanes
    // do an insertion sort to keep a sorted neighbor list
    LANES: for (int i = 0; i < PAR_FACTOR; i ++ )
    {
      INSERTION_SORT_OUTER: for (int j = 0; j < K_CONST; j ++ )
      {
        pos = 1000;
        INSERTION_SORT_INNER: for (int r = 0; r < K_CONST; r ++ )
        {
          pos = ((knn_set[i*K_CONST+j] < min_distance_list[r]) && (pos > K_CONST)) ? r : pos;
        }
        INSERT: for (int r = K_CONST ;r > 0; r -- )
        {
          if(r-1 > pos)
          {
            min_distance_list[r-1] = min_distance_list[r-2];
            label_list[r-1] = label_list[r-2];
          }
          else if (r-1 == pos)
          {
            min_distance_list[r-1] = knn_set[i*K_CONST+j];
            label_list[r-1] = i / (PAR_FACTOR / 10);
          }
        }
      }
    }

    // vote
    INCREMENT: for (int i = 0;i < K_CONST; i ++ )
      vote_list[label_list[i]] += 1;

    LabelType max_vote;
    max_vote = 0;

    // find the maximum value
    __attribute__((opencl_unroll_hint))
    VOTE: for (int i = 0;i < 10; i ++ )
    {
      if(vote_list[i] >= vote_list[max_vote])
      {
        max_vote = i;
      }
    }

//END INLINED KNN VOTE

    results[t] = max_vote;
  }

  // copy classification results back
  results_copy = async_work_group_copy(global_results, results, NUM_TEST, 0);
  wait_group_events(1, &results_copy);

}

