#include <iostream>
#include <cmath>
#include <thread>

const int SIZE = 100;
int index = 0;
int sourcePos[4] = {int(SIZE/3), int(SIZE/3), int(SIZE/2), 0};

float sourceVal(int index){
    float sourceVal = 0.1*sin(0.1 * index);
    return sourceVal;
}

int to1D(int x, int y, int z){
    return (x*SIZE+y)*SIZE + z;
}

class Array3D {
private:
    

public:
    double data[SIZE*SIZE*SIZE];

    Array3D() {
        for (int x = 0; x < SIZE; x++) {
            for (int y = 0; y < SIZE; y++) {
                for (int z = 0; z < SIZE; z++) {
                    data[to1D(x, y, z)] = 0.0;
                }
            }
        }
    }

    void operationPlus (Array3D* outputArray, char dimensions, int indexes[3]) {

        int sizeX = SIZE, sizeY = SIZE, sizeZ = SIZE;

        if (dimensions == 'x')
            sizeX -= 1;
        else if (dimensions == 'y') 
            sizeY -= 1;
        else 
            sizeZ -= 1;

        
        for (int x = 0; x < sizeX; x++) {
            for (int y = 0; y < sizeY; y++) {
                for (int z = 0 ; z < sizeZ; z++) {
                    if (dimensions == 'x'){
                        outputArray->data[to1D(x+indexes[0], y, z)] = data[to1D(x+indexes[1], y, z)] - data[to1D(x+indexes[2], y, z)];
                    }
                    else if (dimensions == 'y') {
                        outputArray->data[to1D(x, y+indexes[0], z)] = data[to1D(x, y+indexes[1], z)] - data[to1D(x, y+indexes[2], z)];
                    }
                    else {
                        outputArray->data[to1D(x, y, z+indexes[0])] = data[to1D(x, y, z+indexes[1])] - data[to1D(x, y, z+indexes[2])];
                    }
                }
            } 
        }
    }

    void operationMinus (Array3D* outputArray, char dimensions, int indexes[3]) {

        int sizeX = SIZE, sizeY = SIZE, sizeZ = SIZE;

        if (dimensions == 'x')
            sizeX -= 1;
        else if (dimensions == 'y') 
            sizeY -= 1;
        else 
            sizeZ -= 1;

        for (int x = 0; x < sizeX; x++) {
            for (int y = 0; y < sizeY; y++) {
                for (int z = 0; z < sizeZ; z++) {
                    if (dimensions == 'x'){
                        outputArray->data[to1D(x+indexes[0], y, z)] = data[to1D(x+indexes[1], y, z)] - data[to1D(x+indexes[2], y, z)];
                    }
                    else if (dimensions == 'y') {
                        outputArray->data[to1D(x, y+indexes[0], z)] = data[to1D(x, y+indexes[1], z)] - data[to1D(x, y+indexes[2], z)];
                    }
                    else {
                        outputArray->data[to1D(x, y, z+indexes[0])] = data[to1D(x, y, z+indexes[1])] - data[to1D(x, y, z+indexes[2])];
                    }
                }
            } 
        }
    }

