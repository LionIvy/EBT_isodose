#define scanOrder


#include "mooretracing.h"

#include <iostream>
#include <vector>

#define debug

MooreTracing::MooreTracing(std::vector<std::vector<double>> inpMatrix, int inpWidth, int inpHeight, double searchingLvl,bool reverseSearchingOn)
{
    Map_width = inpWidth + 4;
    Map_height = inpHeight + 4;

    initMap();
    if (reverseSearchingOn){
        setRevMap(inpMatrix, inpWidth, inpHeight, searchingLvl);
    }else{
        setNewMap(inpMatrix, inpWidth, inpHeight, searchingLvl);
    }

}

MooreTracing::~MooreTracing()
{
   destroyMap();
}

MooreTracing& MooreTracing::operator=(const MooreTracing &other)
{
    Map_width = other.Map_width;
    Map_height = other.Map_height;

    Map_mtrx = other.Map_mtrx;
    Map_mtrx_const = other.Map_mtrx_const;

    return *this;
}

void MooreTracing::clear(){

    Map_width=0;
    Map_height=0;

    Map_mtrx.clear();
    Map_mtrx_const.clear();

    prevX0=1;
    prevY0=2;

    clearTrace();

}

//=====================================================================
///==================0.Задание исходной матрицы========================
//=====================================================================

void MooreTracing::initMap()
{
    Map_mtrx.resize(Map_width);
    for (int i=0; i<Map_width; i++)
    {
        Map_mtrx[i].resize(Map_height);
        std::fill(Map_mtrx[i].begin(),Map_mtrx[i].end(),false);
    }
    Map_mtrx_const = Map_mtrx;
}

void MooreTracing::destroyMap()
{
    Map_mtrx.clear();
    Map_mtrx_const.clear();
}
/*
///Есть проблема, которая приводит к появлению спиралей
int MooreTracing::setNewMap(double** inpMatrix, int inpWidth, int inpHeight, double searchingLvl)
{
    // Проверка размерностей
    if ((inpWidth!=(Map_width)-2) || (inpHeight!=(Map_height)-2))
    {
        return 1; // ошибка, размеры матриц не соответствуют
    }


    // Присвоение элементов, соответствующих исследуемой матрице
    for (int i=0; i<inpWidth; i++)
    {
        for (int j=0; j<inpHeight; j++)
        {
            if (inpMatrix[i][j]>=searchingLvl)
            {
                Map_mtrx[i+1][j+1]=true;
            }else{
                Map_mtrx[i+1][j+1]=false;
            }
        }
    }

    Map_mtrx_const = Map_mtrx;
    return 0; //код отработал исправно
}
*/
int MooreTracing::setNewMap(std::vector<std::vector<double>> inpMatrix, int inpWidth, int inpHeight, double searchingLvl)
{
    // Проверка размерностей
    if ((inpWidth!=(Map_width)-4) || (inpHeight!=(Map_height)-4))
    {
        return 1; // ошибка, размеры матриц не соответствуют
    }


    // Присвоение элементов, соответствующих исследуемой матрице
    for (int i=0; i<inpWidth; i++)
    {
        for (int j=0; j<inpHeight; j++)
        {
            if (inpMatrix[i][j]>=searchingLvl)
            {
                Map_mtrx[i+2][j+2]=true;
            }else{
                Map_mtrx[i+2][j+2]=false;
            }
        }
    }

    Map_mtrx_const = Map_mtrx;

   // set_mtrx_borderLine();
    return 0; //код отработал исправно
}

int MooreTracing::setRevMap(std::vector<std::vector<double>> inpMatrix, int inpWidth, int inpHeight, double searchingLvl)
{
    // Проверка размерностей
    if ((inpWidth!=(Map_width)-4) || (inpHeight!=(Map_height)-4))
    {
        return 1; // ошибка, размеры матриц не соответствуют
    }

    // Присвоение элементов, соответствующих исследуемой матрице
    for (int i=0; i<inpWidth; i++)
    {
        for (int j=0; j<inpHeight; j++)
        {
            if (inpMatrix[i][j]<searchingLvl)
            {
                Map_mtrx[i+2][j+2]=true;
            }else{
                Map_mtrx[i+2][j+2]=false;
            }
        }
    }

    Map_mtrx_const = Map_mtrx;
  //  set_mtrx_borderLine();
    return 0; //код отработал исправно
}

/*
int MooreTracing::updateSearchingLvl(double searchingLvl)
{
    //destroyMap();
    //initMap();
    for (int i=0; i<Map_width; i++)
    {
        for (int j=0; j<Map_width; j++)
        {
                Map_mtrx[i][j]=false;
        }
    }
    // Присвоение элементов, соответствующих исследуемой матрице
    for (int i=0; i<Map_width-2; i++)
    {
        for (int j=0; j<Map_height-2; j++)
        {
            if (baseMtrx[i][j]>=searchingLvl)
            {
                Map_mtrx[i+1][j+1]=true;
            }else{
                Map_mtrx[i+1][j+1]=false;
            }
        }
    }

    return 0; //код отработал исправно
}
*/


//=====================================================================
///=================1.Поиск новой стартовой точки======================
//=====================================================================

/// Поиск новой точки входа в цикл поиска, возвращает значение нашел/не нашел
bool MooreTracing::locateNewStartForScanning()
{
    //printMtrx();

    int X = prevX0;
    int Y = prevY0;
    if (X >= (Map_width - 3) )  // -_______0-
    {
        X = 1;
        Y++  ;
    }

    bool pointIsNotFound = true;

    //сначала закончим сканирование по координате X, при этом Y фиксирован
    do{
        X++;
        //if(Map_mtrx_upd[X][Y])
        if(Map_mtrx[X][Y])
        {
            pointIsNotFound = false;
            prevX0 = X;
            prevY0 = Y;
        }
    }while(pointIsNotFound && (X < (Map_width - 2)));

    //
    if(pointIsNotFound)
    {
      do{ //сканирование по Y
          X = 1;
          Y++  ;
          do{ //сканирование по X
              X++;
              if(Map_mtrx[X][Y])
              {
                  pointIsNotFound = false;
                  prevX0 = X;
                  prevY0 = Y;
              }
          }while(pointIsNotFound && (X < (Map_width - 2)) );//&& (Y < (Map_height - 1))требуется правка
      }while(pointIsNotFound &&  (Y < (Map_height - 2)));//(X < (Map_width - 1)) &&
    }
    return !pointIsNotFound;
}

//=====================================================================
///========================2.Поиск контура=============================
//=====================================================================

