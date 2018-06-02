//
// Created by zachs on 4/5/2018.
//

#include <cmath>
#include <algorithm>
#include <set>
#include <iomanip>
#include "LogicSimplifier.h"

///     CONSTRUCTORS     ///////////////////////////////////////////////////////////////////////////////////////////////

LogicSimplifier::LogicSimplifier() {
  // default constructor ???
}

/**
 * Constructor with specified minterms and dont cares
 * calls setup() to initialize all PMVs
 *
 * @param minterms The minterms of the function to simplify
 * @param dontCares The "dont care's" of the function to simplify
 */
LogicSimplifier::LogicSimplifier(vector<int> minterms, vector<int> dontCares)
    : minterms_{minterms}, dontCares_{dontCares}, alphabet_{"ABCDEFGHIJKLMNOPQRSTUVWXYZ"} {
  setup();
}

/**
 * Constructor with additionally specified alphabet (uses default if not long enough)
 * calls setup() to initialize all PMVs
 *
 * @param minterms The minterms of the function to simplify
 * @param dontCares The "dont care's" of the function to simplify
 */
LogicSimplifier::LogicSimplifier(vector<int> minterms, vector<int> dontCares, string alphabet)
    : minterms_{minterms}, dontCares_{dontCares}, alphabet_{alphabet} {
  setup();
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////









///     PROCESSING FUNCTIONS     ///////////////////////////////////////////////////////////////////////////////////////
/**
* Initializes all PMVs needed for simplification
* Called automatically in constructor
*/
void LogicSimplifier::setup() {
  // Add minterms to dont cares to create vector of implicants
  dontCares_.insert(dontCares_.end(), minterms_.begin(), minterms_.end());
  // minterms_ are inserted into dontCares_ and not the other way around because minterms_ needs to not hold any
  // dont cares when it is used to find essential primes near the end of simplification

  // Initialize number of variables from full list of minterms/dontCares
  numVariables_ = numVariables(dontCares_);

  // If user specified alphabet isn't long enough, use default
  if (alphabet_.length() < numVariables_) {
    std::cerr << "User specified alphabet not long enough, using ABCDE..." << endl;
    alphabet_ = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  }

  // Initialize literals to use in equation
  literals_ = alphabet_.substr(0, numVariables_);

  // Initialize implicants vector using minterms and dontCares
  for (int m : dontCares_) {
    Implicant implicant({m}, decimalToBitstring(m, numVariables_));
    implicants_.push_back(implicant);
  }

  // Set up equation to be " F(A,B,...) = "
  equation_ += literals_[0];
  for (int i = 1; i < literals_.size(); i++) {
    equation_ += ',';
    equation_ += literals_[i];
  }
  equation_ += ") = ";

  // Fill ones table with implicants
  fillTable();
}

/**
 * Adds each implicant in its proper row in the ones table
 * Row 0: implicants with bitstring containing no 1s
 * Row 1: implicants with bitstring containing one 1
 * etc.
 */
void LogicSimplifier::fillTable() {
  // Make table appropriate size (with n variables, rows 0,1,2,...,n  :  need n+1 rows)
  table_.resize(numVariables_ + 1);
  for (auto &implicant : implicants_) {
    // Count ones in the bitstring and push_back to appropriate row in table
    int ones = countOnes(implicant.getBitstring());
    table_[ones].push_back(implicant);
  }
}

/**
 *
 * @return
 */
std::set<Implicant> LogicSimplifier::simplify() {
  // Simplify table until it has one row left
  while (table_.size() > 1) {
    // Declare newTable for next iteration
    vector<vector<Implicant>> newTable;

    // Compare each pair of rows and add their reduced combination to the new table
    for (int i = 0; i < table_.size() - 1; i++) {
      // Only compare if row isn't empty
      if (!table_[i].empty())
        newTable.push_back(compare(table_[i], table_[i + 1]));
    }

    // Loop through every implicant in the old table...
    for (int i = 0; i < table_.size(); i++) {
      for (int j = 0; j < table_[i].size(); j++) {

        // If it isn't included in a reduction in the new table...
        if (!table_[i][j].isIncluded()) {
          // If it isn't already in the primeImplicants_, add it
          if (std::find(primeImplicants_.begin(), primeImplicants_.end(), table_[i][j]) == primeImplicants_.end())
            primeImplicants_.push_back(table_[i][j]);
        }

      }
    }
    table_ = newTable;
  }

  extractEssentials();
  while (minterms_.size() > 0) {
    extractCover();
  }
  essentialsToEquation();
  return essentialPrimeImplicants_;

}














void LogicSimplifier::setupPrimeTable() {

  primeTable_.resize(primeImplicants_.size());
  for (auto &r : primeTable_) {
    r.resize(minterms_.size());
  }

  for (int r = 0; r < primeImplicants_.size(); r++) {
    vector<int> parents = primeImplicants_[r].getParents();

    for (int c = 0; c < minterms_.size(); c++) {
      int mintermNumber = minterms_[c];

      if (std::find(parents.begin(), parents.end(), mintermNumber) != parents.end()) {
        primeTable_[r][c] = true;
      }
    }
  }
}

void LogicSimplifier::extractCover() {

  vector<int> rowCounts;
  int count, max = 0, maxRow = -1;
  for (int r = primeImplicants_.size() - 1; r >= 0; r--) {
    count = 0;
    for (int c = 0; c < minterms_.size(); c++) {
      if (primeTable_[r][c]) count++;
    }
    if (count > max) {
      max = count;
      maxRow = r;
    }
    rowCounts.push_back(count);
  }

  essentialPrimeImplicants_.insert(primeImplicants_[maxRow]);

  std::set<int> rowsToRemove = {maxRow};

  vector<int> coveredMinterms = primeImplicants_[maxRow].getParents();

  std::set<int> columnsToRemove = mintermsToIndices(coveredMinterms);

  removeRows(rowsToRemove);
  removeColumns(columnsToRemove);

}

void LogicSimplifier::extractEssentials() {
  setupPrimeTable();

  std::set<int> columnsToRemove;
  vector<int> mintermsToRemove;
  std::set<int> rowsToRemove;

  int count, index = -1;
  for (int c = 0; c < primeTable_[0].size(); c++) {
    count = 0;

    for (int r = 0; r < primeTable_.size(); r++) {
      if (primeTable_[r][c]) {
        count++;
        index = r;
      }

    }

    if (count == 1 && index > -1) {

      rowsToRemove.insert(index);

      Implicant essentialPrime = primeImplicants_[index];
      essentialPrimeImplicants_.insert(essentialPrime);

      vector<int> parents = essentialPrime.getParents();

      mintermsToRemove.insert(mintermsToRemove.end(), parents.begin(), parents.end());

    }

  }

  columnsToRemove = mintermsToIndices(mintermsToRemove);
  removeColumns(columnsToRemove);
  removeRows(rowsToRemove);

}

void LogicSimplifier::essentialsToEquation() {
  auto it = essentialPrimeImplicants_.begin();
  equation_ += (implicantToLiterals(*it));
  it++;
  for (; it != essentialPrimeImplicants_.end(); it++) {
    equation_ += (" + " + implicantToLiterals(*it));
  }
}


vector<Implicant> LogicSimplifier::compare(vector<Implicant> &vec1, vector<Implicant> &vec2) {
  char delim = '-';
  vector<Implicant> newVec;

  for (int i = 0; i < vec1.size(); i++) {
    for (int j = 0; j < vec2.size(); j++) {

      string reducedString;
      int differences = 0;
      bool add = true;

      for (int c = 0; c < vec1[i].getBitstring().length(); c++) {
        char c1 = vec1[i].getBitstring()[c];
        char c2 = vec2[j].getBitstring()[c];
        if (c1 == c2) {
          reducedString += c1;
        }
        else if (c1 == delim || c2 == delim || differences >= 1) {
          add = false;
          break;
        }
        else if ((c1 == '0' && c2 == '1') || (c1 == '1' && c2 == '0')) {
          reducedString += delim;
          differences++;
        }
      }
      if (add) {
        vec1[i].setIncluded(true);
        vec2[j].setIncluded(true);

        Implicant newI;
        newI.setBitstring(reducedString);

        vector<int> i1Parents = vec1[i].getParents();
        vector<int> i2Parents = vec2[j].getParents();

        vector<int> newIParents;
        newIParents.insert(newIParents.end(), i1Parents.begin(), i1Parents.end());
        newIParents.insert(newIParents.end(), i2Parents.begin(), i2Parents.end());

        newI.setParents(newIParents);

        // Before adding a new reduced, check if its already there
        if (std::find(newVec.begin(), newVec.end(), newI) == newVec.end())
          newVec.push_back(newI);
      }
    }
  }

  return newVec;
}

void LogicSimplifier::removeRows(std::set<int> rows) {
  for (auto it = rows.rbegin(); it != rows.rend(); ++it) {
    int row = *it;
    primeTable_.erase(primeTable_.begin() + row);
    primeImplicants_.erase(primeImplicants_.begin() + row);
  }
}

void LogicSimplifier::removeColumns(std::set<int> cols) {

  for (auto it = cols.rbegin(); it != cols.rend(); ++it) {
    int colToErase = *it;
    minterms_.erase(minterms_.begin() + colToErase);

    for (int r = 0; r < primeTable_.size(); r++) {
      primeTable_[r].erase(primeTable_[r].begin() + colToErase);

    }

  }

}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////










///     UTILITY FUNCTIONS     //////////////////////////////////////////////////////////////////////////////////////////
/**
 *
 * @param num
 * @param bits
 * @return
 */
string LogicSimplifier::decimalToBitstring(int num, int bits) {
  string bitstring;
  while (num != 0) { // uses repeated division method
    bitstring.insert(0, std::to_string(num % 2));
    num /= 2;
  }
  // pad front with zeros until correct length
  while (bitstring.length() < bits)
    bitstring.insert(0, "0");
  return bitstring;
}

/**
 * Determines the number of variables needed to represent all minterms and dont cares in binary
 * @param minterms The integer vector containing minterms
 * @return The number of variables needed
 */
int LogicSimplifier::numVariables(vector<int> minterms) {
  // If no minterms, 0 variables needed
  if (minterms.empty()) return 0;
  else {
    std::sort(minterms.begin(), minterms.end());
    int max = minterms[minterms.size() - 1];
    if (max == 0) return 1;
    return (int) ceil(log(max + 1) / log(2));
  }
}

/**
 * Counts the number of ones in a given string
 * @param s The string to count ones
 * @return The number of ones in string s
 */
int LogicSimplifier::countOnes(string s) {
  int count = 0;
  for (char c : s) {
    // If character is a 1, increment count
    if (c == '1') count++;
  }
  return count;
}

string LogicSimplifier::implicantToLiterals(Implicant i) {
  string bitstring = i.getBitstring();
  string literals;
  for (int i = 0; i < bitstring.size(); i++) {
    switch (bitstring[i]) {
      case '0':literals += literals_[i];
        literals += '\'';
        break;
      case '1':literals += literals_[i];
        break;
      default:break;
    }
  }
  return literals;
}

std::set<int> LogicSimplifier::mintermsToIndices(vector<int> minterms) {
  std::set<int> indices;
  for (int x = 0; x < minterms.size(); x++) {

    for (int i = 0; i < minterms_.size(); i++) {
      if (minterms[x] == minterms_[i]) {
        indices.insert(i);
      }
    }
  }
  return indices;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////










///     SETTERS     ////////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////












///     GETTERS     ////////////////////////////////////////////////////////////////////////////////////////////////////
vector<vector<Implicant>> LogicSimplifier::getTable() {
  return table_;
}

vector<vector<bool>> LogicSimplifier::getPrimeTable() {
  return primeTable_;
}

std::set<Implicant> LogicSimplifier::getEssentialPrimes() {
  return essentialPrimeImplicants_;
}

string LogicSimplifier::getEquation() {
  return equation_;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////







///     DISPLAY FUNCTIONS     //////////////////////////////////////////////////////////////////////////////////////////
void LogicSimplifier::display(vector<Implicant> vec) {
//  cout << "-------- ImplicantVec --------" << endl;
  for (int i = 0; i < vec.size(); i++) {
    vec[i].displayImplicant();
    cout << endl;
  }
}

void LogicSimplifier::display(vector<vector<Implicant>> t) {
  cout << "display table" << endl;
  for (int i = 0; i < t.size(); i++) {
    cout << "----- " << i << " ones -----" << endl;
    display(t[i]);
  }
  cout << "------------------" << endl;
}

void LogicSimplifier::display(vector<vector<bool>> t) {
  cout << "displayPrimeTable()" << endl;
  cout << "       ";

  for (int i = 0; i < minterms_.size(); i++) {
    cout << std::setw(2) << minterms_[i] << " ";
  }
  cout << endl << "- - - - - - - - - - - - - - - - - - - - - - - - - - -" << endl;
  for (int r = 0; r < t.size(); r++) {
    cout << primeImplicants_[r].getBitstring() << ": ";
    for (int c = 0; c < t[r].size(); c++) {
      cout << std::noboolalpha << std::setw(2) << t[r][c] << " ";
    }
    cout << endl;
  }
  cout << endl;
}

void LogicSimplifier::display(vector<int> v) {
  for (int i = 0; i < v.size(); i++) {
    cout << v[i] << " ";
  }
  cout << endl;
}

void LogicSimplifier::displayEssentialPrimes() {
  for (auto it = essentialPrimeImplicants_.begin(); it != essentialPrimeImplicants_.end(); ++it) {
    Implicant i = *it;
    i.displayImplicant();
    cout << endl;
  }

}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


