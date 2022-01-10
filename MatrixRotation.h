#ifndef MATRIXROTATION_H
#define MATRIXROTATION_H


#include "DoseVector.h"

#include "QString"

typedef std::vector<int> intVector;
typedef std::vector<double> dblVector;
typedef std::vector<std::pair<double,double>> vectorPair;

namespace matrixRotation{

void rotate90degLeft (dblVector& matrix, int& width, int& height);
void rotate90degRight(dblVector& matrix, int& width, int& height);

void flipHorizontally(dblVector& matrix, int& width, int& height);
void flipVertically  (dblVector& matrix, int& width, int& height);
}

int index(int X,int Y, int& sizeX);

#endif // MATRIXROTATION_H
