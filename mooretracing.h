#ifndef MOORETRACING_H
#define MOORETRACING_H

#include <vector>
#include <utility>

typedef std::vector<int> intVector;
typedef std::vector<std::vector<int   >> int_matrix;
typedef std::vector<std::vector<double>> double_matrix;
typedef std::vector<std::vector<bool  >> bool_matrix;
//typedef std::vector<std::vector<bool>>

struct Trace{
    intVector X;
    intVector Y;
    int Xmin = 0;
    int Xmax = 0;
    int Ymin = 0;
    int Ymax = 0;
};


class MooreTracing
{
public:
    MooreTracing(){};
//    MooreTracing(double** inpMatrix, int inpWidth, int inpHeight, double searchingLvl); //иниц + присвоение
    MooreTracing(double_matrix inpMatrix, int inpWidth, int inpHeight, double searchingLvl,bool reverseSearchingOn = false);


    ~MooreTracing();

    MooreTracing& operator=(const MooreTracing& other);

    void clear();

    // Задание рабочей матрицы
    int setNewMap(double_matrix inpMatrix, int inpWidth, int inpHeight, double searchingLvl);
    int setRevMap(double_matrix inpMatrix, int inpWidth, int inpHeight, double searchingLvl);
    //    int setNewMap(double** inpMatrix, int inpWidth, int inpHeight, double searchingLvl);

    //    int updateSearchingLvl(double searchingLvl);

    // Поиск новой границы
    bool traceNewObj(bool clearCavities);

    int getTraceXmin();
    int getTraceXmax();
    int getTraceYmin();
    int getTraceYmax();

    // Вывод координат границ
    intVector getNewTraceX();
    intVector getNewTraceY();

    void printMtrx();


private:

    //=====================================================================
    ///==================0.Задание исходной матрицы========================
    //=====================================================================

    bool_matrix Map_mtrx;
    bool_matrix Map_mtrx_const;
    int Map_width, Map_height;

    void initMap();    // инициализация массива рабочей матрицы
    void destroyMap(); // уничтожение массива рабочей матрицы

    //=====================================================================
    ///==============================Алгоритм==============================
    ///
    /// 1. Найти точку старта поиска S, отметив предыдущую точку проверки P.
    ///    Присвоить текущей точки поиска C, значение точки S
    /// 2. Начать поиск контура:
    /// 2.1. Для точки поиска задать соседей Mc
    /// 2.2. Обойти соседей пр.часовой стрелки, начиная с точки P, пока
    ///      не найдется след.элемент, сохранить его как C. Сохранить
    ///      предыдущую проверенную точку, не явл. частью контура, как P
    /// 2.3. Повторять процедуру поиска для новой точки C, до возвращения в точку S
    ///      дважды, исключить из истории повторное прохождение
    /// 3. Исключить из исходной матрицы область внутри контура, сохранив или удалив полости
    //=====================================================================

    //=====================================================================
    ///=================1.Поиск новой стартовой точки======================
    //=====================================================================
    // Координаты точки с которых начинался предыдущий поиск
    int prevX0=0, prevY0=1;

    // Поиск новой стартовой точки
    bool locateNewStartForScanning();

    //=====================================================================
    ///========================2.Поиск контура=============================
    //=====================================================================

    //Внешний контур
    Trace trace;

    // Поиск контура
    void startTracing(bool_matrix& SearchingMap_mtrx,int startX, int startY,int inp_prevX, int inp_prevY);

    // Задание координат точек окрестности
    int MooreNeibours_X[8]{}, MooreNeibours_Y[8]{};
    void setNeighborhood(int consX,int consY,int prevX,int prevY, bool clockwise = true);

    //Проверить есть в окрестности хоть что-то (на случай локальной точки)
    bool testNeighborhood();

    //Проверка окрестности и поиск след точки
    void traceNeighborhood(bool_matrix& SearchingMap_mtrx);
    int consX, consY; // координаты точки, которая рассматривается
    int prevX, prevY; // подтвержденные координаты

    Trace expandTheTrace(Trace trace);
    void scanForTracePoint(Trace trace,int& zoneIndex);


    //=====================================================================
    ///=======================3.Очиска контура=============================
    //=====================================================================

    // Крайние значения координат контура области
    //trace.Xmin = 0;
    //trace.Xmax = 0;
    //trace.Ymin = 0;
    //trace.Ymax = 0;
    void updateLimits();

    void clearArea(Trace trace, bool clearCavities);  // Очистка области

   void  scanToClear(bool_matrix &scannedMtrx, Trace trace, bool clearCavities);
    void updateIndexList(intVector& indexList, intVector& sgnNList, intVector& sgnPList);

    void scanInXDirection(bool_matrix& scannedMtrx, Trace trace, bool clearCavities);
    void scanInYDirection(bool_matrix& scannedMtrx, Trace trace, bool clearCavities);

    void clearTrace(); // Очистка вектора контура




    //=====================================================================
    ///======================4.Служебные функции===========================
    //=====================================================================







};

#endif // MOORETRACING_H
