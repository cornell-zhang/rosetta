/*===============================================================*/
/*                                                               */
/*                            sgd.h                              */
/*                                                               */
/*          Top-level hardware function declaration              */
/*                                                               */
/*===============================================================*/

#include "typedefs.h"

// top-level function declaration
void SgdLR( VectorDataType    data[NUM_FEATURES * NUM_TRAINING / D_VECTOR_SIZE],
            VectorLabelType   label[NUM_TRAINING / L_VECTOR_SIZE],
            VectorFeatureType theta[NUM_FEATURES / F_VECTOR_SIZE],
            bool readLabels,
            bool writeOutput);

