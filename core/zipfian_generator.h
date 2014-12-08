//
//  zipfian_generator.h
//  YCSB-C
//
//  Created by Jinglei Ren on 12/7/14.
//  Copyright (c) 2014 Jinglei Ren <jinglei@ren.systems>.
//

#ifndef YCSB_C_ZIPFIAN_GENERATOR_H_
#define YCSB_C_ZIPFIAN_GENERATOR_H_

#include <cstdint>
#include <cmath>
#include <cassert>

namespace ycsbc {

class ZipfianGenerator : public Generator<uint64_t> {
 public:
  constexpr static const double kZipfianConst = 0.99;
  
  ZipfianGenerator(long min, long max, double zipfian_const = kZipfianConst) :
      num_items_(max - min + 1), base_(min), theta_(zipfian_const),
      zeta_n_(0), n_for_zeta_(0) {

    zeta_2_ = Zeta(2, theta_);
    alpha_ = 1.0 / (1.0 - theta_);
    RaiseZeta(num_items_);
    eta_ = Eta();
    
    Next();
  }
  
  ZipfianGenerator(size_t num_items, double zipfian_const = kZipfianConst) :
      ZipfianGenerator(0, num_items - 1, zipfian_const) { }
  
  uint64_t Next(size_t num_items);
  
  inline uint64_t Next() { return Next(num_items_); }

  inline uint64_t Last() { return last_value_; }
  
 private:
  ///
  /// Compute the zeta constant needed for the distribution.
  /// Remember the number of items, so if it is changed, we can recompute zeta.
  ///
  void RaiseZeta(size_t num) {
    assert(num >= n_for_zeta_);
    zeta_n_ = Zeta(n_for_zeta_, num, theta_, zeta_n_);
    n_for_zeta_ = num;
  }
  
  inline double Eta() {
    return (1 - std::pow(2.0 / num_items_, 1 - theta_)) /
        (1 - zeta_2_ / zeta_n_);
  }

  ///
  /// Calculate the zeta constant needed for a distribution.
  /// Do this incrementally from the last_num of items to the cur_num.
  /// Use the zipfian constant as theta. Remember the new number of items
  /// so that, if it is changed, we can recompute zeta.
  ///
  static double Zeta(size_t last_num, size_t cur_num,
                     double theta, double last_zeta) {
    double zeta = last_zeta;
    for (size_t i = last_num + 1; i <= cur_num; ++i) {
      zeta += 1 / std::pow(i, theta);
    }
    return zeta;
  }
  
  static inline double Zeta(size_t num, double theta) {
    return Zeta(0, num, theta, 0);
  }
  
  size_t num_items_;
  size_t base_; /// Min number of items to generate
  
  // Computed parameters for generating the distribution
  double theta_, zeta_n_, eta_, alpha_, zeta_2_;
  size_t n_for_zeta_; /// Number of items used to compute zeta_n
  uint64_t last_value_;
};

uint64_t ZipfianGenerator::Next(size_t num) {
  if (num > n_for_zeta_) { // Recompute zeta_n and eta
    RaiseZeta(num);
    eta_ = Eta();
  }
  
  double u = utils::RandomDouble();
  double uz = u * zeta_n_;
  
  if (uz < 1.0) {
    return 0;
  }
  
  if (uz < 1.0 + std::pow(0.5, theta_)) {
    return 1;
  }
  
  return last_value_ = base_ + num * std::pow(eta_ * u - eta_ + 1, alpha_);
}

}

#endif // YCSB_C_ZIPFIAN_GENERATOR_H_
