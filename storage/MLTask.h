#ifndef OBAMADB_MLTASK_H
#define OBAMADB_MLTASK_H

#include <cmath>
#include <mutex>

#include "storage/DataBlock.h"
#include "storage/DataView.h"
#include "storage/SparseDataBlock.h"
#include "storage/Utils.h"

namespace obamadb {
  namespace ml {
    /**
     * The number of misclassified examples in a training block.
     * @param theta The model.
     * @param block The block.
     * @return Number misclassified.
     */
    int numMisclassified(const f_vector &theta, const SparseDataBlock<float_t> &block);

    /**
     * Gets the fraction of misclassified examples.
     * @param theta The trained weights.
     * @param block A sample of the data.
     * @return Fraction of misclassified examples.
     */
    float_t fractionMisclassified(const f_vector &theta, std::vector<SparseDataBlock<float_t>*>& block);

    /**
     * Root mean squared error.
     * @param theta The trained weights.
     * @param blocks All the data.
     */
    float_t rmsError(const f_vector &theta, std::vector<SparseDataBlock<float_t>*>& block);

    /**
     * TODO: this is really SVM loss.
     * @param theta
     * @param blocks
     * @return
     */
    float_t rmsErrorLoss(const f_vector &theta, std::vector<SparseDataBlock<float_t> *> &blocks);

    /**
     * @return The L2 distance between two vectors.
     */
    float_t L2Distance(const f_vector &v1, const f_vector &v2);

  } // end namespace ml

  class MLTask {
  public:
    /*
     * Takes ownership of the DataView.
     */
    MLTask(DataView *dataView)
      : data_view_(dataView) {}

    ~MLTask() {
      delete data_view_;
    };

    /**
     * This method will be executed by a threadpool.
     *
     * @param thread_id The ID of the thread which is executing the task.
     * @param ml_state  Some associated state relevent to the computation.
     */
    virtual void execute(int thread_id, void* ml_state) = 0;

  protected:
    DataView *data_view_;

    DISABLE_COPY_AND_ASSIGN(MLTask);
  };

  struct SVMParams {
    SVMParams(float_t mu,
              float_t step_size,
              float_t step_decay)
      : mu(mu),
        step_size(step_size),
        step_decay(step_decay),
        degrees() {}

    float_t mu;
    float_t step_size;
    float_t step_decay;
    std::vector<int> degrees;
  };

  class SVMTask : MLTask {
  public:
    SVMTask(DataView *dataView,
            f_vector *sharedTheta,
            SVMParams *sharedParams)
      : MLTask(dataView),
        shared_theta_(sharedTheta),
        shared_params_(sharedParams) { }

    /**
     * Calculates and applies the gradient of the SVM.
     */
    void execute(int thread_id, void* ml_state) override;

    f_vector* shared_theta_;
    SVMParams* shared_params_;

    DISABLE_COPY_AND_ASSIGN(SVMTask);
  };

  /**
   * Constructs the SVM to the parameters used in the HW! paper.
   * @return Caller-owned SVM params.
   */
  template<class T>
  SVMParams* DefaultSVMParams(std::vector<SparseDataBlock<T>*> &all_blocks) {
    SVMParams * params = new SVMParams(1, 0.1, 0.99);
    int dim = all_blocks[0]->getNumColumns();  // the assumption here is that the last element is that classification
    std::vector<int>& degrees = params->degrees;

    for (int k = 0; k < all_blocks.size(); ++k) {
      const SparseDataBlock<T>& block = *all_blocks[k];
      if (dim < block.getNumColumns()) {
        dim = block.getNumColumns();
        degrees.resize(block.getNumColumns());
      }
      se_vector<float_t> row;
      for (int i = 0; i < block.getNumRows(); i++) {
        block.getRowVector(i, &row);
        for (int j = 0; j < row.numElements(); j++) {
          degrees[row.index_[j]] += 1;
        }
      }
    }

    return params;
  };

}



#endif //OBAMADB_MLTASK_H