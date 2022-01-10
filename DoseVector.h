#ifndef DoseVector_H
#define DoseVector_H


#include <vector>
#include <QString>
#include <tuple>

typedef std::vector<double> DVector;

class DoseVector
{
public:
    DoseVector();
    DoseVector(QString fileName, QString fileFormat);
   ~DoseVector();
    DoseVector& operator=(const DoseVector &other);
    void setAs(DVector doseV, int sizeX, int sizeY, int sizeZ);

    double getDoseMaxValue();
    std::tuple<int,int,int> getMaxValPosition();

    double getXSize();
    double getYSize();
    double getZSize();

    double element(int X, int Y, int Z);
    bool wrongFileFormat = false;

    double at(int indx);    

    inline DVector getDoseVector(){return doseV;}

protected:
    DVector doseV;
    int sizeX = 0, sizeY = 0, sizeZ = 0;


private:

    int index(int X,int Y,int Z);

    double doseMaxValue;
    //std::tuple<int,int,int> maxValPos;
    int maxValPosX = 0, maxValPosY = 0, maxValPosZ = 0;

    void getSizeFromSRNAFile(QString fileName);
    void getSizeFromGEANT4File(QString fileName);

    void loadMatrixFromSRNAFile(QString fileName);
    void loadMatrixFromGEANT4File(QString fileName);





};

#endif // DoseVector_H
