#ifndef ISING_H
#define ISING_H

#include <cstddef>
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <iostream>
#include <random>

class Ising
{
    private:
        int width;
        int height;
    public:
        char* data;
        double field = 0.00;
        double beta = 0.4406868; // critical beta
        double jay = 1.0;
        double mew = 1.0;

        std::mt19937 engine;
        std::uniform_int_distribution<int> coin;
        std::uniform_int_distribution<int> row_die;
        std::uniform_int_distribution<int> col_die;
        std::uniform_real_distribution<double> prob;
       
        Ising(int w, int h); 
        Ising(int w, int h, double b, double f);
        
        int GetWidth() { return width; }
        int GetHeight() { return height; }        
        
        bool HotStart();
        bool ColdStart();
        double GetEnergy();
        double GetMag();
        double DeltaEnergy(int i, int j);
        bool UpdateMetropolis();
        bool PrintData();
};

#endif
