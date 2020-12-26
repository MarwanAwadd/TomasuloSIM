//
//  Tomasulo.cpp
//  TomasuloSIM
//
//  Created by Marwan Awad on 12/11/20.
//

#include "Tomasulo.hpp"

Tomasulo :: Tomasulo(string file){
    Mem.SetInstMem(file);
    
    for(int i = 1; i < 8; i++){
        RegStat[i] = 0;
    }
    
    for(int i = 1; i < 10; i++){
        RS[i].busy = false;
        RS[i].op = 0;
        RS[i].VI = 0;
        RS[i].VJ = 0;
        RS[i].VK = 0;
        RS[i].QJ = 0;
        RS[i].QK = 0;
        RS[i].imm = 0;
        RS[i].A = 0;
        RS[i].PC = 0;
        RS[i].ExecStatus = false;
        
    }
    
    Regs[0] = 0;
    
    Issue = new int[Mem.LastInstIndex];
    Exec_Begin = new int[Mem.LastInstIndex];
    Exec_end = new int[Mem.LastInstIndex];
    WriteBack = new int[Mem.LastInstIndex];
    
    BranchCount = 0;
    BranchMissCount = 0;
    Cycles = 1;
}

void Tomasulo :: printTomasuloTable(){
    for(int i = 0; i < Mem.LastInstIndex; i++){
        cout << Mem.InstMem[i] << "      " << Issue[i] << "      " << Exec_Begin[i]<< "      " << Exec_end[i] << "      " << WriteBack[i] << endl;
    }
    
    cout << "Total cycles: " << Cycles << endl;
    
    cout << "The IPC: " << Mem.LastInstIndex/Cycles << endl;
    if(BranchCount != 0)
        cout << "The Branch Misprediction Rate: " << 100*(BranchMissCount/BranchCount) << "%" << endl;
    else
        cout << "No Branch Instructions given.\n";
}

void Tomasulo :: Program(){
    int PC = 0;
    while(WriteBack[Mem.LastInstIndex] == 0){
        int station;
        if(PC == Mem.LastInstIndex + 1){
            Execute(Cycles, PC);
        }
        else{
            if(!(checkReservationStation(Mem.InstMem[PC], station))){
                Issuing(Mem.InstMem[PC], station, PC, Cycles);
                Execute(Cycles, PC);
                PC++;
            }
        }
        
        Cycles++;
    }
    printTomasuloTable();
}

void Tomasulo :: Issuing(string instruction, int station, int PC, int Cycles){
    string inst;
    int rd = 0;
    int rs1 = 0;
    int rs2 = 0;
    int imm = 0;
    stringstream s;
    s << instruction;
    s >> inst;
    if(inst == "LW"){
        s >> rd >> imm >> rs1;
        RS[station].op = Load;
        RS[station].A = imm;
    }
    else if(inst == "SW"){
        s >> rs2 >> imm >> rs1;
        RS[station].op = Store;
        RS[station].A = imm;

    }
    else if(inst == "BEQ"){
        s >> rs1 >> rs2 >> imm;
        RS[station].op = BEQ;
        RS[station].imm = imm + PC;
        BranchCount++;
    }
    else if(inst == "JALR"){
        s >> rs1;
        rd = 1;
        RS[station].op = JALR;
    }
    else if(inst == "ADD"){
        s >> rd >> rs1 >> rs2;
        RS[station].op = ADD;
    }
    else if(inst == "ADDI"){
        s >> rd >> rs1 >> imm;
        RS[station].op = ADDI;
        RS[station].imm = imm;
    }
    else if(inst == "NEG"){
        s >> rd >> rs1;
        RS[station].op = NEG;
    }
    else if(inst == "DIV"){
        s >> rd >> rs1 >> rs2;
        RS[station].op = DIV;
    }
    
    Issue[PC] = Cycles;
    
    RS[station].busy = true;
    RS[station].QJ = &RegStat[rs1];
    RS[station].QK = &RegStat[rs2];
    RS[station].VI = rd;
    RS[station].VJ = rs1;
    RS[station].VK = rs2;
    RS[station].PC = PC;
        
    RegStat[rd] = station;
}

bool Tomasulo :: checkExecution(int VI, int *QJ, int *QK){
    bool flag = false;
    if(*QJ == 0 && *QK == 0 && RegStat[VI] == 0)
        flag = true;
    else
        flag = false;
    return flag;
}

