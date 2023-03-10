<a name="readme-top"></a>

<!-- SHIELDS -->
<div align="center">
  <img src="https://img.shields.io/badge/C-00599C?style=for-the-badge&logo=c&logoColor=white"/>
</div>

<!-- HEADER -->
<div align="center">
  <h3 align="center">Game of Life</h3>
  <p align="center">
    This is simple demonstration of <a href="https://en.wikipedia.org/wiki/Conway's_Game_of_Life">Conway's "Game of Life"</a>.
  </p>
</div>

<!-- TABLE OF CONTENTS -->
## **Table of Contents**

- [About The Repository](#about-the-repository)
- [Getting Started](#getting-started) 
    - [Prerequisites](#prerequisites) 
    - [Installation](#installation)
        - [Local](#local)
        - [Lisa Cluster](#lisa-cluster)


<!-- ABOUT THE REPOSITORY -->
## About The Repository
---
This is simple demonstration of [Conway's "Game of Life"](https://en.wikipedia.org/wiki/Conway's_Game_of_Life) for the third assignment of the course 'High Performance Computing and Big Data' performed by Group 3.

<p align="right">(<a href="#readme-top">back to top</a>)</p>

<!-- GETTING STARTED -->
## Getting Started
---
### Prerequisites
- [OpenMP](https://en.wikipedia.org/wiki/OpenMP)

### Installation
A step by step series of examples that tell you how to get a version running.

1. **Clone the [Repository](https://github.com/dazinesal/Game-of-Life)**.\
Open the terminal and head over to a suitable folder and run the following command: 
```sh
git clone git@github.com/dazinesal/Game-of-Life.git
```

#### Local
2. **Build the Repository**.\
In the terminal, write the following command to build the object files,
```sh
make
```

3. **Run the Repository**.\
After, you can write the following command to run the program,
```sh
make run
```

#### Lisa Cluster
2. **Build the Repository**.\
In the terminal, write the following command to build the object files,
```sh
gcc -fopenmp main.c -o main.o
```

3. **Queue the Build**.\
After, you can write the following command to run the program,
```sh
sbatch main.sh
```

4. **Retrieve the Output**.\
Use the following command to see the job list,
```sh
squeue -u $LOGNAME
```
After the job is done, you can read the output/err with the following command,
```sh
cat gol_<PartitionID>.[out|err]
```

<p align="right">(<a href="#readme-top">back to top</a>)</p>