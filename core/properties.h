//
//  properties.h
//  YCSB-C
//
//  Created by Jinglei Ren on 12/9/14.
//  Copyright (c) 2014 Jinglei Ren <jinglei@ren.systems>.
//

#ifndef YCSB_C_PROPERTIES_H_
#define YCSB_C_PROPERTIES_H_

#include <string>
#include <map>

namespace utils {

class Properties {
 public:
  std::string GetProperty(const std::string &key,
      const std::string &default_value = std::string()) const;
  void SetProperty(const std::string &key, const std::string &value);
 private:
  std::map<std::string, std::string> properties_;
};

inline std::string Properties::GetProperty(const std::string &key,
    const std::string &default_value) const {
  std::map<std::string, std::string>::const_iterator it = properties_.find(key);
  if (properties_.end() == it) {
    return default_value;
  } else return it->second;
}

inline void Properties::SetProperty(const std::string &key,
                                    const std::string &value) {
  properties_[key] = value;
}

} // utils

#endif // YCSB_C_PROPERTIES_H_
