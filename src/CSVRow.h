/* 
 * File:   CSVRow.h
 * Author: Merlin
 *
 * Created on August 23, 2013, 1:44 PM
 */

#ifndef CSVROW_H
#define    CSVROW_H

#include <vector>
#include <string>
#include <sstream>
#include <istream>

class CSVRow {
 public:
  CSVRow();

  CSVRow(const CSVRow &orig);

  virtual ~CSVRow();

 public:
  std::string const &operator[](std::size_t index) const {
    return data_[index];
  }

  std::size_t size() const {
    return data_.size();
  }

  void read_next_row(std::istream &str) {
    std::string line;
    std::getline(str, line);

    std::stringstream line_stream(line);
    std::string cell;

    data_.clear();
//        int i =0;
    while (std::getline(line_stream, cell, ',')) {
//            std::cout<<i++<<std::endl;
      data_.push_back(cell);
    }
  }

 private:
  std::vector<std::string> data_;
};

#endif    /* CSVROW_H */

