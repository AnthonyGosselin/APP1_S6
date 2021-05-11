#include <iostream>

const int SIZE = 10;

class Array3D {
private:
    double data[SIZE][SIZE][SIZE] = {{{1.0}}};

public:
    Array3D() {}

    void operation () {
        Array3D slicedArray = Array3D();

        for (int x = 0; x < SIZE; x++) {
            for (int y = 0; y < SIZE; y++) {
                for (int z = 0; z < SIZE; z++) {
                    slicedArray.data[x][y][z] += data[x][y][z] - data[x][y][z];
                }
            } 
        }

    }

    void printit() {
        for (int x = 0; x < SIZE; x++) {
            for (int y = 0; y < SIZE; y++) {
                for (int z = 0; z < SIZE; z++) {
                    std::cout << data[x][y][z];
                }
                std::cout << std::endl;
            }
            std::cout << std::endl;
        }
    }
};

class Array4D {
private:
    Array3D dimensions[3] = {Array3D(), Array3D(), Array3D()};

public:
    Array4D() {}


};


int main(int argc, char** argv)
{
    std::cout << "FTDT start\n";

    Array3D test = Array3D();
    test.printit();

}