//
//  Tomasulo.hpp
//  TomasuloSIM
//
//  Created by Marwan Awad on 12/11/20.
//

#ifndef Tomasulo_hpp
#define Tomasulo_hpp
//op code for the reservation station
#define Load1 1
#define Load2 2
#define Store1 3
#define Store2 4
#define Beq 5
#define JalrRet 6
#define AddNegAddi1 7
#define AddNegAddi2 8
#define Div 9
//Execution delays
#define LoadExec 1
#define StoreExec 1
#define BEQExec 1
#define JALRExec 1
#define ArithExec 2
#define DIVExec 8
//op code for operations
#define Load 1
#define Store 2
#define BEQ 3
#define JALR 4
#define RET 5
#define ADD 6
#define ADDI 7
#define NEG 8
#define DIV 9

#include <stdio.h>
#include "InitMem.hpp"
#include <string>
#include <sstream>
using namespace std;

struct ResStation{
    bool busy;
    int op;
    int VI;
    int VJ;
    int VK;
    int *QJ;
    int *QK;
    int imm;
    int A;
    int PC;
    bool ExecStatus;
};


class Tomasulo{
public:
    Tomasulo(string);
    
    void Program();
    void printTomasuloTable();
    void Execute(int, int&);
    void Issuing(string, int, int, int);
    bool checkExecution(int, int*, int*);
    void Operations(int, int, int, int, int, int, int&, int);
    bool checkReservationStation(string, int&);
private:
    InitMem Mem;
    int *Issue;
    int *Exec_Begin;
    int *Exec_end;
    int *WriteBack;
    int Regs[8];
    
    ResStation RS[10];
    
    int RegStat[8];
    
    int BranchCount;
    int BranchMissCount;
    
    int Cycles;
};
#endif /* Tomasulo_hpp */
