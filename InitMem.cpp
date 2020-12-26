//
//  InitMem.cpp
//  TomasuloSIM
//
//  Created by Marwan Awad on 12/9/20.
//

#include "InitMem.hpp"

InitMem :: InitMem (){
    for(int i = 0; i < 4096; i++){
        DataMem[i] = 0;
    }
    
}

void InitMem :: SetInstMem(string file){
    ifstream myfile1 (file);
    if(myfile1.is_open()){
        while ( getline(myfile1, InstMem[LastInstIndex]) )
        {
            cout << InstMem[LastInstIndex] << endl;
            LastInstIndex++;
        }
        myfile1.close();
    }
    else  cout << "Unable to open file" << endl;
}
