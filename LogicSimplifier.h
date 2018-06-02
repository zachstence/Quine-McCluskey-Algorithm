//
// Created by zachs on 4/5/2018.
//

#ifndef QUINE_MCCLUSKEY_ALGORITHM_LOGICSIMPLIFIER_H
#define QUINE_MCCLUSKEY_ALGORITHM_LOGICSIMPLIFIER_H

#include "Implicant.h"

class LogicSimplifier {
 public:
  LogicSimplifier();
  LogicSimplifier(vector<int>, vector<int>);
  LogicSimplifier(vector<int>, vector<int>, string);

  vector<vector<Implicant>> getTable();

  vector<vector<Implicant>> getOriginalTable();
  void display(vector<Implicant>);
  vector<vector<bool>> getPrimeTable();
  std::set<Implicant> getEssentialPrimes();

  void display(vector<vector<Implicant>>);
  void display(vector<vector<bool>>);
  void display(vector<int>);
  void display(std::set<int>);
  std::set<Implicant> simplify();

  void displayEssentialPrimes();

  void extractEssentials();
  void extractCover();

  string getEquation();

 private:
  vector<vector<Implicant>> table_;
  vector<Implicant> implicants_;
  vector<Implicant> primeImplicants_;

  // row index = index in primeImplicants_
  // col index = index in minterms_
  // primeTable_[r][c] = ???
  vector<vector<bool>> primeTable_;

  std::set<Implicant> essentialPrimeImplicants_;

  vector<int> minterms_;
  vector<int> dontCares_;
  int numVariables_;
  string alphabet_;
  string literals_;
  string equation_ = "F(";

  void essentialsToEquation();
  string implicantToLiterals(Implicant i);

  std::set<int> mintermsToIndices(vector<int>);
  void removeRows(std::set<int>);
  void removeColumns(std::set<int>);
  void setupPrimeTable();
  vector<Implicant> compare(vector<Implicant> &, vector<Implicant> &);
  void setup();
  void fillTable();
  int countOnes(string);
  string decimalToBitstring(int, int);
  int numVariables(vector<int>);

};

#endif //QUINE_MCCLUSKEY_ALGORITHM_LOGICSIMPLIFIER_H
