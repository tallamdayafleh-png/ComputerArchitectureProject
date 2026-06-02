#include <iostream>
#include <string>
#include <fstream>

using namespace std;

// binary to int
int binToInt(string s){
    return stoi(s, nullptr, 2);
}


// binary to correct int
int convertSigned(int value, int bits){
    int maxValue = 1 << bits; // move binary left one time.. 2^ bits
    int half = maxValue / 2;

    if(value >= half){ // checking if negative
        value = value - maxValue; // if greater then negative
    }

    return value;
}

// global variables
int pc = 0; // program counter
int next_pc = 0; 
int branch_target = 0; 
int total_clock_cycles = 0;

int rf[32] = {0}; // register file
int d_mem[32] = {0}; // data memory

int RegWrite = 0;
int MemRead = 0;
int MemWrite = 0;
int MemtoReg = 0;
int ALUSrc = 0;
int Branch = 0;
int Jump = 0;
int Jalr = 0;

int alu_zero = 0;
int alu_result = 0;
int mem_data = 0;
int link_address = 0;



string current_op = "";



int rs1_i = 0;
int rs2_i = 0;
int rd_i = 0;
int imm_i = 0;


int read_data1 = 0;
int read_data2 = 0;


string program[100];
int program_size = 0;


// reset control signals to 0 !!
void resetControl(){
    RegWrite = 0;
    MemRead = 0;
    MemWrite = 0;
    MemtoReg = 0;

    ALUSrc = 0;
    Branch = 0;
    Jump = 0;
    Jalr = 0;
}


// control unit
void ControlUnit(string opcode){
    resetControl();

    // R type
    if(opcode == "0110011"){
        RegWrite = 1;
        ALUSrc = 0;

    }

    // I type addi, andi, ori
    else if(opcode == "0010011"){
        RegWrite = 1;
        ALUSrc = 1;
    }

    // lw
    else if(opcode == "0000011"){
        RegWrite = 1;
        MemRead = 1;
        MemtoReg = 1;
        ALUSrc = 1;


    }

    // sw
    else if(opcode == "0100011"){
        MemWrite = 1;
        ALUSrc = 1;

    }

    // beq
    else if(opcode == "1100011"){
        Branch = 1;
        ALUSrc = 0;

    }

    // jal
    else if(opcode == "1101111"){
        RegWrite = 1;
        Jump = 1;

    }

    // jalr
    else if(opcode == "1100111"){
        RegWrite = 1;
        Jump = 1;
        Jalr = 1;
        ALUSrc = 1;

    }
}


// Fetch function
string Fetch(){
    int index = pc / 4;

    if(index < 0 || index >= program_size){
        return "";
    }

    string instr = program[index];

    next_pc = pc + 4;

    return instr;
}


