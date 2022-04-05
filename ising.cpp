#include "Ising.h"

Ising::Ising(int w, int h)
{
    width = w;
    height = h;
    data = new char[w*h];
    
    // initialize RNG
    engine = std::mt19937(1);
    coin = std::uniform_int_distribution<int>(0,1);
    row_die = std::uniform_int_distribution<int>(0,(h>0)?h-1:0);
    col_die = std::uniform_int_distribution<int>(0,(w>0)?w-1:0);
    prob = std::uniform_real_distribution<double>(0,1);
}

Ising::Ising(int w, int h, double b, double f)
{
    width = w;
    height = h;
    beta = b;
    field = f;
    data = new char[w*h];
    
    // initialize RNG
    engine = std::mt19937(1);
    coin = std::uniform_int_distribution<int>(0,1);
    row_die = std::uniform_int_distribution<int>(0,(h>0)?h-1:0);
    col_die = std::uniform_int_distribution<int>(0,(w>0)?w-1:0);
    prob = std::uniform_real_distribution<double>(0,1);
}

bool Ising::HotStart()
{
    if (data==NULL)
        return false;
    for (int i=0; i<height; i++){
        for (int j=0; j<width; j++){
            data[i*width+j] = (coin(engine)==1)?1:-1;
        }
    }
    return true;
}

bool Ising::ColdStart()
{
    if (data==NULL)
        return false;
    for (int i=0; i<height; i++){
        for (int j=0; j<width; j++){
            data[i*width+j] = 1;
        }
    }
    return true;
}

double Ising::GetEnergy()
{   
    double site_energy = 0.0;
    double bond_energy = 0.0;
    if (data==NULL)
        return site_energy;
    for (int i=0; i<height; i++){
        for (int j=0; j<width; j++){
            int site = i*width + j;
            int left = i*width+(j+width-1)%width;
            int up = (i+height-1)%height*width+j;
            int right = i*width+(j+1)%width;
            int down = (i+1)%height*width+j;
            //printf("[%d:%d,%d,%d,%d]\n",site,left,up,right,down); 
            
            char spin = data[site];
            char neighbors = data[left] + data[up] + data[right] + data[down];
            site_energy -= spin;
            bond_energy -= neighbors*spin;
        }
    }
    //printf("Site Energy: %.1f\n",site_energy);
    //printf("Bond Energy: %.1f\n",bond_energy);
    return 0.5*jay*bond_energy + mew*field*site_energy;
}
        
double Ising::GetMag()
{
    double mag = 0.0;
    if (data==NULL)
        return mag;
    for (int i=0; i<height; i++){
        for (int j=0; j<width; j++){
            mag += data[i*width+j];
        }
    }
    return mew*mag;
}

double Ising::DeltaEnergy(int i, int j)
{
    if (data==NULL)
        return 0.0;
    
    int site = i*width + j;

    int left = i*width+(j+width-1)%width;
    int up = (i+height-1)%height*width+j;
    int right = i*width+(j+1)%width;
    int down = (i+1)%height*width+j;
    char neighbors = data[left] + data[up] + data[right] + data[down];
    
    return 2*(jay*neighbors + mew*field)*data[site];
}

bool Ising::UpdateMetropolis()
{
    if (data==NULL)
        return false;

    int i = row_die(engine);
    int j = col_die(engine);
    double delta = DeltaEnergy(i,j);
    if ((delta <= 0.0) || (exp(-delta*beta) >= prob(engine)))
        data[i*width+j] *= -1;
    return true;
}

bool Ising::PrintData()
{
    if (data==NULL)
        return false;
    for (int i=0; i<height; i++){
        for (int j=0; j<width; j++){
            std::cout << ((data[i*width+j]==1)?1:0) << ' ';
        }
        std::cout << std::endl;
    }
    return true;
}

