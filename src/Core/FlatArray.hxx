/*
 * FlatArray.hxx
 * 
 * This pseudo-library provides templates for a 2D and 3D data structure that has been flattened
 * into a 1D array. This is generally going to be more performant than dimensional data structures 
 * but may not be as flexible.
 */ 
#ifndef FLATARRAY_H
#define FLATARRAY_H

#include <vector>

template <typename T>
class Flat2D {
    private:
        std::vector<T> data;
        std::size_t x_size = 0;         // Width
        std::size_t y_size = 0;         // Height

    public:
        Flat2D() = default;
        Flat2D(int x_size, int y_size)  {
            this->x_size = x_size;
            this->y_size = y_size;

            data.reserve(x_size * y_size);
        }

        T get(int x, int y) { return data[x + y * y_size]; }
        void set(int x, int y, T value) { data[x + y * y_size] = value; }

        static Flat2D<T>* flatten(std::vector<std::vector<T>> input)  {
            // Create the object
            auto result = new Flat2D<T>();

            // Note the dimensions
            result->x_size = input.size();
            result->y_size = input[0].size();
            for (std::size_t ndx = 1; ndx < result.x_size; ndx++) {
                result->y_size = (input[ndx].size() > result->y_size) ? input[ndx].size() : result->y_size;
            }

            // Allocate the memory
            result->data(result->x_size * result->y_size);

            // Copy the data
            for (std::size_t ndx = 0; ndx < result->x_size; ndx++) {
                for (std::size_t ndy = 0; ndy < result->y_size; ndy++) {
                    result->set(ndx, ndy, input[ndx][ndy]);
                }
            }

            // Return the object
            return result;
        }
};

template <typename T>
class Flat3D {
    private:
        std::vector<T> data;
        std::size_t x_size = 0;         // Width
        std::size_t y_size = 0;         // Height
        std::size_t z_size = 0;         // Depth

    public:
        Flat3D() = default;

        // Allocate the space for an x by y by z sized space. Note that if max(x) > max(y)
        // unexpected behavior may occur.
        Flat3D(int x_size, int y_size, int z_size)  {
            this->x_size = x_size;
            this->y_size = y_size;
            this->z_size = z_size;

            data.reserve(x_size * y_size * z_size);
        }

        T get(int x, int y, int z) { return data[x + y_size * (y + x_size * z)]; }
        void set(int x, int y, int z, T value) { data[x + y_size * (y + x_size * z)] = value; }

        static Flat3D<T>* flatten(std::vector<std::vector<std::vector<T>>> input)  {
            // Create the object
            auto result = new Flat3D<T>();

            // Note the dimensions
            result->x_size = input.size();
            result->y_size = 0;
            result->z_size = 0;
            for (std::size_t ndx = 0; ndx < result->x_size; ndx++) {
                if (input[ndx].size() == 0) { continue; }
                result->y_size = (input[ndx].size() > result->y_size) ? input[ndx].size() : result->y_size;
                for (std::size_t ndy = 0; ndy < input[ndx].size(); ndy++) {
                    result->z_size = (input[ndx][ndy].size() > result->z_size) ? input[ndx][ndy].size() : result->z_size;
                }
            }

            // Allocate the memory
            result->data.reserve(result->x_size * result->y_size * result->z_size);

            // Copy the data
            for (std::size_t ndx = 0; ndx < result->x_size; ndx++) {
                for (std::size_t ndy = 0; ndy < result->y_size; ndy++) {
                    for (std::size_t ndz = 0; ndz < result->z_size; ndz++) {
                        result->set(ndx, ndy, ndz, input[ndx][ndy].size() > 0 ? input[ndx][ndy][ndz] : 0);
                    }
                }
            }

            // Return the object
            return result;
        }
};

#endif