#include "MatrixRotation.h"


void matrixRotation::rotate90degLeft (dblVector& matrix, int& width, int& height){
    int size = matrix.size();
    dblVector newMatrix(size);

    int indx;

    int i, j;

    for(int y = 0; y < height; y++){
        for(int x = 0; x < width; x++){
            i = y;//height-1-y;
            j = width-1-x;
            indx = index(i, j, height) ;
            newMatrix[indx] = matrix[index(x,y,width)];
        }
    }
    matrix.clear();
    matrix = newMatrix;

    int temp = height;
    height = width;
    width = temp;


}
void matrixRotation::rotate90degRight(dblVector& matrix, int& width, int& height){
    int size = matrix.size();
    dblVector newMatrix(size);

    int indx;

    int i, j;

    for(int y = 0; y < height; y++){
        for(int x = 0; x < width; x++){
            i = height-1-y;
            j = x;
            indx = index(i, j, height) ;
            newMatrix[indx] = matrix[index(x,y,width)];
        }
    }
    matrix.clear();
    matrix = newMatrix;

    int temp = height;
    height = width;
    width = temp;
}

void matrixRotation::flipHorizontally(dblVector& matrix, int& width, int& height){
    int size = matrix.size();
    dblVector newMatrix(size);

    int indx;

    int i, j;

    for(int y = 0; y < height; y++){
        for(int x = 0; x < width; x++){
            i = width-1-x;//
            j = y;//height-1-y;
            indx = index(i, j, width) ;
            newMatrix[indx] = matrix[index(x,y,width)];
        }
    }
    matrix.clear();
    matrix = newMatrix;
}
void matrixRotation::flipVertically  (dblVector& matrix, int& width, int& height){
    int size = matrix.size();
    dblVector newMatrix(size);

    int indx;

    int i, j;

    for(int y = 0; y < height; y++){
        for(int x = 0; x < width; x++){
            i = x;//width -1-x;//
            j = height-1-y;
            indx = index(i, j, width) ;
            newMatrix[indx] = matrix[index(x,y,width)];
        }
    }
    matrix.clear();
    matrix = newMatrix;
}



int index(int X,int Y, int& sizeX){
   return X + Y * sizeX;
}
