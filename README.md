# Operating System Course Project

This repo is for course project at the University of Oklahoma operating system (FA25).   
This contain four kernel simulations in C++ covering process scheduling, resource allocation, and memory management.

## Project overview
Project1: Round-Robin Scheduler  
Project2: Priority + Round-Robin  
Project3: Banker's Algorithm  
Project4: LRU Page Replacement  

## Technical Details
Language: C++11  
Compiler: GCC/G++ or Clang++  
Input: Standard input via redirection  
Output: Process states, metrics, safety decisions, or page replacement info  

## quick start 
###  Compile
Make sure you have `g++` installed (with C++11 support or later).
```bash
g++ -std=c++11 pcb_simulator.cpp -o pcb_simulator
```
### Run
```bash
./pcb_simulator < input.txt
```
repeat the above two step for each project, please note the file name might be different and need to be changed
