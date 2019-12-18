/*
 * MatrixWriter.hxx
 * 
 * Generate a CSV file based upon the matrix provided.
 */
#ifndef MATRIXWRITER_HXX
#define MATRIXWRITER_HXX

#include <fstream>
#include <iostream>
#include <stdexcept>
#include <vector>

template<typename T>
class MatrixWriter {    
    public:
        // Write the matrix provided to the path indicated.
        void static write(T matrix, std::string fileName) {
            // Open the file, check for errors
            std::ofstream out(fileName);
            if (out.fail()) {
                throw std::runtime_error("Error opening file to write.");
            }

            // Write the contents of the matrix
            for (std::size_t ndx = 0; ndx < matrix.size(); ndx++) {
                for (std::size_t ndy = 0; ndy < matrix[ndx].size(); ndy++) {
                    out << matrix[ndx][ndy] << ',';
                }
                out << std::endl;
            }
            out.close();
        }

        // Write the matrix provided to the path indicated.
        void static write(std::vector<std::vector<T>> matrix, std::string fileName) {
            std::ofstream out(fileName);
            if (out.fail()) {
                throw std::runtime_error("Error opening file to write.");
            }
            
            for (std::size_t ndx = 0; ndx < matrix.size(); ndx++) {
                for (std::size_t ndy = 0; ndy < matrix[ndx].size(); ndy++) {
                    out << matrix[ndx][ndy] << ',';
                }
                out << std::endl;
            }
            out.close();
        }
};

#endif