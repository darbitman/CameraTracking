#include "SVMTrainer.h"
#include "HOG.h"


ct::SVMTrainer::SVMTrainer() {}


bool ct::SVMTrainer::train(const std::vector<cv::Mat>& positiveSamples, const std::vector<cv::Mat>& negativeSamples) {
  uint32_t nPositiveSamples = positiveSamples.size();
  uint32_t nNegativeSamples = negativeSamples.size();
  
  // make sure we have training data
  if (nPositiveSamples == 0 || nNegativeSamples == 0) {
    return false;
  }

  // create a HOG object with default parameters
  ct::HOG hog;

    // compute first descriptor to extract number of features
  std::vector<float> desc;
  hog.RunHOG(positiveSamples[0], desc);

  uint32_t nFeatures = desc.size();

  // if there are no features then an error occured
  if (nFeatures == 0) {
    return false;
  }

  // create matrix that will hold the descriptors for all the samples
  cv::Mat samples(nPositiveSamples + nNegativeSamples, nFeatures, CV_32FC1);
  
  // copy first row that was already computed
  for (uint32_t i = 0; i < nFeatures; i++) {
    samples.ptr<float>(0)[i] = desc[i];
  }

  // compute descriptors of the remaining positive samples
  for (uint32_t i = 1; i < nPositiveSamples; i++) {
    hog.RunHOG(positiveSamples[i], desc);
    // copy descriptors into matrix
    for (uint32_t j = 0; j < nFeatures; j++) {
      samples.ptr<float>(i)[j] = desc[j];
    }
  }

  // compute descriptors of the negative samples
  for (uint32_t i = 0; i < nPositiveSamples; i++) {
    hog.RunHOG(negativeSamples[i], desc);
    // copy descriptors into matrix
    for (uint32_t j = 0; j < nFeatures; j++) {
      samples.ptr<float>(i + nPositiveSamples)[j] = desc[j];
    }
  }

  // create labels for training data
  cv::Mat labels(nPositiveSamples + nNegativeSamples, 1, CV_32SC1);
  // label positve data
  labels.rowRange(0, nPositiveSamples) = 1.0;
  // label negative data
  labels.rowRange(nPositiveSamples, nPositiveSamples + nNegativeSamples) = -1.0;

  // create SVM classifier
  cv::Ptr<cv::ml::SVM> svm = cv::ml::SVM::create();
  // set SVM type to classification type 1
  svm->setType(cv::ml::SVM::C_SVC);
  // set SVM kernel to linear
  svm->setKernel(cv::ml::SVM::LINEAR);

  // TODO prep training data to a format that SVM can read
  // TODO train SVM


  // if we reach here, SVM was successfully trained
  return true;
}