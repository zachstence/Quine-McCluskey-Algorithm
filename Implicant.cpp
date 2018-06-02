//
// Created by zachs on 2/18/2018.
//

#include "Implicant.h"

Implicant::Implicant()
    : parents_{}, bitstring_(""), included_(false) {

}
Implicant::Implicant(vector<int> parents, string binary)
    : parents_{parents}, bitstring_(binary), included_(false) {}

Implicant::Implicant(vector<int> parents, string binary, bool included)
    : parents_{parents}, bitstring_(binary), included_(included) {}

void Implicant::setParents(vector<int> parents) {
  parents_ = parents;
}

vector<int> Implicant::getParents() {
  return parents_;
}

void Implicant::setBitstring(string bitString) {
  bitstring_ = bitString;
}

string Implicant::getBitstring() {
  return bitstring_;
}

void Implicant::setIncluded(bool included) {
  included_ = included;
}

bool Implicant::isIncluded() {
  return included_;
}


bool Implicant::operator==(const Implicant &i) const {
  return bitstring_ == i.bitstring_;
}

bool Implicant::operator<(const Implicant &i) const {
//  return parents_.size() < i.parents_.size();
//  return bitstring_.compare(i.bitstring_) < 0;

  if (parents_.size() != i.parents_.size())
    return parents_.size() > i.parents_.size();
  else
    return bitstring_.compare(i.bitstring_) < 0;

}

void Implicant::displayImplicant() {
  cout << "parents:  ";
  this->displayParents();
  cout << endl;
  cout << "binary:   " << bitstring_ << endl;
  cout << "included: " << std::boolalpha << included_ << endl;
}

void Implicant::displayParents() {
  for (int i = 0; i < parents_.size(); i++) {
    cout << parents_[i] << " ";
  }
}