// Decode function
void Decode(string instr){

    //  figure out opcode first
    string opcode = instr.substr(25,7); // take 7 bits starting at index 25

    ControlUnit(opcode);

    current_op = "";
    rs1_i = 0;
    rs2_i = 0;
    rd_i = 0;
    imm_i = 0;


    // R TYPE 
    // [ funct7 ][ rs2 ][ rs1 ][ funct3 ][ rd ][ opcode ]
    if(opcode == "0110011"){
        string op;

        string rd = instr.substr(20,5); // take 5 bits at index 20

        string funct3 = instr.substr(17,3); // take 3 bits at index 17

        string rs1 = instr.substr(12,5); // take 5 bits at index 12
        string rs2 = instr.substr(7,5); // take 5 bits at index 7

        string funct7 = instr.substr(0,7); // take 7 bits at index 0 and so on...

        // binary to int
        rd_i = binToInt(rd);
        rs1_i = binToInt(rs1);
        rs2_i = binToInt(rs2);

        int funct3_i = binToInt(funct3);
        int funct7_i = binToInt(funct7);

        // determining operation using the funct 7 and funct 3 bits
        if(funct3 == "000" && funct7 == "0000000") op = "add";

        else if(funct3 == "000" && funct7 == "0100000") op = "sub";
        else if(funct3 == "111") op = "and";
        else if(funct3 == "110") op = "or";
        else if(funct3 == "100") op = "xor";
        else if(funct3 == "001") op = "sll";
        else if(funct3 == "101" && funct7 == "0000000") op = "srl";
        else if(funct3 == "101" && funct7 == "0100000") op = "sra";
        else if(funct3 == "010") op = "slt";
        else if(funct3 == "011") op = "sltu";

        current_op = op;
    }



    // I TYPE 
    //[ imm[11:0] ][ rs1 ][ funct3 ][ rd ][ opcode ]
    else if(opcode=="0010011" || opcode=="0000011" || opcode=="1100111"){
        string op;

        string rd = instr.substr(20,5);
        string funct3 = instr.substr(17,3);
        string rs1 = instr.substr(12,5);
        string imm = instr.substr(0,12);

        rd_i = binToInt(rd);
        rs1_i = binToInt(rs1);
        int funct3_i = binToInt(funct3);
        imm_i = convertSigned(binToInt(imm),12); // 12 bits in imm



        // determine I type instruction using funct 3
        if(opcode=="0010011"){ // I type opcode
            if(funct3=="000") op="addi";
            else if(funct3=="111") op="andi";
            else if(funct3=="110") op="ori";
        }

        if(opcode=="0000011"){ // I type opcode
            if(funct3=="010") op="lw";
        }

        if(opcode=="1100111"){
            op = "jalr";
        }

        current_op = op;
    }


    //  S TYPE 
    //[ imm[11:5] ][ rs2 ][ rs1 ][ funct3 ][ imm[4:0] ][ opcode ]
    // sb, sh, sw 
    else if(opcode=="0100011"){
        string op; 
        string funct3 = instr.substr(17,3);
        string rs1 = instr.substr(12,5);
        string rs2 = instr.substr(7,5);
        
        // building the immediant for s type
        string imm = instr.substr(0,7) + instr.substr(20,5); // imm 11:5 + imm 4:0

        //converting to int
        rs1_i = binToInt(rs1);
        rs2_i = binToInt(rs2);

        int funct3_i = binToInt(funct3);
        imm_i = convertSigned(binToInt(imm),12); // 12 bits in imm


        // determine the S type instruction based off funct3
        if(funct3 == "000") op="sb";
        else if(funct3 == "001") op="sh";
        else if(funct3 == "010") op="sw";

        current_op = op;
    }

    // SB TYPE 
    //[ imm[12] ][ imm[10:5] ][ rs2 ][ rs1 ][ funct3 ][ imm[4:1] ][ imm[11] ][ opcode ]
    // beq, bne, blt, bge
    else if(opcode == "1100011"){
        string op;

        string funct3 = instr.substr(17,3);
        string rs1 = instr.substr(12,5);
        string rs2 = instr.substr(7,5);

        string imm;
        imm += instr[0]; // add imm 12
        imm += instr.substr(24,1); // add imm 11
        imm += instr.substr(1,6); // add imm10:5
        imm += instr.substr(20,4); // add imm 4:1
        imm += "0"; // last imm always 0


        //Convert to int
        rs1_i = binToInt(rs1);
        rs2_i = binToInt(rs2);

        int funct3_i = binToInt(funct3);
        imm_i = convertSigned(binToInt(imm),13); // 13 bits in imm

        
        // determine SB type instruction based off funct3
        if(funct3=="000") op="beq";
        else if(funct3=="001") op="bne";
        else if(funct3=="100") op="blt";
        else if(funct3=="101") op="bge";

        current_op = op;
    }


    // UJ TYPE jal
    //[ imm[20] ][ imm[10:1] ][ imm[11] ][ imm[19:12] ][ rd ][ opcode ]
    else if(opcode=="1101111"){

        string rd = instr.substr(20,5);
        string imm;

        imm += instr[0]; // imm 20 
        imm += instr.substr(12,8); // add imm 19:12
        imm += instr.substr(11,1); // add imm 11
        imm += instr.substr(1,10); // add imm 10:1
        imm += "0";

        rd_i = binToInt(rd);
        imm_i = convertSigned(binToInt(imm),21); // 21 bits in imm

        current_op = "jal";
    }

    read_data1 = rf[rs1_i];
    read_data2 = rf[rs2_i];
}