/// Поиск нового контура, входной параметр - удалить или сохранить полость из зоны поиска,
///  возвращает значение нашел контур или нет
bool MooreTracing::traceNewObj(bool clearCavities)
{
    bool NewTraceFound;
    if (locateNewStartForScanning())  //Просканировать матрицу на наличие объектов
    {
        //Найден объект
        NewTraceFound = true;

        //Поиск контура объекта
        startTracing(Map_mtrx,prevX0,prevY0,prevX0-1,prevY0);
        Trace temp;
        temp = trace;

       // При необходимости поиска полостей, нужно доконтурить объект
       // Эта часть кода позволяет внутреннему контуру идти по границе области, принадлежащей объекту
        bool cavityBorderIsInsideOfObject = true;//false;//
        if (!clearCavities && cavityBorderIsInsideOfObject){
            if (!Map_mtrx_const[trace.X[0]][trace.Y[0]]){

                int startX = trace.X[1];
                int startY = trace.Y[1];
                int inp_prevX = trace.X[0];
                int inp_prevY = trace.Y[0];
                setNeighborhood(startX,startY,inp_prevX,inp_prevY);

                if (testNeighborhood()){ // Если точка одна в котуре - возврат
                    clearArea(temp, clearCavities);
                    return true;
                }
                trace = expandTheTrace(trace);
            }
        }

        clearArea(temp, clearCavities);     //Удаление объекта из матрицы


       /* std::cout << '\n';
        std::cout << "Cleared mtrx:\n";
        std::cout << '\n';
        printMtrx();*/
    }else{
        NewTraceFound = false;//новая точка не найдена
    }
    return NewTraceFound;
}


/// Поиск внешнего контура
void MooreTracing::startTracing(bool_matrix& SearchingMap_mtrx,int startX, int startY,int inp_prevX, int inp_prevY)
{
    clearTrace(); // при повторном считывании следует обнулить контур

    consX = startX;
    consY = startY;
    trace.X.push_back(consX);
    trace.Y.push_back(consY);

    trace.Xmin=startX;
    trace.Xmax=startX;
    trace.Ymin=startY;
    trace.Ymax=startY;

   // int traceSize = 0;
   // std::cout<<trace.X.size() << '\n';
   // std::cout<<"X,Y = " << trace.X[traceSize] << "  " << trace.Y[traceSize] << std::endl;

    setNeighborhood(startX,startY,inp_prevX, inp_prevY);

    if (testNeighborhood()) return; // Если точка одна в котуре - возврат

    traceNeighborhood(SearchingMap_mtrx);
    trace.X.push_back(consX);
    trace.Y.push_back(consY);
    updateLimits();

   // traceSize++;
   // std::cout<<trace.X.size() << '\n';
   // std::cout<<"X,Y = " << trace.X[traceSize] << "  " << trace.Y[traceSize] << std::endl;
    int attempt=1;
    int I[3]{};
    while(attempt<=2)
    {
        setNeighborhood(consX,consY,prevX,prevY);
        traceNeighborhood(SearchingMap_mtrx);
        trace.X.push_back(consX);
        trace.Y.push_back(consY);
        updateLimits();

        if(((trace.X[0] == consX) && (trace.Y[0] == consY)))
        {
            I[attempt]=trace.X.size()-1;
            attempt++;
        }
    //    traceSize++;
    //    std::cout<<trace.X.size() << '\n';
    //    std::cout<<"X,Y = " << trace.X.at(traceSize) << "  " << trace.Y.at(traceSize) << std::endl;
    }

    if((trace.X[I[0]+1] == trace.X[I[1]+1]) &&
       (trace.Y[I[0]+1] == trace.Y[I[1]+1]) &&
       (trace.X[I[0]+2] == trace.X[I[1]+2]) &&
       (trace.Y[I[0]+2] == trace.Y[I[1]+2]) )
    {
        trace.X.resize(I[1]+1);
        trace.Y.resize(I[1]+1);
    }
}

