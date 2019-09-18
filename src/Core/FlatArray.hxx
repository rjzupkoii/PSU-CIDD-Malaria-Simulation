/*
 *
 * 
 */ 
#ifndef FLATARRAY_H
#define FLATARRAY_H

#include <vector>

template <typename T>
class Flat2D {
    private:
        T* data = nullptr;
        int x_size = 0;
        int y_size = 0;

    public:
        Flat2D() = default;
        Flat2D(int x_size, int y_size)  {
            this->x_size = x_size;
            this->y_size = y_size;

            data = new T[x_size * y_size];
        }

        T get(int x, int y) { return data[x + y * y_size]; }
        void set(int x, int y, T value) { data[x + y * y_size] = value; }

        static Flat2D<T>* flatten(std::vector<std::vector<T>> input)  {
            // Create the object
            auto result = new Flat2D<T>();

            // Note the dimensions
            result->x_size = input.size();
            result->y_size = input[0].size();
            for (auto ndx = 1; ndx < result.x_size; ndx++) {
                result->y_size = (input[ndx].size() > result->y_size) ? input[ndx].size() : result->y_size;
            }

            // Allocate the memory
            result = new T[result->x_size * result->y_size];

            // Copy the data
            for (auto ndx = 0; ndx < result->x_size; ndx++) {
                for (auto ndy = 0; ndy < result->y_size; ndy++) {
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
        T* data = nullptr;
        int x_size = 0;
        int y_size = 0;
        int z_size = 0;

    public:
        Flat3D() = default;
        Flat3D(int x_size, int y_size, int z_size)  {
            this->x_size = x_size;
            this->y_size = y_size;
            this->z_size = z_size;

            data = new T[x_size * y_size * z_size];
        }

        T get(int x, int y, int z) { return data[x + y_size * (y + z_size * z)]; }
        void set(int x, int y, int z, T value) { data[x + y_size * (y + z_size * z)] = value; }

        static Flat3D<T>* flatten(std::vector<std::vector<std::vector<T>>> input)  {
            // Create the object
            auto result = new Flat3D<T>();

            // Note the dimensions
            result->x_size = input.size();
            result->y_size = 0;
            result->z_size = 0;
            for (auto ndx = 0; ndx < result->x_size; ndx++) {
                if (input[ndx].size() == 0) { continue; }
                result->y_size = (input[ndx].size() > result->y_size) ? input[ndx].size() : result->y_size;
                for (auto ndy = 0; ndy < input[ndx].size(); ndy++) {
                    result->z_size = (input[ndx][ndy].size() > result->z_size) ? input[ndx][ndy].size() : result->z_size;
                }
            }

            // Allocate the memory
            result->data = new T[result->x_size * result->y_size * result->z_size];

            // Copy the data
            for (auto ndx = 0; ndx < result->x_size; ndx++) {
                for (auto ndy = 0; ndy < result->y_size; ndy++) {
                    for (auto ndz = 0; ndz < result->z_size; ndz++) {
                        result->set(ndx, ndy, ndz, input[ndx][ndy].size() > 0 ? input[ndx][ndy][ndz] : 0);
                    }
                }
            }

            // Return the object
            return result;
        }
};

#endif