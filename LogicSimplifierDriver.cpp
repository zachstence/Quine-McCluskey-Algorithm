//
// Created by zachs on 4/5/2018.
//

#include <set>
#include <chrono>
#include "LogicSimplifier.h"

void displayImplicantVec(vector<Implicant> vec) {
//  cout << "-------- ImplicantVec --------" << endl;
  for (int i = 0; i < vec.size(); i++) {
    vec[i].displayImplicant();
    cout << endl;
  }
}

void displayTable(vector<vector<Implicant>> t) {
  cout << "display table" << endl;
  for (int i = 0; i < t.size(); i++) {
    cout << "----- " << i << " ones -----" << endl;
    displayImplicantVec(t[i]);
  }
  cout << "------------------" << endl;
}

void displayPrimeTable(vector<vector<bool>> t) {
  cout << "displayPrimeTable()" << endl;
  for (int r = 0; r < t.size(); r++) {
    for (int c = 0; c < t[r].size(); c++) {
      cout << std::noboolalpha << t[r][c] << " ";
    }
    cout << endl;
  }
  cout << endl;
}

int main() {

  vector<int> minterms_3{0, 1, 2, 7};

  vector<int> minterms_4{1, 3, 4, 5, 8, 9};

  vector<int> minterms_5{0, 3, 4, 5, 6, 10, 11, 13, 14, 15, 17, 24, 26, 28, 30, 31};

  vector<int> minterms_6
      {0, 1, 3, 6, 7, 8, 9, 11, 12, 14, 15, 17, 19, 22, 23, 24, 25, 26, 27, 28, 32, 35, 38, 39, 40, 41, 42, 43, 45, 46,
       48, 49, 53, 54, 57, 59, 62};

  vector<int> minterms_8
      {0, 1, 5, 7, 8, 13, 16, 20, 22, 26, 28, 30, 32, 33, 34, 37, 38, 40, 41, 44, 46, 48, 49, 51, 52, 53, 57, 58, 66,
       68, 69, 72, 73, 77, 79, 80, 81, 82, 83, 84, 86, 87, 91, 92, 93, 94, 95, 96, 98, 99, 100, 102, 107, 110, 117, 119,
       123, 124, 125, 127};

  LogicSimplifier ls({1, 3, 4, 5}, {2});

  auto start = std::chrono::high_resolution_clock::now();

  auto essentialPrimes = ls.simplify();

  auto finish = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> elapsed = finish - start;
  cout << "Elapsed time: " << elapsed.count() << "s\n";

  string eq = ls.getEquation();

  cout << eq << endl;

}


