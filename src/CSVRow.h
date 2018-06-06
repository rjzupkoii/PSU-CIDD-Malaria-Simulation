/* 
 * File:   CSVRow.h
 * Author: Merlin
 *
 * Created on August 23, 2013, 1:44 PM
 */

#ifndef CSVROW_H
#define	CSVROW_H
#include <vector>
#include <string>
#include <sstream>
#include <istream>
#include <iostream>

class CSVRow {
public:
    CSVRow();
    CSVRow(const CSVRow& orig);
    virtual ~CSVRow();
public:
    std::string const& operator[](std::size_t index) const {
        return m_data[index];
    }

    std::size_t size() const {
        return m_data.size();
    }

    void readNextRow(std::istream& str) {
        std::string line;
        std::getline(str, line);

        std::stringstream lineStream(line);
        std::string cell;

        m_data.clear();
//        int i =0;
        while (std::getline(lineStream, cell, ',')) {
//            std::cout<<i++<<std::endl;
            m_data.push_back(cell);
        }
    }
private:
    std::vector<std::string> m_data;
};



#endif	/* CSVROW_H */

