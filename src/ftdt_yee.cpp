#include <iostream>
#include <cmath>
#include <thread>

const int SIZE = 100;
int index = 0;

float sourceVal(int index){
    return 0.1*sin(0.1 * index);
}

int to1D(int x, int y, int z){
    return (x*SIZE+y)*SIZE + z;
}

class Array3D {
private:
    

public:
    double data[SIZE*SIZE*SIZE] = {0.0};

    Array3D() {
        for (int x = 0; x < SIZE; x++) {
            for (int y = 0; y < SIZE; y++) {
                for (int z = 0; z < SIZE; z++) {
                    data[to1D(x, y, z)] = 0.0;
                }
            }
        }
    }

    Array3D* operation (char dimensions[2], int indexes[3]) {
        Array3D* slicedArray = new Array3D();

        int sizeXpos, sizeYpos, sizeZpos, sizeXneg, sizeYneg, sizeZneg = SIZE;

        if (dimensions[0] == 'x')
            sizeXpos -= 1;
        else if (dimensions[0] == 'y') 
            sizeYpos -= 1;
        else 
            sizeZpos -= 1;

        if (dimensions[1] == 'x')
            sizeXneg -= 1;
        else if (dimensions[1] == 'y') 
            sizeYneg -= 1;
        else 
            sizeZneg -= 1;

        
        for (int x = 0; x < sizeXpos; x++) {
            for (int y = 0; y < sizeYpos; y++) {
                for (int z = 0 ; z < sizeZpos; z++) {
                    if (dimensions[0] == 'x'){
                        slicedArray->data[to1D(x+indexes[0], y, z)] += data[to1D(x+indexes[1], y, z)] - data[to1D(x+indexes[2], y, z)];
                    }
                    else if (dimensions[0] == 'y') {
                        slicedArray->data[to1D(x, y+indexes[0], z)] += data[to1D(x, y+indexes[1], z)] - data[to1D(x, y+indexes[2], z)];
                    }
                    else {
                        slicedArray->data[to1D(x, y, z+indexes[0])] += data[to1D(x, y, z+indexes[1])] - data[to1D(x, y, z+indexes[2])];
                    }
                }
            } 
        }

        for (int x = 0; x < sizeXneg; x++) {
            for (int y = 0; y < sizeYneg; y++) {
                for (int z = 0; z < sizeZneg; z++) {
                    if (dimensions[1] == 'x'){
                        slicedArray->data[to1D(x+indexes[0], y, z)] -= data[to1D(x+indexes[1], y, z)] - data[to1D(x+indexes[2], y, z)];
                    }
                    else if (dimensions[0] == 'y') {
                        slicedArray->data[to1D(x, y+indexes[0], z)] -= data[to1D(x, y+indexes[1], z)] - data[to1D(x, y+indexes[2], z)];
                    }
                    else {
                        slicedArray->data[to1D(x, y, z+indexes[0])] -= data[to1D(x, y, z+indexes[1])] - data[to1D(x, y, z+indexes[2])];
                    }
                }
            } 
        }

        return slicedArray;
    }

    void printit() {
        for (int x = 0; x < SIZE; x++) {
            for (int y = 0; y < SIZE; y++) {
                for (int z = 0; z < SIZE; z++) {
                    std::cout << data[to1D(x, y, z)];
                }
                std::cout << std::endl;
            }
            std::cout << std::endl;
        }
    }
};

class Array4D {
private:
    
    char curlType;

public:

    //Array3D dimensions[3] = {Array3D(), Array3D(), Array3D()};

    Array3D * dimension1 = new Array3D();
    Array3D * dimension2 = new Array3D();
    Array3D * dimension3 = new Array3D();

    Array3D* dimensions[3] = {dimension1, dimension2, dimension3};

    Array4D(char curlTypeIn) {
        curlType = curlTypeIn;
    }

    ~Array4D() {
        delete dimension1;
        delete dimension2;
        delete dimension3;
    }

    void operation (int dim, char dimTable[2], int indexTable[3], bool addition, int courantNumber, int sourcePos[4]){

        // std::cout << "4D operation started\n";

        Array3D* newArray = dimensions[dim]->operation(dimTable, indexTable);

        int sign = addition ? 1 : -1;

        for (int x = 0; x < SIZE; x++) {
            for (int y = 0; y < SIZE; y++) {
                for (int z = 0; z < SIZE; z++) {
                    dimensions[dim]->data[to1D(x, y, z)] += sign * newArray->data[to1D(x, y, z)] * courantNumber;
                }
            } 
        }
        
        if (curlType == 'E' && dim == 0) {
            dimensions[sourcePos[3]]->data[to1D(sourcePos[0], sourcePos[1], sourcePos[2])] = sourceVal(index);
        }
    }

};

void curlH(Array4D& H, float courantNumber, int sourcePos[4]){

    // std::cout << "CurlH started\n";

    int indexTable[3] = {0, 1, 0};

    char dimTable_thread0[2] = {'y', 'z'};
    std::thread curlH_thread0(&Array4D::operation, &H, 0, dimTable_thread0, indexTable, false, courantNumber, sourcePos);

    // std::cout << "CurlH thread 0 started\n";

    char dimTable_thread1[2] = {'z', 'x'};
    std::thread curlH_thread1(&Array4D::operation, &H, 1, dimTable_thread1, indexTable, false, courantNumber, sourcePos);

    // std::cout << "CurlH thread 1 started\n";

    char dimTable_thread2[2] = {'x', 'y'};
    std::thread curlH_thread2(&Array4D::operation, &H, 2, dimTable_thread2, indexTable, false, courantNumber, sourcePos);

    // std::cout << "CurlH thread 2 started\n";

    curlH_thread0.join();
    curlH_thread1.join();
    curlH_thread2.join();

    // std::cout << "CurlH threads joined \n";
}

void curlE(Array4D& E, float courantNumber, int sourcePos[4]){
    int indexTable[3] = {1, 1, 0};

    char dimTable_thread0[2] = {'y', 'z'};
    std::thread curlE_thread0(&Array4D::operation, &E, 0, dimTable_thread0, indexTable, true, courantNumber, sourcePos);

    char dimTable_thread1[2] = {'z', 'x'};
    std::thread curlE_thread1(&Array4D::operation, &E, 1, dimTable_thread1, indexTable, true, courantNumber, sourcePos);

    char dimTable_thread2[2] = {'x', 'y'};
    std::thread curlE_thread2(&Array4D::operation, &E, 2, dimTable_thread2, indexTable, true, courantNumber, sourcePos);

    curlE_thread0.join();
    curlE_thread1.join();
    curlE_thread2.join();

    // std::cout << "CurlE threads joined \n";
}

void timestep(Array4D& E, Array4D& H, float courantNumber, int sourcePos[4]){
    curlH(H, courantNumber, sourcePos);
    curlE(E, courantNumber, sourcePos);
}


int main(int argc, char** argv)
{
    std::cout << "FTDT start\n";

    float courantNumber = 0.1;
    int sourcePos[4] = {int(SIZE/3), int(SIZE/3), int(SIZE/3), 0};

    std::cout << "4D create\n";

    Array4D E = Array4D('E');
    Array4D H = Array4D('H');

    std::cout << "4D created\n";

    while (index < 50) {
        std::cout << "Index: " << index << std::endl;
        timestep(E, H, courantNumber, sourcePos);
        index++;
    }

    std::cout << "FTDT end\n";

}