/// Задать точки соседства, начиная с предыдущей исследованной
void MooreTracing::setNeighborhood(int consX,int consY,int prevX,int prevY, bool clockwise)
{
    int N, S, keepY, W, E, keepX;

    N = consY + 1; // СЕВЕР
    S = consY - 1; // ЮГ
    keepY = consY;

    W = consX + 1; // ЗАПАД
    E = consX - 1; // ВОСТОК
    keepX = consX;


    if (clockwise){
        if (consX > prevX) // E
        {
            if(consY == prevY)       //prev  E, next NE
            {
                MooreNeibours_Y[0] = keepY;
                MooreNeibours_X[0] = E;

                MooreNeibours_Y[1] = N;
                MooreNeibours_X[1] = E;

                MooreNeibours_Y[2] = N;
                MooreNeibours_X[2] = keepX;

                MooreNeibours_Y[3] = N;
                MooreNeibours_X[3] = W;

                MooreNeibours_Y[4] = keepY;
                MooreNeibours_X[4] = W;

                MooreNeibours_Y[5] = S;
                MooreNeibours_X[5] = W;

                MooreNeibours_Y[6] = S;
                MooreNeibours_X[6] = keepX;

                MooreNeibours_Y[7] = S;
                MooreNeibours_X[7] = E;


            }else if (consY > prevY) //prev SE, next  E
            {
                MooreNeibours_Y[0] = S;
                MooreNeibours_X[0] = E;

                MooreNeibours_Y[1] = keepY;
                MooreNeibours_X[1] = E;

                MooreNeibours_Y[2] = N;
                MooreNeibours_X[2] = E;

                MooreNeibours_Y[3] = N;
                MooreNeibours_X[3] = keepX;

                MooreNeibours_Y[4] = N;
                MooreNeibours_X[4] = W;

                MooreNeibours_Y[5] = keepY;
                MooreNeibours_X[5] = W;

                MooreNeibours_Y[6] = S;
                MooreNeibours_X[6] = W;

                MooreNeibours_Y[7] = S;
                MooreNeibours_X[7] = keepX;


            }else if (consY < prevY) //prev NE, next N
            {
                MooreNeibours_Y[0] = N;
                MooreNeibours_X[0] = E;

                MooreNeibours_Y[1] = N;
                MooreNeibours_X[1] = keepX;

                MooreNeibours_Y[2] = N;
                MooreNeibours_X[2] = W;

                MooreNeibours_Y[3] = keepY;
                MooreNeibours_X[3] = W;

                MooreNeibours_Y[4] = S;
                MooreNeibours_X[4] = W;

                MooreNeibours_Y[5] = S;
                MooreNeibours_X[5] = keepX;

                MooreNeibours_Y[6] = S;
                MooreNeibours_X[6] = E;

                MooreNeibours_Y[7] = keepY;
                MooreNeibours_X[7] = E;
            }
        }else if (consX == prevX) //0
        {
            if (consY > prevY)       //prev S , next SE
            {
                MooreNeibours_Y[0] = S;
                MooreNeibours_X[0] = keepX;

                MooreNeibours_Y[1] = S;
                MooreNeibours_X[1] = E;

                MooreNeibours_Y[2] = keepY;
                MooreNeibours_X[2] = E;

                MooreNeibours_Y[3] = N;
                MooreNeibours_X[3] = E;

                MooreNeibours_Y[4] = N;
                MooreNeibours_X[4] = keepX;

                MooreNeibours_Y[5] = N;
                MooreNeibours_X[5] = W;

                MooreNeibours_Y[6] = keepY;
                MooreNeibours_X[6] = W;

                MooreNeibours_Y[7] = S;
                MooreNeibours_X[7] = W;

            }else if (consY < prevY) //prev N , next NW
            {
                MooreNeibours_Y[0] = N;
                MooreNeibours_X[0] = keepX;

                MooreNeibours_Y[1] = N;
                MooreNeibours_X[1] = W;

                MooreNeibours_Y[2] = keepY;
                MooreNeibours_X[2] = W;

                MooreNeibours_Y[3] = S;
                MooreNeibours_X[3] = W;

                MooreNeibours_Y[4] = S;
                MooreNeibours_X[4] = keepX;

                MooreNeibours_Y[5] = S;
                MooreNeibours_X[5] = E;

                MooreNeibours_Y[6] = keepY;
                MooreNeibours_X[6] = E;

                MooreNeibours_Y[7] = N;
                MooreNeibours_X[7] = E;
            }
        }else if (consX < prevX) //W
        {
            if ( consY == prevY)     //prev W , next SW
            {
                MooreNeibours_Y[0] = keepY;
                MooreNeibours_X[0] = W;

                MooreNeibours_Y[1] = S;
                MooreNeibours_X[1] = W;

                MooreNeibours_Y[2] = S;
                MooreNeibours_X[2] = keepX;

                MooreNeibours_Y[3] = S;
                MooreNeibours_X[3] = E;

                MooreNeibours_Y[4] = keepY;
                MooreNeibours_X[4] = E;

                MooreNeibours_Y[5] = N;
                MooreNeibours_X[5] = E;

                MooreNeibours_Y[6] = N;
                MooreNeibours_X[6] = keepX;

                MooreNeibours_Y[7] = N;
                MooreNeibours_X[7] = W;

            }else if (consY > prevY) //prev SW, next S
            {
                MooreNeibours_Y[0] = S;
                MooreNeibours_X[0] = W;

                MooreNeibours_Y[1] = S;
                MooreNeibours_X[1] = keepX;

                MooreNeibours_Y[2] = S;
                MooreNeibours_X[2] = E;

                MooreNeibours_Y[3] = keepY;
                MooreNeibours_X[3] = E;

                MooreNeibours_Y[4] = N;
                MooreNeibours_X[4] = E;

                MooreNeibours_Y[5] = N;
                MooreNeibours_X[5] = keepX;

                MooreNeibours_Y[6] = N;
                MooreNeibours_X[6] = W;

                MooreNeibours_Y[7] = keepY;
                MooreNeibours_X[7] = W;

            }else if (consY < prevY) //prev NW, next  W
            {
                MooreNeibours_Y[0] = N;
                MooreNeibours_X[0] = W;

                MooreNeibours_Y[1] = keepY;
                MooreNeibours_X[1] = W;

                MooreNeibours_Y[2] = S;
                MooreNeibours_X[2] = W;

                MooreNeibours_Y[3] = S;
                MooreNeibours_X[3] = keepX;

                MooreNeibours_Y[4] = S;
                MooreNeibours_X[4] = E;

                MooreNeibours_Y[5] = keepY;
                MooreNeibours_X[5] = E;

                MooreNeibours_Y[6] = N;
                MooreNeibours_X[6] = E;

                MooreNeibours_Y[7] = N;
                MooreNeibours_X[7] = keepX;
            }
        }
    }
    else{
        if (consX > prevX) // E
        {
            if(consY == prevY)       //prev  E, next NE
            {
                MooreNeibours_Y[0] = keepY;
                MooreNeibours_X[0] = E;

                MooreNeibours_Y[7] = N;
                MooreNeibours_X[7] = E;

                MooreNeibours_Y[6] = N;
                MooreNeibours_X[6] = keepX;

                MooreNeibours_Y[5] = N;
                MooreNeibours_X[5] = W;

                MooreNeibours_Y[4] = keepY;
                MooreNeibours_X[4] = W;

                MooreNeibours_Y[3] = S;
                MooreNeibours_X[3] = W;

                MooreNeibours_Y[2] = S;
                MooreNeibours_X[2] = keepX;

                MooreNeibours_Y[1] = S;
                MooreNeibours_X[1] = E;


            }else if (consY > prevY) //prev SE, next  E
            {
                MooreNeibours_Y[0] = S;
                MooreNeibours_X[0] = E;

                MooreNeibours_Y[7] = keepY;
                MooreNeibours_X[7] = E;

                MooreNeibours_Y[6] = N;
                MooreNeibours_X[6] = E;

                MooreNeibours_Y[5] = N;
                MooreNeibours_X[5] = keepX;

                MooreNeibours_Y[4] = N;
                MooreNeibours_X[4] = W;

                MooreNeibours_Y[3] = keepY;
                MooreNeibours_X[3] = W;

                MooreNeibours_Y[2] = S;
                MooreNeibours_X[2] = W;

                MooreNeibours_Y[1] = S;
                MooreNeibours_X[1] = keepX;


            }else if (consY < prevY) //prev NE, next N
            {
                MooreNeibours_Y[0] = N;
                MooreNeibours_X[0] = E;

                MooreNeibours_Y[7] = N;
                MooreNeibours_X[7] = keepX;

                MooreNeibours_Y[6] = N;
                MooreNeibours_X[6] = W;

                MooreNeibours_Y[5] = keepY;
                MooreNeibours_X[5] = W;

                MooreNeibours_Y[4] = S;
                MooreNeibours_X[4] = W;

                MooreNeibours_Y[3] = S;
                MooreNeibours_X[3] = keepX;

                MooreNeibours_Y[2] = S;
                MooreNeibours_X[2] = E;

                MooreNeibours_Y[1] = keepY;
                MooreNeibours_X[1] = E;
            }
        }else if (consX == prevX) //0
        {
            if (consY > prevY)       //prev S , next SE
            {
                MooreNeibours_Y[0] = S;
                MooreNeibours_X[0] = keepX;

                MooreNeibours_Y[7] = S;
                MooreNeibours_X[7] = E;

                MooreNeibours_Y[6] = keepY;
                MooreNeibours_X[6] = E;

                MooreNeibours_Y[5] = N;
                MooreNeibours_X[5] = E;

                MooreNeibours_Y[4] = N;
                MooreNeibours_X[4] = keepX;

                MooreNeibours_Y[3] = N;
                MooreNeibours_X[3] = W;

                MooreNeibours_Y[2] = keepY;
                MooreNeibours_X[2] = W;

                MooreNeibours_Y[1] = S;
                MooreNeibours_X[1] = W;

            }else if (consY < prevY) //prev N , next NW
            {
                MooreNeibours_Y[0] = N;
                MooreNeibours_X[0] = keepX;

                MooreNeibours_Y[7] = N;
                MooreNeibours_X[7] = W;

                MooreNeibours_Y[6] = keepY;
                MooreNeibours_X[6] = W;

                MooreNeibours_Y[5] = S;
                MooreNeibours_X[5] = W;

                MooreNeibours_Y[4] = S;
                MooreNeibours_X[4] = keepX;

                MooreNeibours_Y[3] = S;
                MooreNeibours_X[3] = E;

                MooreNeibours_Y[2] = keepY;
                MooreNeibours_X[2] = E;

                MooreNeibours_Y[1] = N;
                MooreNeibours_X[1] = E;
            }
        }else if (consX < prevX) //W
        {
            if ( consY == prevY)     //prev W , next SW
            {
                MooreNeibours_Y[0] = keepY;
                MooreNeibours_X[0] = W;

                MooreNeibours_Y[7] = S;
                MooreNeibours_X[7] = W;

                MooreNeibours_Y[6] = S;
                MooreNeibours_X[6] = keepX;

                MooreNeibours_Y[5] = S;
                MooreNeibours_X[5] = E;

                MooreNeibours_Y[4] = keepY;
                MooreNeibours_X[4] = E;

                MooreNeibours_Y[3] = N;
                MooreNeibours_X[3] = E;

                MooreNeibours_Y[2] = N;
                MooreNeibours_X[2] = keepX;

                MooreNeibours_Y[1] = N;
                MooreNeibours_X[1] = W;

            }else if (consY > prevY) //prev SW, next S
            {
                MooreNeibours_Y[0] = S;
                MooreNeibours_X[0] = W;

                MooreNeibours_Y[7] = S;
                MooreNeibours_X[7] = keepX;

                MooreNeibours_Y[6] = S;
                MooreNeibours_X[6] = E;

                MooreNeibours_Y[5] = keepY;
                MooreNeibours_X[5] = E;

                MooreNeibours_Y[4] = N;
                MooreNeibours_X[4] = E;

                MooreNeibours_Y[3] = N;
                MooreNeibours_X[3] = keepX;

                MooreNeibours_Y[2] = N;
                MooreNeibours_X[2] = W;

                MooreNeibours_Y[1] = keepY;
                MooreNeibours_X[1] = W;

            }else if (consY < prevY) //prev NW, next  W
            {
                MooreNeibours_Y[0] = N;
                MooreNeibours_X[0] = W;
                MooreNeibours_Y[1] = N;
                MooreNeibours_X[1] = keepX;
                MooreNeibours_Y[2] = N;
                MooreNeibours_X[2] = E;
                MooreNeibours_Y[3] = keepY;
                MooreNeibours_X[3] = E;
                MooreNeibours_Y[4] = S;
                MooreNeibours_X[4] = E;
                MooreNeibours_Y[5] = S;
                MooreNeibours_X[5] = keepX;
                MooreNeibours_Y[6] = S;
                MooreNeibours_X[6] = W;
                MooreNeibours_Y[7] = keepY;
                MooreNeibours_X[7] = W;

            }
        }
    }
}

