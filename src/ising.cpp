#include "ising.h"

Ising::Ising(int w, int h)
{
    width = w;
    height = h;
    size = w*h;
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
    size = w*h;

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
    for (int i=0; i<height; i++)
    {
        for (int j=0; j<width; j++)
        {
            data[i*width+j] = (coin(engine)==1)?1:-1;
        }
    }
    energy = CalcEnergy();
    mag = CalcMag();
    return true;

}

bool Ising::ColdStart()
{
    if (data==NULL)
        return false;
    for (int i=0; i<height; i++)
    {
        for (int j=0; j<width; j++)
        {
            data[i*width+j] = 1;
        }
    }
    energy = CalcEnergy();
    mag = CalcMag();
    return true;
}

double Ising::CalcEnergy()
{   
    double site_energy = 0.0;
    double bond_energy = 0.0;
    if (data==NULL)
        return site_energy;
    for (int i=0; i<height; i++)
    {
        for (int j=0; j<width; j++)
        {
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
    return (0.5*jay*bond_energy + mew*field*site_energy)/size;
}
        
double Ising::CalcMag()
{
    double mag = 0.0;
    if (data==NULL)
        return mag;
    for (int i=0; i<height; i++)
    {
        for (int j=0; j<width; j++)
        {
            mag += data[i*width+j];
        }
    }
    return mew*mag/size;
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
    {
        data[i*width+j] *= -1;
        energy += delta/size;
        mag += 2*mew*data[i*width+j]/size;
    }
    time += 1.0/size;   
    return true;
}

bool Ising::UpdateWolff()
{
    if (data==NULL)
    {
        return false;
    }

    int i = row_die(engine);
    int j = col_die(engine);

    int site = i*width+j;
    wolff_stack.push(site);
    
    double dE = 0.0;
    double dM = 0.0;
    
    int original_spin = data[site];
    data[site] *= -1;
    dM += 2*data[site];
    int flipped = 1;

    double flip_prob = 1-exp(-2*beta*jay);
    while (!wolff_stack.empty())
    {
        int current_site = wolff_stack.top();
        wolff_stack.pop();

        int left = ((current_site % width)==0)?(current_site+width-1):(current_site-1);
        int up = (current_site - width + size)%size;
        int right = ((current_site % width)==(width-1))?(current_site-width+1):(current_site+1);
        int down = (current_site + width)%size;
        
        int neighbor_sum = data[left] + data[up] + data[right] + data[down];

        dE -= 2.0*(jay*neighbor_sum + mew*field)*data[site];

        if ((data[left] == original_spin) && (prob(engine) <= flip_prob))
        {
            data[left] *= -1;
            dM += 2*data[left];
            flipped++;
            wolff_stack.push(left);
        }
        if ((data[up] == original_spin) && (prob(engine) <= flip_prob))
        {
            data[up] *= -1;
            dM += 2*data[up];
            flipped++;
            wolff_stack.push(up);
        }
        if ((data[right] == original_spin) && (prob(engine) <= flip_prob))
        {
            data[right] *= -1;
            dM += 2*data[right];
            flipped++;
            wolff_stack.push(right);
        }
        if ((data[down] == original_spin) && (prob(engine) <= flip_prob))
        {
            data[down] *= -1;
            dM += 2*data[down];
            flipped++;
            wolff_stack.push(down);
        }
    }

    // Update variables
    mag += dM/size;
    //energy += dE/(width*height); dE calculation NYI -> make sure to update energy with CalcEnergy()
    time += (1.0*flipped)/size;
    return true;
}

bool Ising::PrintData()
{
    if (data==NULL)
        return false;
    for (int i=0; i<height; i++)
    {
        for (int j=0; j<width; j++)
        {
            std::cout << ((data[i*width+j]==1)?1:0) << ' ';
        }
        std::cout << std::endl;
    }
    return true;
}

