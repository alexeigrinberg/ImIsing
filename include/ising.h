#ifndef ISING_H
#define ISING_H

#include <cstddef>
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <iostream>
#include <random>
#include <stack>

class Ising
{
    private:
        int width;
        int height;
        int size;
    public:
        char* data;
        double field = 0.00;
        double beta = 0.4406868; // critical beta
        double jay = 1.0;
        double mew = 1.0;

        double time = 0.0;
        double energy = 0.0;
        double mag = 0.0;
        
        std::mt19937 engine;
        std::uniform_int_distribution<int> coin;
        std::uniform_int_distribution<int> row_die;
        std::uniform_int_distribution<int> col_die;
        std::uniform_real_distribution<double> prob;
        std::stack<int> wolff_stack;
        
        Ising(int w, int h); 
        Ising(int w, int h, double b, double f);
        
        int GetWidth() { return width; }
        int GetHeight() { return height; }        
        
        bool HotStart();
        bool ColdStart();
        double CalcEnergy();
        double CalcMag();
        double DeltaEnergy(int i, int j);
        bool UpdateMetropolis();
        bool UpdateWolff();
        bool PrintData();
};

#endif