/// Проверка окрестностей, р-т 1 = точка-одиночка / 0 = точка не однаж
bool MooreTracing::testNeighborhood()
{
    bool isASinglePoint = true;
    int i=-1;

    while(isASinglePoint && (i<7)) //i=-1 0 1 2 3 4 5 6
    {
        i++;                       //i= 0 1 2 3 4 5 6 7
        if(Map_mtrx[MooreNeibours_X[i]][MooreNeibours_Y[i]])
        {
            isASinglePoint = false;
        }
    }

    return isASinglePoint;
}

/// Поиск новой точки в окрестностях известной
void MooreTracing::traceNeighborhood(bool_matrix& SearchingMap_mtrx)
{
    bool isASinglePoint = true;
    int i=0;

    while(isASinglePoint && (i<7)) //i= 0 1 2 3 4 5 6
    {
        i++;                       //i= 1 2 3 4 5 6 7

#ifdef debug
        if (MooreNeibours_X[i]<0) std::cout << "traceNeighborhood MooreNeibours_X[i]<0" << std::endl;
        if (MooreNeibours_X[i]>=Map_width) std::cout << "traceNeighborhood MooreNeibours_X[i]>=Map_width" << std::endl;
        if (MooreNeibours_Y[i]<0) std::cout << "traceNeighborhood MooreNeibours_Y[i]<0" << std::endl;
        if (MooreNeibours_Y[i]>=Map_height) std::cout << "traceNeighborhood MooreNeibours_Y[i]>=Map_height" << std::endl;
#endif

        if(SearchingMap_mtrx[MooreNeibours_X[i]][MooreNeibours_Y[i]])
        {
            isASinglePoint = false;
            prevX =MooreNeibours_X[i-1];// consX;
            prevY =MooreNeibours_Y[i-1];// consY;
            consX=MooreNeibours_X[i];
            consY=MooreNeibours_Y[i];
        }
    }
}

///Расширение контура для выхода на границу полости
Trace MooreTracing::expandTheTrace(Trace trace){
    Trace expTrace;
    /// 1. Найти точку старта
    /// 2. Сохранить значение точки для условия выхода из цикла и замыкания контура
    /// 3. Задать точку формирования соседства
    /// 4. Поиск внутри соседства до столкновения с точкой пинадлежащей исходной границе

    // 1+2. Найти точку старта и сохранить значение точки для условия выхода из цикла
    int zoneIndex = 0;
    int startX = trace.X[0]-1;
    int startY = trace.Y[0];
    prevX = startX;
    prevY = startY-1;

    expTrace.Xmin = consX;
    expTrace.Xmax = consX;
    expTrace.Ymin = consY;
    expTrace.Ymax = consY;

    consX = startX;
    consY = startY;
    expTrace.X.push_back(consX);
    expTrace.Y.push_back(consY);

    // поиск диапазона значений X
    if(consX < expTrace.Xmin)
    {
         expTrace.Xmin = consX;
    }else if (consX > expTrace.Xmax)
    {
         expTrace.Xmax = consX;
    }
    // поиск диапазона значений Y
    if(consY < expTrace.Ymin)
    {
        expTrace.Ymin = consY;
    }else if (consY > expTrace.Ymax)
    {
         expTrace.Ymax = consY;
    }

    // 3. Задать точку формирования соседства

    // 4. Поиск внутри соседства до столкновения с точкой пинадлежащей исходной границе
    do{
    setNeighborhood(consX,consY,prevX, prevY);//);, false
    scanForTracePoint(trace, zoneIndex);

    consX == 0 ? consX = 1 : false;
    consY == 0 ? consY = 1 : false;
    consX >= Map_width  ? consX = Map_width  - 1 : false;
    consY >= Map_height ? consY = Map_height - 1 : false;

    expTrace.X.push_back(consX);
    expTrace.Y.push_back(consY);

    // поиск диапазона значений X
    if(consX < expTrace.Xmin)
    {
         expTrace.Xmin = consX;
    }else if (consX > expTrace.Xmax)
    {
         expTrace.Xmax = consX;
    }

    // поиск диапазона значений Y
    if(consY < expTrace.Ymin)
    {
        expTrace.Ymin = consY;
    }else if (consY > expTrace.Ymax)
    {
         expTrace.Ymax = consY;
    }

    }while(!(consX == startX && consY == startY));

    return expTrace;
}

