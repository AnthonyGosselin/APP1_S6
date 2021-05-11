#include <iostream>
#include <cmath>
#include <thread>

float sourceVal(int index);

const int SIZE = 10;
int index = 0;

class Array3D {
private:
    

public:
    double data[SIZE][SIZE][SIZE] = {{{0.0}}};

    Array3D() {
        for (int x = 0; x < SIZE; x++) {
            for (int y = 0; y < SIZE; y++) {
                for (int z = 0; z < SIZE; z++) {
                    data[x][y][z] = 0.0;
                }
            }
        }
    }

    Array3D operation (char dimensions[2], int indexes[3]) {
        Array3D slicedArray = Array3D();

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
                        slicedArray.data[x+indexes[0]][y][z] += data[x+indexes[1]][y][z] - data[x+indexes[2]][y][z];
                    }
                    else if (dimensions[0] == 'y') {
                        slicedArray.data[x][y+indexes[0]][z] += data[x][y+indexes[1]][z] - data[x][y+indexes[2]][z];
                    }
                    else {
                        slicedArray.data[x][y][z+indexes[0]] += data[x][y][z+indexes[1]] - data[x][y][z+indexes[2]];
                    }
                }
            } 
        }

        for (int x = 0; x < sizeXneg; x++) {
            for (int y = 0; y < sizeYneg; y++) {
                for (int z = 0; z < sizeZneg; z++) {
                    if (dimensions[1] == 'x'){
                        slicedArray.data[x+indexes[0]][y][z] -= data[x+indexes[1]][y][z] - data[x+indexes[2]][y][z];
                    }
                    else if (dimensions[0] == 'y') {
                        slicedArray.data[x][y+indexes[0]][z] -= data[x][y+indexes[1]][z] - data[x][y+indexes[2]][z];
                    }
                    else {
                        slicedArray.data[x][y][z+indexes[0]] -= data[x][y][z+indexes[1]] - data[x][y][z+indexes[2]];
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
    
    char curlType;

public:

    Array3D dimensions[3] = {Array3D(), Array3D(), Array3D()};

    Array4D(char curlTypeIn) {
        curlType = curlTypeIn;
    }

    void operation (int dim, char dimTable[2], int indexTable[3], bool addition, int courantNumber, int sourcePos[4]){

        Array3D newArray = dimensions[dim].operation(dimTable, indexTable);

        int sign = addition ? 1 : -1;

        for (int x = 0; x < SIZE; x++) {
            for (int y = 0; y < SIZE; y++) {
                for (int z = 0; z < SIZE; z++) {
                    dimensions[dim].data[x][y][z] += sign * newArray.data[x][y][z] * courantNumber;
                }
            } 
        }
        
        if (curlType == 'E' && dim == 0) {
            dimensions[sourcePos[3]].data[sourcePos[0]][sourcePos[1]][sourcePos[2]] = sourceVal(index);
        }
    }

};

Array4D E = Array4D('E');
Array4D H = Array4D('H');

void abstractOperationCall(char fourD, int dim, char dimTable[2], int indexTable[3], bool addition, int courantNumber, int sourcePos[4]){
    if (fourD == 'H'){
        H.operation(dim, dimTable, indexTable, addition, courantNumber, sourcePos);
    }
    else {
        E.operation(dim, dimTable, indexTable, addition, courantNumber, sourcePos);
    }
    
}

void curlH(Array4D H, float courantNumber, int sourcePos[4]){
    int indexTable[3] = {0, 1, 0};

    char dimTable_thread0[2] = {'y', 'z'};
    std::thread curlH_thread0(abstractOperationCall, 'H', 0, dimTable_thread0, indexTable, false, courantNumber, sourcePos);

    char dimTable_thread1[2] = {'z', 'x'};
    std::thread curlH_thread1(abstractOperationCall, 'H', 1, dimTable_thread1, indexTable, false, courantNumber, sourcePos);

    char dimTable_thread2[2] = {'x', 'y'};
    std::thread curlH_thread2(abstractOperationCall, 'H', 2, dimTable_thread2, indexTable, false, courantNumber, sourcePos);

    curlH_thread0.join();
    curlH_thread1.join();
    curlH_thread2.join();
}


void curlE(Array4D E, float courantNumber, int sourcePos[4]){
    int indexTable[3] = {1, 1, 0};

    char dimTable_thread0[2] = {'y', 'z'};
    std::thread curlE_thread0(abstractOperationCall, 'E', 0, dimTable_thread0, indexTable, true, courantNumber, sourcePos);

    char dimTable_thread1[2] = {'z', 'x'};
    std::thread curlE_thread1(abstractOperationCall, 'E', 1, dimTable_thread1, indexTable, true, courantNumber, sourcePos);

    char dimTable_thread2[2] = {'x', 'y'};
    std::thread curlE_thread2(abstractOperationCall, 'E', 2, dimTable_thread2, indexTable, true, courantNumber, sourcePos);

    curlE_thread0.join();
    curlE_thread1.join();
    curlE_thread2.join();
}

void timestep(Array4D E, Array4D H, float courantNumber, int sourcePos[4]){
    curlH(H, courantNumber, sourcePos);
    curlE(E, courantNumber, sourcePos);
}


float sourceVal(int index){
    return 0.1*sin(0.1 * index);
}


int main(int argc, char** argv)
{
    std::cout << "FTDT start\n";

    float courantNumber = 0.1;
    int sourcePos[4] = {int(SIZE/3), int(SIZE/3), int(SIZE/3), 0};

    while (index < 50) {
        timestep(E, H, courantNumber, sourcePos);
        index++;
    }

    Array3D test = Array3D();
    test.printit();

}