void Tomasulo :: Execute(int Cycles, int &PC){
    for(int i = 1; i < 10; i++){
        if(RS[i].ExecStatus){
            if(Cycles == WriteBack[RS[i].PC]){
                Operations(RS[i].VI, RS[i].VJ, RS[i].VK, RS[i].imm, RS[i].op, RS[i].A, PC, RS[i].PC);
                RegStat[RS[i].VI] = 0;
                RS[i].busy = false;
                RS[i].op = 0;
                RS[i].VI = 0;
                RS[i].VJ = 0;
                RS[i].VK = 0;
                RS[i].QJ = 0;
                RS[i].QK = 0;
                RS[i].imm = 0;
                RS[i].A = 0;
                RS[i].PC = 0;
                RS[i].ExecStatus = false;
            }
        }
        else{
            bool exec = checkExecution(RS[i].VI, RS[i].QJ, RS[i].QK);
            if(exec){
                switch(RS[i].op){
                    case Load:{
                        Exec_Begin[RS[i].PC] = Cycles + 2;
                        Exec_end[RS[i].PC] = Exec_end[RS[i].PC] + LoadExec;
                        WriteBack[RS[i].PC] = Exec_end[RS[i].PC] + 1;
                        break;
                    }
                    case Store:{
                        Exec_Begin[RS[i].PC] = Cycles + 1;
                        Exec_end[RS[i].PC] = Exec_end[RS[i].PC] + StoreExec;
                        WriteBack[RS[i].PC] = Exec_end[RS[i].PC] + 2;
                        break;
                    }
                    case BEQ:{
                        Exec_Begin[RS[i].PC] = Cycles + 1;
                        Exec_end[RS[i].PC] = Exec_end[RS[i].PC] + BEQExec;
                        WriteBack[RS[i].PC] = Exec_end[RS[i].PC] + 1;
                        break;
                    }
                    case JALR:{
                        Exec_Begin[RS[i].PC] = Cycles + 1;
                        Exec_end[RS[i].PC] = Exec_end[RS[i].PC] + JALRExec;
                        WriteBack[RS[i].PC] = Exec_end[RS[i].PC] + 1;
                        break;
                    }
                    case RET:{
                        Exec_Begin[RS[i].PC] = Cycles + 1;
                        Exec_end[RS[i].PC] = Exec_end[RS[i].PC] + JALRExec;
                        WriteBack[RS[i].PC] = Exec_end[RS[i].PC] + 1;
                        break;
                    }
                    case ADD:{
                        Exec_Begin[RS[i].PC] = Cycles + 1;
                        Exec_end[RS[i].PC] = Exec_end[RS[i].PC] + ArithExec;
                        WriteBack[RS[i].PC] = Exec_end[RS[i].PC] + 1;
                        break;
                    }
                    case ADDI:{
                        Exec_Begin[RS[i].PC] = Cycles + 1;
                        Exec_end[RS[i].PC] = Exec_end[RS[i].PC] + ArithExec;
                        WriteBack[RS[i].PC] = Exec_end[RS[i].PC] + 1;
                        break;
                    }
                    case NEG:{
                        Exec_Begin[RS[i].PC] = Cycles + 1;
                        Exec_end[RS[i].PC] = Exec_end[RS[i].PC] + ArithExec;
                        WriteBack[RS[i].PC] = Exec_end[RS[i].PC] + 1;
                        break;
                    }
                    case DIV:{
                        Exec_Begin[RS[i].PC] = Cycles + 1;
                        Exec_end[RS[i].PC] = Exec_end[RS[i].PC] + DIVExec;
                        WriteBack[RS[i].PC] = Exec_end[RS[i].PC] + 1;
                        break;
                    }
                }
                RegStat[RS[i].VI] = i;
            }
        }
    }
}

bool Tomasulo :: checkReservationStation(string instruction, int &station){
    string inst;
    stringstream s;
    s >> inst;
    bool busy = false;
    station = 0;
    if(inst == "LW"){
        if(RS[0].busy == true){
            if(RS[1].busy == true)
                busy = true;
            else{
                busy = false;
                station = 1;
            }
        }
        else{
            busy = false;
            station = 0;
        }
    }
    else if(inst == "SW"){
        if(RS[2].busy == true){
            if(RS[3].busy == true)
                busy = true;
            else{
                busy = false;
                station = 3;
            }
        }
        else{
            busy = false;
            station = 2;
        }
    }
    else if(inst == "BEQ"){
        if(RS[Beq].busy == true)
            busy = true;
        else{
            busy = false;
            station = Beq;
        }
    }
    else if(inst == "JAL" || inst == "RET"){
        if(RS[JalrRet].busy == true)
            busy = true;
        else{
            busy = false;
            station = JalrRet;
        }
    }
    else if(inst == "ADD" || inst == "ADDI" || inst == "NEG"){
        if(RS[AddNegAddi1].busy == true){
            if(RS[AddNegAddi2].busy == true)
                busy = true;
            else{
                busy = false;
                station = AddNegAddi2;
            }
        }
        else{
            busy = false;
            station = AddNegAddi1;
        }
    }
    else {//for DIV
        if(RS[Div].busy == true)
            busy = true;
        else{
            busy = false;
            station = Div;
        }
    }
    return busy;
}

void Tomasulo :: Operations(int VI, int VJ, int VK, int imm, int op, int A, int &PC, int RPC){
    Regs[0] = 0;
    switch(op){
        case Load:{
            Regs[VI] = Mem.DataMem[Regs[VJ] + A];
            break;
        }
        case Store:{
            Mem.DataMem[Regs[VJ]] = Regs[VK];
            break;
        }
        case RET:{
            PC = Regs[VJ];
            break;
        }
        case JALR:{
            PC = Regs[VJ];
            Regs[VI] = RPC + 1;
            break;
        }
        case BEQ:{
            if(Regs[VJ] == Regs[VK]){
                PC = PC + imm;
                BranchMissCount++;
            }
            break;
        }
        case ADD:{
            Regs[VI] = Regs[VK] + Regs[VJ];
            break;
        }
        case ADDI:{
            Regs[VI] = Regs[VJ] + imm;
            break;
        }
        case NEG:{
            Regs[VI] = -Regs[VJ];
            break;
        }
        case DIV:{
            Regs[VI] = Regs[VJ] / Regs[VK];
            break;
        }
    }
    
}
