# ImIsing

ImIsing is a simulation of the ubiquitous [2D Ising model](https://en.wikipedia.org/wiki/Square_lattice_Ising_model), equipped with a fully interactive GUI using the popular C++ immediate mode GUI [Dear ImGui](https://github.com/ocornut/imgui). With the goal of being cross-platform, graphics are rendered using OpenGL and context/windows are managed by SDL.

![app](https://user-images.githubusercontent.com/35658028/164163370-2878cf72-8949-47cd-8e19-5b26f80417ef.png)

## Features

ImIsing allows the user to visualize the spin lattice in real-time as it evolves, with two distinct modes of operation.

 First, in free-play mode, the various physical parameters such as temperature and magnetic field can be changed in real-time by using the sliders on the left side of the screen. The simulation can be paused and stepped through manually using the "Step" button. 
 
 The energy and magnetization of the lattice are calculated and plotted on the right side using the third-party Dear ImGui plugin [ImPlot](https://github.com/epezent/implot). Lattice screenshot pngs as well as gifs up to 10 seconds long can be saved with the help of the single-file libraries [stb](https://github.com/nothings/stb) and [gif-h](https://github.com/charlietangora/gif-h).
 
 <img src="https://user-images.githubusercontent.com/35658028/164157151-47a93006-e500-4e98-bf22-42a3bb3178ed.gif" width="400"/> <img src="https://user-images.githubusercontent.com/35658028/164157463-260fa044-9898-48ce-8ef8-ad4302f2e5ae.gif" width="400"/>

Second, in collect-mode, the simulation begins a series of simulated annealing runs for a fixed number of steps at various fixed temperature values. The results are collected and plotted, allowing the user to investigate various parameters such as the average energy, average magnetization, specific heat, and susceptibility. While ImPlot provides great immediate feedback, those seeking to analyze the results further can download the data formatted as a csv.

<img src="https://user-images.githubusercontent.com/35658028/164164186-78db3f63-f742-4d84-964c-f2c676a91752.png"/> <img src="https://user-images.githubusercontent.com/35658028/164163827-53302fd6-02e5-4a9a-941b-c3d2d8d0147e.png"/>

Note that play-mode is disabled while an experiment is being ran, and vice-versa. In order to re-enable play mode, the current experiment must either finish or be manually reset by the user by pressing the "Reset" button on the bottom-righthand side.

The default size of the lattice is 400x400, which can be increased to 800x800 at the cost of performance. Numerous sizes all the way down to 25x25 are supported and  can be change by using the size drop-down box.

The colors of the lattice (default blue for spin-up and black for spin-down) can also be freely changed using the color picker widgets.

## Installation

You may need to first install SDL
```bash
sudo apt-get install libsdl2-dev
```

The following package is useful for GL related tools
```bash
sudo apt-get install mesa-utils
```

If you are working on WSL, you will need to configure X11 forwarding. I recommend [VcXsrv](https://sourceforge.net/projects/vcxsrv/). When using XLaunch make sure to disable "native opengl".

![xlaunch](https://user-images.githubusercontent.com/35658028/164176602-67d9dc0b-d2b3-439c-98f1-cd56505e1ac2.png)

Once the pre-requisite software is installed, simply clone this repository and compile the code using the supplied ```Makefile``` (make sure ```g++``` is installed).

```bash
git clone https://github.com/alexeigrinberg/ImIsing.git
cd src
make
```

Currently, the software must be run from the ```src``` directory.
```bash
cd src
./ising
```

## Implementation

The 2D Ising model is simulated using either of two update algorithms. By default, the [Metropolis-Hastings](https://en.wikipedia.org/wiki/Metropolis%E2%80%93Hastings_algorithm) algorithm is used. However, by checking the "Wolff Cluster" box, the user can switch to the [Wolff Cluster](https://en.wikipedia.org/wiki/Wolff_algorithm) algorithm for better results. Instead of attempting to flip one random spin at a time, the Wolff algorithm attempts to construct a cluster of parallel spins to flip simultaneously (for further reading, see [this paper](https://csml.northwestern.edu/resources/Reprints/lnp_color.pdf)).

In order to calculate statistical information such as the average and variance of the energy and magnetization distributions, [Welford's online algorithm](https://en.wikipedia.org/wiki/Algorithms_for_calculating_variance#Welford's_online_algorithm) is used so that only a single pass of the data is required.

It is necessary to make sure the lattice is properly thermalized before taking data. As such, a fixed number of initial sweeps will be ignored for the purposes of calculating averages and variances. This greatly improves the accuracy of the plots.

### Conventions

**Beta** refers to "inverse temperature", or more specifically ![](https://latex.codecogs.com/svg.image?\beta&space;=&space;\frac{1}{k_B&space;T}) where ![](https://latex.codecogs.com/svg.image?k_B) is Boltzmann's constant and is set to 1.

**Hot start** refers to starting the lattice in a random spin configuration, as if temperature were infinite. Conversely, a **cold start** refers to starting the lattice in an all spin-up configuration, as if temperature were zero.

Time is calculated in **sweeps**. One sweep refers to ![](https://latex.codecogs.com/svg.image?N^2) attempted spin flips. When using the Metropolis algorithm, time will always advance in steps of one sweep at a time. 

However, when using the Wolff algorithm, since each spin flipped counts for ![](https://latex.codecogs.com/svg.image?\frac{1}{N^2}) sweeps, time will advance in fractional steps. Additionally, in this mode the lattice image is only rendered every other sweep to prevent rapid flickering (when the entire lattice flips from spin-up to spin-down).

All of the plotted quantities are measured per spin. Additionally, magnetization is plotted in absolute value.