void MooreTracing::scanForTracePoint(Trace trace,int& zoneIndex)
{
    struct Point{
        int X;
        int Y;
        int addInt;
    };
    int N = 4;
    std::vector<Point> scanList;
    for (int k=0;k<N; k++){
        scanList.push_back({trace.X[zoneIndex+k], trace.Y[zoneIndex+k],k});
    }

    Point position;
    bool isASinglePoint = true;
    bool noCollision = true;
    unsigned long i=1;
    int k;
    vMooreNeibours_X.clear();
    vMooreNeibours_Y.clear();
    for (int p = 0; p < 8 ;++p){
        if (!(MooreNeibours_X[p] < 0 || MooreNeibours_X[p] >= Map_width || MooreNeibours_Y[p] < 0 || MooreNeibours_Y[p] >= Map_height))
          {
            vMooreNeibours_X.push_back(MooreNeibours_X[p]);
            vMooreNeibours_Y.push_back(MooreNeibours_Y[p]);
            }
    }

//    while(isASinglePoint && (i<7)) //i= 8 7 6 5 4 3 2 1
//    {
//        i++;
//        //i= 7 6 5 4 3 2 1
//        position.X = MooreNeibours_X[i];
//        position.Y = MooreNeibours_Y[i];
//        noCollision = true;
//        k=N;
//        while(noCollision && k>-1){
//            if(position.X == scanList[k].X && position.Y == scanList[k].Y)
//            {
//                zoneIndex = zoneIndex+scanList[k].addInt;
//                isASinglePoint = false;
//                noCollision = false;
//                prevX =MooreNeibours_X[i-2];
//                prevY =MooreNeibours_Y[i-2];
//                consX=MooreNeibours_X[i-1];
//                consY=MooreNeibours_Y[i-1];
//            }
//            k--;
//        }
//    }
         while(isASinglePoint && (i < vMooreNeibours_X.size())) //i= 8 7 6 5 4 3 2 1
        {
            i++;
            //i= 7 6 5 4 3 2 1
            position.X = vMooreNeibours_X[i];
            position.Y = vMooreNeibours_Y[i];
            noCollision = true;
            k=N;
            while(noCollision && k>-1){
                if(position.X == scanList[k].X && position.Y == scanList[k].Y)
                {
                    zoneIndex = zoneIndex+scanList[k].addInt;
                    isASinglePoint = false;
                    noCollision = false;
                    prevX =vMooreNeibours_X[i-2];
                    prevY =vMooreNeibours_Y[i-2];
                    consX=vMooreNeibours_X[i-1];
                    consY=vMooreNeibours_Y[i-1];
                }
                k--;
            }
        }

}

//=====================================================================
///=======================3.Очиска контура=============================
//=====================================================================

/// Поиск диапазона значений координат контура
void MooreTracing::updateLimits()
{
    // поиск диапазона значений X
    if(consX < trace.Xmin)
    {
        trace.Xmin = consX;
    }else if (consX > trace.Xmax)
    {
         trace.Xmax = consX;
    }

    // поиск диапазона значений Y
    if(consY < trace.Ymin)
    {
        trace.Ymin = consY;
    }else if (consY > trace.Ymax)
    {
         trace.Ymax = consY;
    }
}

///Очистить матрицу от исследованной области
void MooreTracing::clearArea(Trace trace,bool clearCavities)
{
    bool_matrix xScannedMtrx, yScannedMtrx;

    int key=4;
    switch (key) {
    case 1:
        scanInXDirection(xScannedMtrx, trace, clearCavities);
        Map_mtrx = xScannedMtrx;
        break;
    case 2:
        scanInYDirection(yScannedMtrx, trace, clearCavities);
        Map_mtrx = yScannedMtrx;
        break;
    case 3:
        scanInXDirection(xScannedMtrx, trace, clearCavities);
        scanInYDirection(yScannedMtrx, trace, clearCavities);
        for (int I=trace.Xmin; I<=trace.Xmax;I++)
        {
            for (int J=trace.Ymin; J<=trace.Ymax;J++)
            {
                if(xScannedMtrx[I][J]==yScannedMtrx[I][J]){
                    Map_mtrx[I][J] = xScannedMtrx[I][J];
                }
                else{
                    Map_mtrx[I][J] = !Map_mtrx[I][J];
                }
            }
        }
        break;
    case 4:
        scanToClear(xScannedMtrx, trace, clearCavities);
        Map_mtrx = xScannedMtrx;
        break;
    default:
        scanInXDirection(xScannedMtrx, trace, clearCavities);
        Map_mtrx = xScannedMtrx;
        break;
    }
   // borderProtection();
}