// Execute function
void Execute(){
    int input1 = read_data1;
    int input2;

    if(ALUSrc == 1){

        input2 = imm_i;
    }
    else{
        input2 = read_data2;

    }

    if(current_op == "add" || current_op == "addi" || current_op == "lw" || current_op == "sw"){
        alu_result = input1 + input2;
    }

    else if(current_op == "sub" || current_op == "beq"){

        alu_result = input1 - input2;
    }

    else if(current_op == "and" || current_op == "andi"){

        alu_result = input1 & input2;
    }
    else if(current_op == "or" || current_op == "ori"){

        alu_result = input1 | input2;
    }

    if(alu_result == 0){

        alu_zero = 1;
    }
    else{

        alu_zero = 0;
    }

    branch_target = next_pc + imm_i;

    link_address = next_pc;


    // extended
    if(current_op == "jal"){
        branch_target = pc + imm_i;
    }

    if(current_op == "jalr"){
        branch_target = read_data1 + imm_i;
    }
}


// Mem function
void Mem(){
    mem_data = 0;

    int index = alu_result / 4;

    if(MemRead == 1){
        mem_data = d_mem[index];
    }

    if(MemWrite == 1){
        d_mem[index] = read_data2;

        cout << "memory 0x" << hex << alu_result << " is modified to 0x" << read_data2 << dec << endl; // reads in hex
    }
}

// register to name
string regName(int r){

    if(r == 1) return "ra";
    
    else if(r == 8) return "s0";
    else if(r == 10) return "a0";
    else if(r == 11) return "a1";
    else if(r == 12) return "a2";
    else if(r == 13) return "a3";
    else if(r == 30) return "t5";
    else return "x" + to_string(r);
}

// WriteBack function
void WriteBack(){
    if(RegWrite == 1 && rd_i != 0){
        if(Jump == 1){
            rf[rd_i] = link_address;
        }
        else if(MemtoReg == 1){
            rf[rd_i] = mem_data;
        }
        else{
            rf[rd_i] = alu_result;
        }

       cout << regName(rd_i) << " is modified to 0x" << hex << rf[rd_i] << dec << endl; 

    }

    total_clock_cycles++;

    if(Jump == 1){
        pc = branch_target;
    }
    else if(Branch == 1 && alu_zero == 1){
        pc = branch_target;
    }
    else{
        pc = next_pc;
    }
     cout << "pc is modified to 0x" << hex << pc << dec << endl;

}


int main(){

    string filename; // to store the txt file
    string instr; // to store the intsruction to read

    cout << "Enter the program file name to run: " << endl;
    cin >> filename; 

    ifstream file(filename);

    if(!file){
        cout << "Could not open file!!!!" << endl;
        return 0;
    }

    while(file >> instr){
        if(instr.length() == 32){
            program[program_size] = instr;
            program_size++;
        }
        else{
            cout << "Invalid instruction..." << endl;
        }
    }

    file.close();

    
    
    // if the file is sample_part1.txt, use part 1 initialization
    if(filename == "sample_part1.txt"){
    // to initialize register file for sample_part1
    // x1 = 0x20, x2 = 0x5, x10 = 0x70, x11 = 0x4

        rf[1] = 0x20;    // x1 = 32
        rf[2] = 0x5;     // x2 = 5
        rf[10] = 0x70;   // x10 = 112
        rf[11] = 0x4;    // x11 = 4

        // initialize d_mem array for sample_part1
        // memory address 0x70 = 0x5
        // memory address 0x74 = 0x10

        d_mem[0x70 / 4] = 0x5;
        d_mem[0x74 / 4] = 0x10;
    }

// if the file is sample_part2.txt, use part 2 initialization
else if(filename == "sample_part2.txt"){
    // initialize register file for sample_part2
    // s0 = 0x20, a0 = 0x5, a1 = 0x2, a2 = 0xa, a3 = 0xf

    rf[8]  = 0x20;   // s0 = 32
    rf[10] = 0x5;    // a0 = 5
    rf[11] = 0x2;    // a1 = 2
    rf[12] = 0xa;    // a2 = 10
    rf[13] = 0xf;    // a3 = 15

}

    

    while(pc / 4 < program_size){
        string fetched = Fetch();

        if(fetched == ""){
            break;
        }

        Decode(fetched);
        Execute();
        

        cout << "total_clock_cycles " << total_clock_cycles + 1 << " :" << endl;

        Mem();

        WriteBack();

        cout << endl;
    }

    cout << "program terminated:" << endl;
    cout << "total execution time is " << total_clock_cycles << " cycles" << endl;

    return 0;
}