    void printit(int xStart = 0, int xEnd = SIZE, int yStart = 0, int yEnd = SIZE, char dimension = 'a', std::string title = " ") {
        std::cout << title << std::endl;
        std::cout << dimension << std::endl;
        for (int x = xStart; x < xEnd; x++) {
            for (int y = yStart; y < yEnd; y++) {
                for (int z = 45; z < 55; z++) {
                    std::cout << data[to1D(x, y, z)] << " ";
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

    void operation (Array4D& otherArray, int dim, char dimTable[2], int indexTable[3], float courantNumber, int sourcePos[4]){

        int addDim = 0;
        int subDim = 0;
        if (dim == 0){
            addDim = 2;
            subDim = 1;
        }
        else if (dim == 1){
            addDim = 0;
            subDim = 2;
        }
        else {
            addDim = 1;
            subDim = 0;
        }

        Array3D* newArrayPlus = new Array3D();
        Array3D* newArrayMinus = new Array3D();
        std::thread plusThread( [&] {dimensions[addDim]->operationPlus(newArrayPlus, dimTable[0], indexTable); });
        std::thread minusThread( [&] {dimensions[subDim]->operationMinus(newArrayMinus, dimTable[1], indexTable); });
        plusThread.join();
        minusThread.join();
        
        // Add add and sub 3D arrays together
        for (int x = 0; x < SIZE; x++) {
            for (int y = 0; y < SIZE; y++) {
                for (int z = 0; z < SIZE; z++) {
                    newArrayPlus->data[to1D(x, y, z)] -= newArrayMinus->data[to1D(x, y, z)];
                }
            } 
        }
        
        int sign = curlType == 'H' ? 1 : -1;

        for (int x = 0; x < SIZE; x++) {
            for (int y = 0; y < SIZE; y++) {
                for (int z = 0; z < SIZE; z++) {
                    otherArray.dimensions[dim]->data[to1D(x, y, z)] += newArrayPlus->data[to1D(x, y, z)] * courantNumber * sign;
                }
            }
        }

        if (curlType == 'H' && dim == sourcePos[3]) {
            otherArray.dimensions[sourcePos[3]]->data[to1D(sourcePos[0], sourcePos[1], sourcePos[2])] += sourceVal(index);
        }

    }

    void printit(int xStart = 0, int xEnd = SIZE, int yStart = 0, int yEnd = SIZE, int zStart = 45, int zEnd = 55) {

        std::cout << std::endl;
        for (int x = xStart; x < xEnd; x++) {
            for (int y = yStart; y < yEnd; y++) {
                for (int z = zStart; z < zEnd; z++) {
                    for (int d = 0; d < 3; d++) {
                        std::cout << dimensions[d]->data[to1D(x, y, z)] << "\t";
                    }
                    std::cout << std::endl;
                }
                std::cout << std::endl;
            }
            std::cout << std::endl;
        }
    }
};


void timestep(Array4D& E, Array4D& H, float courantNumber, int sourcePos[4]){

    int indexTableE[3] = {0, 1, 0};
    int indexTableH[3] = {1, 1, 0};
    char dim0Table_thread0[2] = {'y', 'z'};
    char dim1Table_thread1[2] = {'z', 'x'};
    char dim2Table_thread2[2] = {'x', 'y'};

    std::thread curlH_thread0([&] { H.operation(E, 0, dim0Table_thread0, indexTableH, courantNumber, sourcePos); });    
    std::thread curlH_thread1([&] { H.operation(E, 1, dim1Table_thread1, indexTableH, courantNumber, sourcePos); });    
    std::thread curlH_thread2([&] { H.operation(E, 2, dim2Table_thread2, indexTableH, courantNumber, sourcePos); });    
    curlH_thread0.join();
    curlH_thread1.join();
    curlH_thread2.join();

    std::thread curlE_thread0([&] { E.operation(H, 0, dim0Table_thread0, indexTableE, courantNumber, sourcePos); });    
    std::thread curlE_thread1([&] { E.operation(H, 1, dim1Table_thread1, indexTableE, courantNumber, sourcePos); });    
    std::thread curlE_thread2([&] { E.operation(H, 2, dim2Table_thread2, indexTableE, courantNumber, sourcePos); });    
    curlE_thread0.join();
    curlE_thread1.join();
    curlE_thread2.join();

}


int main(int argc, char** argv)
{
    std::cout << "FTDT start\n";

    float courantNumber = 0.1;
    int sourcePos[4] = {int(SIZE/3), int(SIZE/3), int(SIZE/2), 0};

    std::cout << "4D create\n";

    Array4D E = Array4D('E');
    Array4D H = Array4D('H');

    std::cout << "4D created\n";

    while (index < 50) {
        std::cout << "Index: " << index << std::endl;
        timestep(E, H, courantNumber, sourcePos);
        index++;
    }

    // std::cout << "End result E\n";
    // E.printit(sourcePos[0], sourcePos[0]+1, sourcePos[1], sourcePos[1]+1);

    // std::cout << "End result H\n";
    // H.printit(sourcePos[0], sourcePos[0]+1, sourcePos[1], sourcePos[1]+1);

    std::cout << "FTDT end\n";
}