void MooreTracing::scanToClear(bool_matrix &scannedMtrx, Trace trace, bool clearCavities)
{
    scannedMtrx = Map_mtrx;

    unsigned int len = trace.X.size();

    int Xmin = trace.Xmin;
    int Xmax = trace.Xmax;

    //на случай, если точек меньше 3 - выход из функции
    if (len<=2){
        for (unsigned int i=0; i< len; i++)
        {
            //избавление матрицы от самого контура, чтобы избавиться от одиночек и горизонтальных линий
            scannedMtrx[trace.X[i]][trace.Y[i]]=false;
        }
        return;
    }
    //если область шире:

    //задаем массив узлов области по списку координат (двумерный)
    int_matrix indexList((Xmax-Xmin+1), std::vector<int>());
    int_matrix sgnNList ((Xmax-Xmin+1), std::vector<int>());
    int_matrix sgnPList ((Xmax-Xmin+1), std::vector<int>());

    int sgnNext, sgnPrev; //знаки справа и слева от точки (позволяет отделить ветикальные линии и точки

    sgnNext = trace.X[1] - trace.X[0];
    sgnPrev = trace.X[len-2] - trace.X[0];

    if (sgnNext != sgnPrev)
    {
        indexList[trace.X[0] - Xmin].push_back(0);
        sgnNList [trace.X[0] - Xmin].push_back(sgnNext);
        sgnPList [trace.X[0] - Xmin].push_back(sgnPrev);
    }

    for (unsigned int i=1; i< len-1; i++)
    {
        sgnNext = trace.X[i+1] - trace.X[i];
        sgnPrev = trace.X[i-1] - trace.X[i];

        if (sgnNext != sgnPrev)
        {
            indexList[trace.X[i] - Xmin].push_back(i);
            sgnNList [trace.X[i] - Xmin].push_back(sgnNext);
            sgnPList [trace.X[i] - Xmin].push_back(sgnPrev);
        }
    }

    int cXval, cYval, nYval; //текущее зн-е Х и Y
    int YvalSize;     //количество узлов для заданного Х
    int temp;

    bool isOdd, notSorted;
    intVector indexListX;
    intVector  sgnNListX;
    intVector  sgnPListX;
    for (int I=0; I < (Xmax-Xmin+1);I++)
    {
        indexListX.clear();
        sgnNListX.clear();
        sgnPListX.clear();

        indexListX = indexList[I];
        sgnNListX = sgnNList[I];
        sgnPListX = sgnPList[I];

        if (indexListX.empty()|| indexListX.size()==1) continue; //на случай если узел был одинокой точкой

        updateIndexList(indexListX,sgnNListX, sgnPListX);
        if (indexListX.empty() || indexListX.size()==1) continue;

        cXval = trace.X[indexListX[0]]; //текущая кордината Х
        YvalSize=indexListX.size();             //количество узлов для заданного Х

        do{
            notSorted = false;
            for(int j = 0; j < YvalSize - 1; j++)
            {
                if(trace.Y[indexListX[j]] > trace.Y[indexListX[j + 1]])
                {
                    // меняем элементы местами
                    temp = indexListX[j];
                    indexListX[j] = indexListX[j + 1];
                    indexListX[j + 1] = temp;

                    temp = sgnNListX[j];
                    sgnNListX[j] = sgnNListX[j + 1];
                    sgnNListX[j + 1] = temp;

                    temp = sgnPListX[j];
                    sgnPListX[j] = sgnPListX[j + 1];
                    sgnPListX[j + 1] = temp;

                    notSorted=true;
                }
            }
        }while(notSorted);

        isOdd = false;
        // Удаление точек
        for(int J=0;J<YvalSize-1;J++) {

            isOdd = !isOdd;
            cYval = trace.Y[indexListX[J]];
            nYval = trace.Y[indexListX[J+1]];

//            if (cYval < nYval){
//                temp = cYval;
//                cYval = nYval;
//                nYval = temp;
//            }

            if(isOdd) // внутри ( (cYval+1) < nYval ) &&
            {
                for (int K=cYval;K < nYval; K++)
                {
                    clearCavities ?  scannedMtrx[cXval][K]=false : scannedMtrx[cXval][K]=!(scannedMtrx[cXval][K]);
                }
            }
        }
    }

    //Повторное избавление матрицы от самого контура
    for (unsigned int i=0; i< len; i++) {
        //избавление матрицы от самого контура, чтобы избавиться от одиночек и горизонтальных линий
        scannedMtrx[trace.X[i]][trace.Y[i]]=false;
    }
}

void MooreTracing::updateIndexList(intVector& indexList, intVector& sgnNList, intVector& sgnPList){

    intVector indexListUpD;
    intVector  sgnNListUpD;
    intVector  sgnPListUpD;

    int len = indexList.size();

    int i=0;
    do{
        if(i==0 && sgnPList[i]==0 && sgnNList[len-1]==0){
            if(sgnNList[i] == -sgnPList[len-1]){
                indexListUpD.push_back(indexList[i]);
                sgnNListUpD.push_back( sgnNList[i]);
                sgnPListUpD.push_back( sgnPList[i]);
            }
            i++;
            len--;
            continue;
        }
        if (sgnNList[i] == 0){
            if(sgnNList[i+1] == -sgnPList[i]){
                indexListUpD.push_back(indexList[i+1]);
                sgnNListUpD.push_back( sgnNList[i+1]);
                sgnPListUpD.push_back( sgnPList[i+1]);
            }
            i++;
        }else{
            indexListUpD.push_back(indexList[i]);
            sgnNListUpD.push_back( sgnNList[i]);
            sgnPListUpD.push_back( sgnPList[i]);
        }
        i++;
    }while(i<len);

    indexList.clear();
    sgnNList.clear();
    sgnPList.clear();

    indexList = indexListUpD;
    sgnNList = sgnNListUpD;
    sgnPList = sgnPListUpD;
}


/*
void MooreTracing::scanInXDirection(bool_matrix& scannedMtrx,Trace trace, bool clearCavities)
{
    scannedMtrx = Map_mtrx;

    unsigned int len = trace.X.size();

    int Xmin = trace.Xmin;
    int Xmax = trace.Xmax;

    //на случай, если точек меньше 3 - выход из функции
    if (len<=2){
        for (unsigned int i=0; i< len; i++)
        {
            //избавление матрицы от самого контура, чтобы избавиться от одиночек и горизонтальных линий
            scannedMtrx[trace.X[i]][trace.Y[i]]=false;
        }
        return;
    }
    //если область шире:

    //задаем массив узлов области по списку координат (двумерный)
    int_matrix IndexList((Xmax-Xmin+1), std::vector<int>());

    int sgnNext, sgnPrev; //знаки справа и слева от точки (позволяет отделить ветикальные линии и точки

    sgnNext = trace.X[1] - trace.X[0];
    sgnPrev = trace.X[len-2] - trace.X[0];



    int lastX, lastY;
    int newX, newY;
        IndexList[trace.X[0] - Xmin].push_back(0);
        lastX = trace.X[0];
        lastY = trace.Y[0];




    for (unsigned int i=1; i< len; i++)
    {
        sgnNext = trace.X[i+1] - trace.X[i];
        sgnPrev = trace.X[i-1] - trace.X[i];

        newX = trace.X[i];
        newY = trace.Y[i];

        if(newX == lastX && ((newY == lastY-1)||(newY == lastY+1)||(newY == lastY))){

        }else{
           IndexList[trace.X[i] - Xmin].push_back(i);
        }

        lastX = trace.X[i];
        lastY = trace.Y[i];

//        if ((sgnNext != sgnPrev) ||
//            ((sgnNext >  0) && (sgnPrev == 0))||
//            ((sgnNext == 0) && (sgnPrev >  0)) ) //||((sgnNext == 0) && (sgnPrev == 0)) РАЗНЫЙ ЗНАК ОЗНАЧАЕТ ТОЧКУ ПОВОРОТА. ОДИНАКОВЫЙ - ВЕРТИКАЛЬ И ТОЧКИ. ТОЧКИ УЖЕ ИСКЛЮЧИЛИ
//        {
//                IndexList[trace.X[i] - Xmin].push_back(i);
//        }
    }

    int cXval, cYval, nYval; //текущее зн-е Х и Y
    int YvalSize;     //количество узлов для заданного Х
    int buffY=0;
    bool notSorted;


    bool isOdd = true; //нечетный
    for (int I=0; I < (Xmax-Xmin+1);I++)
    {
        if (IndexList[I].empty()) continue; //на случай если узел был одинокой точкой

        cXval = trace.X[IndexList[I][0]]; //текущая кордината Х
        YvalSize=IndexList[I].size();             //количество узлов для заданного Х

        do{
            notSorted = false;
            for(int j = 0; j < YvalSize - 1; j++)
            {
                if(trace.Y[IndexList[I][j]] > trace.Y[IndexList[I][j + 1]])
                {
                    // меняем элементы местами
                    buffY = IndexList[I][j];
                    IndexList[I][j] = IndexList[I][j + 1];
                    IndexList[I][j + 1] = buffY;
                    notSorted=true;
                }else if (trace.Y[IndexList[I][j]] == trace.Y[IndexList[I][j + 1]])
                {
                    IndexList[I][j + 1] = IndexList[I][YvalSize-1];
                    IndexList[I].pop_back();
                    YvalSize--;

                    notSorted = true;
                }
            }
        }while(notSorted);
        isOdd = false;

        // Удаление точек
        for(int J=0;J<YvalSize-1;J++) {
            cYval = trace.Y[IndexList[I][J]];
            nYval = trace.Y[IndexList[I][J+1]];
            isOdd = !isOdd; //scannedMtrx[cXval][cYval+1]
#ifdef scanOrder
            if(isOdd){
#else
            if( !scannedMtrx[cXval][cYval-1]){
#endif
                for (int K=cYval;K < nYval; K++)
                {
                    clearCavities ?  scannedMtrx[cXval][K]=false : scannedMtrx[cXval][K]=!(scannedMtrx[cXval][K]);
                }
            }
        }
    }

    //Повторное избавление матрицы от самого контура
    for (unsigned int i=0; i< len; i++) {
        //избавление матрицы от самого контура, чтобы избавиться от одиночек и горизонтальных линий
        scannedMtrx[trace.X[i]][trace.Y[i]]=false;
    }
}
*/

