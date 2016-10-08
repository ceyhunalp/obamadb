#ifndef OBAMADB_DATABLOCK_H_
#define OBAMADB_DATABLOCK_H_

#include "storage/StorageConstants.h"
#include "gflags/gflags.h"

// #include <third_party/glog/>

#include <cstdint>
#include <cstdlib>

namespace obamadb {

  class DataBlock {
  public:
    DataBlock() :
      width_(0),
      height_(0),
      elements_(0),
      max_elements_(kStorageBlockSize/sizeof(double)),
      store_(nullptr) {
      store_ = (double*) new char[kStorageBlockSize];
    }

    ~DataBlock() {
      delete store_;
    }

    void append(double *element) {
      store_[elements_++] = *element;
    }

    std::uint32_t getSize() const {
      return elements_;
    }

    double* getStore() const {
      return store_;
    }

    void setWidth(std::uint32_t width) {
      width_ = width;
    }
    

  private:

    std::uint32_t width_;   // attributes in row
    std::uint32_t height_;  // number of rows
    std::uint32_t elements_;
    std::uint32_t max_elements_;
    double *store_;
  };

}



#endif //OBAMADB_DATABLOCK_H_