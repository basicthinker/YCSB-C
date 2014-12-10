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
      const std::string &default_value = std::string());
  void SetProperty(const std::string &key, const std::string &value);
 private:
  std::map<std::string, std::string> properties_;
};

std::string Properties::GetProperty(const std::string &key,
                                    const std::string &default_value) {
  std::map<std::string, std::string>::iterator it = properties_.find(key);
  if (properties_.end() == it) {
    return default_value;
  } else return it->second;
}

void Properties::SetProperty(const std::string &key,
                                    const std::string &value) {
  properties_[key] = value;
}

} // utils

#endif // YCSB_C_PROPERTIES_H_
