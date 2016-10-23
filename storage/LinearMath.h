#ifndef OBAMADB_LINEARMATH_H_
#define OBAMADB_LINEARMATH_H_

#include <cmath>
#include "glog/logging.h"

namespace obamadb {

  class DenseDataBlock;

  static double alpha = 0.001;

  /**
   * The dot product between 2 row vectors.
   *
   * @param row_a
   * @param row_b
   * @param row_dimension The length of the vectors.
   * @return The dot product.
   */
  double rowDot(
    double const *row_a,
    double const *row_b,
    unsigned row_dimension);

  /**
   * Euclidean distance between two vectors of equal length.
   *
   * @param p1 Vector 1
   * @param p2 Vector 2
   * @param dimension The length of both the vectors.
   * @return The distance between the two vectors.
   */
  double distance(double const *p1, double const *p2, unsigned dimension);

  /**
   * Calculate the LMS error.
   *
   * @param A The datablock containing training examples.
   * @param y The target values.
   * @param theta The current model. This will be changed during the course of the call.
   */
  double error(DenseDataBlock *const A,
               DenseDataBlock *const y,
               double const *theta);

  /**
   * Applies the gradient function of one training example on theta.
   *
   * @param training_example A single training example vector.
   * @param y The target output.
   * @param theta The model vector.
   * @param width The width of the model and training vector.
   * @param num_training_examples
   */
  void rowGradient(
    double const *training_example,
    double y,
    double *theta,
    unsigned width,
    double num_training_examples);

  /**
   * Runs gradient descent over the entire dataset for one iteration.
   *
   * @param A The datablock containing training examples.
   * @param y The target values.
   * @param theta The current model. This will be changed during the course of the call.
   */
  void gradientItr(
    DenseDataBlock const *A,
    DenseDataBlock const *y,
    double *theta);

  struct DoubleVector {
    DoubleVector(unsigned dimension)
      : dimension_(dimension) {
      values_ = new double[dimension_];
    }

    DoubleVector(const DoubleVector& other) {
      dimension_ = other.dimension_;
      values_ = new double[dimension_];
      memcpy(values_, other.values_, sizeof(double) * dimension_);
    }

    ~DoubleVector() {
      delete values_;
    }

    double& operator[](int idx) const {
      DCHECK_GT(dimension_, idx);

      return values_[idx];
    }

    unsigned dimension_;
    double *values_;

  };

}
#endif //OBAMADB_LINEARMATH_H_