void MooreTracing::scanInXDirection(bool_matrix& scannedMtrx,Trace trace, bool clearCavities)
{
    scannedMtrx = Map_mtrx;

    unsigned int len = trace.X.size();

    int Xmin = trace.Xmin;
    int Xmax = trace.Xmax;

    //на случай, если точек меньше 3 - выход из функции
    if (len<=2){
        for (unsigned int i=0; i< len; i++)
        {
            //избавление матрицы от самого контура, чтобы избавиться от одиночек и горизонтальных линий
            scannedMtrx[trace.X[i]][trace.Y[i]]=false;
        }
        return;
    }
    //если область шире:

    //задаем массив узлов области по списку координат (двумерный)
    int_matrix IndexList((Xmax-Xmin+1), std::vector<int>());

    int sgnNext, sgnPrev; //знаки справа и слева от точки (позволяет отделить ветикальные линии и точки

    sgnNext = trace.X[1] - trace.X[0];
    sgnPrev = trace.X[len-2] - trace.X[0];

    int lastY;

    if ((sgnNext != sgnPrev) ||//((sgnNext == 0) && (sgnPrev == 0)) РАЗНЫЙ ЗНАК ОЗНАЧАЕТ ТОЧКУ ПОВОРОТА. ОДИНАКОВЫЙ - ВЕРТИКАЛЬ И ТОЧКИ. ТОЧКИ УЖЕ ИСКЛЮЧИЛИ
         ((sgnNext >  0) && (sgnPrev == 0))||
         ((sgnNext == 0) && (sgnPrev >  0)))
    {
        IndexList[trace.X[0] - Xmin].push_back(0);
        lastY = trace.Y[0];
    }



    for (unsigned int i=1; i< len-1; i++)
    {
        sgnNext = trace.X[i+1] - trace.X[i];
        sgnPrev = trace.X[i-1] - trace.X[i];

        if ((sgnNext != sgnPrev) ||
            ((sgnNext >  0) && (sgnPrev == 0))||
            ((sgnNext == 0) && (sgnPrev >  0)) ) //||((sgnNext == 0) && (sgnPrev == 0)) РАЗНЫЙ ЗНАК ОЗНАЧАЕТ ТОЧКУ ПОВОРОТА. ОДИНАКОВЫЙ - ВЕРТИКАЛЬ И ТОЧКИ. ТОЧКИ УЖЕ ИСКЛЮЧИЛИ
        {
                IndexList[trace.X[i] - Xmin].push_back(i);
        }
    }

    int cXval, cYval, nYval; //текущее зн-е Х и Y
    int YvalSize;     //количество узлов для заданного Х
    int buffY=0;
    bool notSorted;


    //bool isOdd = true; //нечетный
    for (int I=0; I < (Xmax-Xmin+1);I++)
    {
        if (IndexList[I].empty()) continue; //на случай если узел был одинокой точкой

        cXval = trace.X[IndexList[I][0]]; //текущая кордината Х
        YvalSize=IndexList[I].size();             //количество узлов для заданного Х

        do{
            notSorted = false;
            for(int j = 0; j < YvalSize - 1; j++)
            {
                if(trace.Y[IndexList[I][j]] > trace.Y[IndexList[I][j + 1]])
                {
                    // меняем элементы местами
                    buffY = IndexList[I][j];
                    IndexList[I][j] = IndexList[I][j + 1];
                    IndexList[I][j + 1] = buffY;
                    notSorted=true;
                }else if (trace.Y[IndexList[I][j]] == trace.Y[IndexList[I][j + 1]])
                {
                    IndexList[I][j + 1] = IndexList[I][YvalSize-1];
                    IndexList[I].pop_back();
                    YvalSize--;

                    notSorted = true;
                }
            }
        }while(notSorted);
        //isOdd = false;

        // Удаление точек
        for(int J=0;J<YvalSize-1;J++) {
            cYval = trace.Y[IndexList[I][J]];
            nYval = trace.Y[IndexList[I][J+1]];
           // isOdd = !isOdd; //
#ifdef scanOrder
            if(scannedMtrx[cXval][cYval+1]){//isOdd
#else
            if( !scannedMtrx[cXval][cYval-1]){
#endif
                for (int K=cYval;K < nYval; K++)
                {
                    clearCavities ?  scannedMtrx[cXval][K]=false : scannedMtrx[cXval][K]=!(scannedMtrx[cXval][K]);
                }
            }
        }
    }

    //Повторное избавление матрицы от самого контура
    for (unsigned int i=0; i< len; i++) {
        //избавление матрицы от самого контура, чтобы избавиться от одиночек и горизонтальных линий
        scannedMtrx[trace.X[i]][trace.Y[i]]=false;
    }
}
void MooreTracing::scanInYDirection(bool_matrix& scannedMtrx, Trace trace, bool clearCavities)
{
    scannedMtrx = Map_mtrx;

    unsigned int len = trace.Y.size();

    int Ymin = trace.Ymin;
    int Ymax = trace.Ymax;

    //на случай, если точек меньше 3 - выход из функции
    if (len<=2){
        for (unsigned int i=0; i< len; i++)
        {
            //избавление матрицы от самого контура, чтобы избавиться от одиночек и горизонтальных линий
            scannedMtrx[trace.X[i]][trace.Y[i]]=false;
        }
        return;
    }
    //если область шире:

    //задаем массив узлов области по списку координат (двумерный)
    int_matrix IndexList((Ymax-Ymin+1), std::vector<int>());

    int sgnNext, sgnPrev; //знаки справа и слева от точки (позволяет отделить горизонтальные линии и точки

    sgnNext = trace.Y[1] - trace.Y[0];
    sgnPrev = trace.Y[len-2] - trace.Y[0];



    if ( (sgnNext != sgnPrev) ||  //||((sgnNext == 0) && (sgnPrev == 0)) РАЗНЫЙ ЗНАК ОЗНАЧАЕТ ТОЧКУ ПОВОРОТА. ОДИНАКОВЫЙ - ВЕРТИКАЛЬ И ТОЧКИ. ТОЧКИ УЖЕ ИСКЛЮЧИЛИ
         ((sgnNext >  0) && (sgnPrev == 0))||
         ((sgnNext == 0) && (sgnPrev >  0)) )
    {
        IndexList[trace.Y[0] - Ymin].push_back(0);
    }


    for (unsigned int i=1; i< len-1; i++)
    {
        sgnNext = trace.Y[i+1] - trace.Y[i];
        sgnPrev = trace.Y[i-1] - trace.Y[i];

        if ( (sgnNext != sgnPrev) ||
             ((sgnNext >  0) && (sgnPrev == 0))||
             ((sgnNext == 0) && (sgnPrev >  0)) ) //||((sgnNext == 0) && (sgnPrev == 0)) РАЗНЫЙ ЗНАК ОЗНАЧАЕТ ТОЧКУ ПОВОРОТА. ОДИНАКОВЫЙ - ВЕРТИКАЛЬ И ТОЧКИ. ТОЧКИ УЖЕ ИСКЛЮЧИЛИ
        {
            IndexList[trace.Y[i] - Ymin].push_back(i);
        }
    }

    int cYval, cXval, nXval; //текущее зн-е Х и Y
    int XvalSize;     //количество узлов для заданного Y
    int buffX=0;
    bool notSorted;

  //  bool isOdd = true; //нечетный
    for (int I=0; I < (Ymax-Ymin+1);I++)
    {
        if (IndexList[I].empty()) continue; //на случай если узел был одинокой точкой

        cYval = trace.Y[IndexList[I][0]]; //текущая кордината Y
        XvalSize=IndexList[I].size();       //количество узлов для заданного Y

        do{
            notSorted = false;
            for(int j = 0; j < XvalSize - 1; j++)
            {
                if(trace.X[IndexList[I][j]] > trace.X[IndexList[I][j + 1]])
                {
                    // меняем элементы местами
                    buffX = IndexList[I][j];
                    IndexList[I][j] = IndexList[I][j + 1];
                    IndexList[I][j + 1] = buffX;
                    notSorted=true;
                }else if (trace.X[IndexList[I][j]] == trace.X[IndexList[I][j + 1]])
                {
                    IndexList[I][j + 1] = IndexList[I][XvalSize-1];
                    IndexList[I].pop_back();
                    XvalSize--;

                    notSorted = true;
                }
            }
        }while(notSorted);

       // isOdd = false; //нечетный
        // Удаление точек
        for(int J=0;J<XvalSize-1;J++) {
            cXval = trace.X[IndexList[I][J]];
            nXval = trace.X[IndexList[I][J+1]];
       //     isOdd = !isOdd; //
#ifdef scanOrder
            if(scannedMtrx[cXval+1][cYval]) {//isOdd
#else
            if( !scannedMtrx[cXval-1][cYval]){
#endif
                for (int K=cXval;K <= nXval; K++)
                {
                    //scannedMtrx[K][cYval]=!(scannedMtrx[K][cYval]);
                    clearCavities ? scannedMtrx[K][cYval]=false : scannedMtrx[K][cYval]=!(scannedMtrx[K][cYval]);
                }
            }
        }
    }


    //Повторное избавление матрицы от самого контура
    for (unsigned int i=0; i< len; i++) {
        //избавление матрицы от самого контура, чтобы избавиться от одиночек и горизонтальных линий
        scannedMtrx[trace.X[i]][trace.Y[i]]=false;
    }
}

/// Очистка векторов от данных
void MooreTracing::clearTrace()
{
   // std::cout << "TraceSize = " << trace.X.size() << std::endl;

    trace.X.clear();
    trace.Y.clear();

    trace.Xmin = 0;
    trace.Xmax = 0;
    trace.Ymin = 0;
    trace.Ymax = 0;


   // std::cout << "TraceSize = " << trace.X.size() << std::endl;
}

///Задание края
void MooreTracing::set_mtrx_borderLine(){
    Map_mtrx_borderLineX.clear();
    Map_mtrx_borderLineY.clear();

    for (int x = 0; x < Map_width; ++x){
        Map_mtrx_borderLineX.push_back(x);
        Map_mtrx_borderLineY.push_back(0);
    }

    for (int x = 0; x < Map_width; ++x){
        Map_mtrx_borderLineX.push_back(x);
        Map_mtrx_borderLineY.push_back(Map_height-1);
    }

    for (int y = 1; y < Map_height; ++y){
        Map_mtrx_borderLineX.push_back(0);
        Map_mtrx_borderLineY.push_back(y);
    }

    for (int y = 1; y < Map_height; ++y){
        Map_mtrx_borderLineX.push_back(Map_width-1);
        Map_mtrx_borderLineY.push_back(y);
    }

}

/// Защита края
void MooreTracing::borderProtection(){
    for (std::vector<int>::iterator itX = Map_mtrx_borderLineX.begin(), itY = Map_mtrx_borderLineY.begin(); itX != Map_mtrx_borderLineX.end(); ++itX, ++itY){
        Map_mtrx[*itX][*itY] =false;
    }

}
//=====================================================================
///======================4.Служебные функции===========================
//=====================================================================
void MooreTracing::printMtrx()
{
    int v;
    for (int j = 0; j < Map_height; j++)
    {
        std::cout << std::endl;
        for (int i = 0; i < Map_width; i++)
        {
            if (Map_mtrx[i][j] == true)
            {
                v=1;
            }else{
                v=0;
            }
            std::cout << v << "\t";
        }
    }
}

//=====================================================================
///===========================5.Геттеры================================
//=====================================================================

std::vector<int> MooreTracing::getNewTraceX()
{
    int size =(trace.X).size();
    std::vector<int> res (size);
    for (int i=0; i<size; i++)
    {
      res[i]=  (trace.X)[i]-2;
    }
    return res;
}
std::vector<int> MooreTracing::getNewTraceY()
{
    int size =(trace.Y).size();
     std::vector<int> res (size);
     for (int i=0; i<size; i++)
     {
       res[i]=  (trace.Y)[i]-2;
     }
    return res;
}


int MooreTracing::getTraceXmin()
{
    return trace.Xmin;
}
int MooreTracing::getTraceXmax()
{
    return trace.Xmax;
}
int MooreTracing::getTraceYmin()
{
    return trace.Ymin;
}
int MooreTracing::getTraceYmax()
{
    return trace.Ymax;
}


