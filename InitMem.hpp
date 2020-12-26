//
//  InitMem.hpp
//  TomasuloSIM
//
//  Created by Marwan Awad on 12/9/20.
//

#ifndef InitMem_hpp
#define InitMem_hpp

#include <stdio.h>
#include <iostream>
#include <string>
#include <fstream>
using namespace std;

class InitMem{
public:
    InitMem();
    void SetInstMem(string);
    
    int LastInstIndex;
    string InstMem[128];
    int DataMem[4096];    
};
#endif /* InitMem_hpp */
