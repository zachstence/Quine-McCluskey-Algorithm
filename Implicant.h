//
// Created by zachs on 2/18/2018.
//

#ifndef QUINE_MCCLUSKEY_ALGORITHM_IMPLICANT_H
#define QUINE_MCCLUSKEY_ALGORITHM_IMPLICANT_H

#include <iostream>
#include <vector>

using std::cout;
using std::endl;
using std::vector;
using std::string;

class Implicant {
 private:
  vector<int> parents_;
  string bitstring_;
  bool included_;

 public:
  Implicant();
  Implicant(vector<int>, string);
  Implicant(vector<int>, string, bool);

  void displayImplicant();
  void displayParents();

  void setParents(vector<int>);
  vector<int> getParents();
  void setBitstring(string);
  string getBitstring();
  void setIncluded(bool);
  bool isIncluded();

  bool operator<(const Implicant &) const;
  bool operator==(const Implicant &) const;
};

#endif //QUINE_MCCLUSKEY_ALGORITHM_IMPLICANT_H