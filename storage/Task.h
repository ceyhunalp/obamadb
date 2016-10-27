#ifndef OBAMADB_TASK_H_
#define OBAMADB_TASK_H_

#include <iostream>
#include <mutex>

#include "storage/DataBlock.h"
#include "storage/DataView.h"
#include "storage/SparseDataBlock.h"
#include "storage/Utils.h"

namespace obamadb {

  double dot(const de_vector<double>& v1, double * d2);

  double dot(const se_vector<double>& v1, double * d2);

  /*
   * The Task describes what a thread will perform.
   */
  class Task {
  public:
    /*
     * Does not take ownership of the view or model.
     */
    Task(DataView *dataView, DoubleVector *doubleVector)
      : data_view_(dataView), model_(doubleVector) {}

    ~Task() {};

    virtual void execute() {};

    /**
     * Gets the fraction of misclassified examples.
     * @param theta The trained weights.
     * @param block A sample of the data.
     * @return Fraction of misclassified examples.
     */
    static double error(const DoubleVector &theta, const SparseDataBlock<double> &block);

  protected:
    DataView *data_view_;
    DoubleVector *model_;
  };

  struct SVMParams {
    SVMParams(double mu, double step_size, double step_decay, std::vector<int> degrees)
      : mu(mu),
        step_size(step_size),
        step_decay(step_decay),
        degrees(degrees) {}

    double mu;
    double step_size;
    double step_decay;
    std::vector<int> degrees;
  };

  template<class T>
  SVMParams DefaultSVMParams(std::vector<SparseDataBlock<T> *> &all_blocks);

  class SVMTask : public Task {
  public:
    SVMTask(DataView *dataView, DoubleVector *doubleVector, SVMParams &params)
      : Task(dataView, doubleVector), params_(params) { }

    /**
     * Calculates and applies the gradient of the SVM.
     */
    void execute() override;

  private:
    SVMParams params_;
  };

  template<class T>
  SVMParams DefaultSVMParams(std::vector<SparseDataBlock<T>*> &all_blocks) {
    int dim = all_blocks[0]->getNumColumns();  // the assumption here is that the last element is that classification

    std::vector<int> degrees(dim);
    for (int k = 0; k < all_blocks.size(); ++k) {
      const SparseDataBlock<T>& block = *all_blocks[k];
      if (dim < block.getNumColumns()) {
        dim = block.getNumColumns();
        degrees.resize(block.getNumColumns());
      }
      se_vector<double> row;
      for (int i = 0; i < block.getNumRows(); i++) {
        block.getRowVector(i, &row);
        for (int j = 0; j < row.numElements(); j++) {
          degrees[row.index_[j]] += 1;
        }
      }
    }

    SVMParams params(1, 0.1, 0.99, degrees);
    return params;
  }


}  // namespace obamadb

#endif //OBAMADB_TASK_H_
