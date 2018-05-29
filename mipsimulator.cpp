/**
 * @package             Single Cycle MIPS Processor Simulator
 * @author              TechieForFun - https://techieforfun.com
 * @license             The Unlicense - http://unlicense.org
 * @copyright           Public Domain - All Rights Reserved Forever
 * @comment				Thanks to Open Source Community
 */

#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string>
#include <sstream>

using namespace std;


class helper{
    public:
        int intPow(int inpA, int inpB){
            int co = 0, value = 1;
            for (; co < inpB; co++){
                value *= inpA;
            }
            return value;
        }

        string decToBin(int inp){
            int co = 0, value = 0;
            while(inp >= 2){
                value += intPow(10,co) * (inp % 2);
                inp /= 2;
                co++;
            }
            value += intPow(10,co) * (inp);
            ostringstream ss;
            ss << value;
            return ss.str();
        }

        int binToDec(string inp){
            int co = 0, value = 0;
            for (; co < inp.length(); co++){
                value += intPow(2,co) * (inp.at(inp.length() - co - 1) - '0');
            }
            return value;
        }

        void showError(){
            cout << "Error" << endl;
            exit(EXIT_SUCCESS);
        }

        string fillReg(string inp){
            int co1 = inp.length() - 1, co2 = 31;
            string reg = "00000000000000000000000000000000";
            for (; co1 >= 0; co1-- , co2--){
                reg.at(co2) = inp.at(co1);
            }
            return reg;
        }
};


class dataMemory{
    public:
        string mem[1024]; // Every House Assumed: One MIPS Word = 32 Bit
        string dataOut;
        helper help;
        void getMemData(string ma, int memRead){
            if (memRead == 1){
                dataOut = mem[help.binToDec(ma)];
            }
        }
        void setMemData(string ma, int data, int memWrite){
            if (memWrite == 1){
                if (help.decToBin(data).length() > 32){
                    help.showError();
                }
                mem[help.binToDec(ma)] = help.decToBin(data);
            }
        }
};


class alu{
    public:
        int zero,result,gtzr;
        helper help;
        void operate(string inpA, string inpB,string func, string shamt){
            zero = 0, gtzr = 1;
            dataMemory memIns;
            if (func == "100000" || func == "001000"){//add, addi
                result = help.binToDec(inpA) + help.binToDec(inpB);
            } else if (func == "100010" || func == "000100"){//sub, beq
                result = help.binToDec(inpA) - help.binToDec(inpB);
            } else if (func == "100100" || func == "001100"){//and, andi
                result = help.binToDec(inpA) & help.binToDec(inpB);
            } else if (func == "100101" || func == "001101"){//or, ori
                result = help.binToDec(inpA) | help.binToDec(inpB);
            } else if (func == "100111"){//nor
                result = ~(help.binToDec(inpA) | help.binToDec(inpB));
            } else if (func == "100110" || func == "001110"){//xor, xori
                result = help.binToDec(inpA) ^ help.binToDec(inpB);
            } else if (func == "000000"){//sll
                result = help.binToDec(inpB) << help.binToDec(shamt);
            }

            if (result == 0){
                zero = 1;
                gtzr = 0;
            }
        }
};


class registerFile{
    public:
        string registers[32];// If Data Was More Than 32 Bit Lenght Then It Got Error Message
        helper help;
        string getRegData(string ra){
            string rd_data;
            if (help.binToDec(ra) < 32){
                rd_data = registers[help.binToDec(ra)];
                if (ra == "00000"){
                    rd_data = "0";
                }
                return rd_data;
            } else {
                help.showError();
            }
        }
        void setRegData(string ra,int data, int regWrite){
            if (regWrite == 1){
                helper help;
                string wdata = help.decToBin(data);
                if (wdata.length() > 32){
                    help.showError();
                }
                if (help.binToDec(ra) != 0 && help.binToDec(ra) < 32){
                    registers[help.binToDec(ra)] = wdata;
                } else {
                    help.showError();
                }
            }
        }
};


class controlUnit{
    public:
        string aluFunc;
        int aluSrc,memRead,memToReg,memWrite,pcSrc,regWrite,regDst;
        helper help;
        void checkCommand(string op, string func){
            aluSrc = 0;
            memRead = 0;
            memToReg = 0;
            memWrite = 0;
            pcSrc = 0;
            regWrite = 1;
            regDst = 0;
            aluFunc = op;
            if (op == "000000"){
                if (func == "100000"
                    || func == "000000"
                    || func == "100010"
                    || func == "100100"
                    || func == "100101"
                    || func == "100111"
                    || func == "100110"
                    ){//add, sll, sub, and, or, nor, xor
                    regDst = 1;
                }
                aluFunc = func;
            } else if (op == "001000"
                       || op == "001100"
                       || op == "001101"
                       || op == "001100"
                       || op == "001110"
                       ){//addi, andi, ori, xori
                aluSrc = 1;
            } else if (op == "000100"){//beq
                pcSrc = 1;
                regWrite = 0;
            } else if (op == "000010"){//j
                pcSrc = 2;
                regWrite = 0;
            } else if (op == "101011"){//sw
                memWrite = 1;
                regWrite = 0;
            } else if (op == "100011"){//lw
                memRead = 1;
                memToReg = 1;
            } else {
                help.showError();
            }
        }
};


class signExtender{
    public:
        string doSignExtending(string value){
            string extended = "00000000000000000000000000000000";
            int co;
            for (co = 0; co < 16; co++){
                extended.at(co) = value.at(0);
            }
            for (; co < 32; co++){
                extended.at(co) = value.at(co - 16);
            }
            return extended;
        }
};


class shifter{
    public:
        string shiftLeft(string value, int shv){
            int co1 = value.length() - 1, co2 = 31;
            string yield1 = "00000000000000000000000000000000", yield2 = "00000000000000000000000000000000";
            for (; co1 >= 0; co1--, co2--){
                yield1.at(co2) = value.at(co1);
            }
            for(co1 = 0; co1 < 32; co1++){
                if (co1 + shv < 32){
                    yield2.at(co1) = yield1.at(co1 + shv);
                } else {
                    yield2.at(co1) = '0';
                }
            }
            return yield2;
        }
};


class dataPath{
    public:
        string pc; //Assumed 32 Bit
        void printer(string strcode, string pc, string memValue, int address, registerFile rf){
            helper help;
            cout << "* Machine Code: " << strcode << endl;
            cout << "* PC: " << pc << endl;
            cout << "* Memory[" << help.decToBin(address) << "]: " << memValue << endl;
            cout << "* Register File:" << endl;
            cout << "00001(at): " << rf.registers[1] << endl;
            cout << "00010(v0): " << rf.registers[2] << endl;
            cout << "00011(v1): " << rf.registers[3] << endl;
            cout << "00100(a0): " << rf.registers[4] << endl;
            cout << "00101(a1): " << rf.registers[5] << endl;
            cout << "00110(a2): " << rf.registers[6] << endl;
            cout << "00111(a3): " << rf.registers[7] << endl;
            cout << "01000(t0): " << rf.registers[8] << endl;
            cout << "01001(t1): " << rf.registers[9] << endl;
            cout << "01010(t2): " << rf.registers[10] << endl;
            cout << "01011(t3): " << rf.registers[11] << endl;
            cout << "01100(t4): " << rf.registers[12] << endl;
            cout << "01101(t5): " << rf.registers[13] << endl;
            cout << "01110(t6): " << rf.registers[14] << endl;
            cout << "01111(t7): " << rf.registers[15] << endl;
            cout << "10000(s0): " << rf.registers[16] << endl;
            cout << "10001(s1): " << rf.registers[17] << endl;
            cout << "10010(s2): " << rf.registers[18] << endl;
            cout << "10011(s3): " << rf.registers[19] << endl;
            cout << "10100(s4): " << rf.registers[20] << endl;
            cout << "10101(s5): " << rf.registers[21] << endl;
            cout << "10110(s6): " << rf.registers[22] << endl;
            cout << "10111(s7): " << rf.registers[23] << endl;
            cout << "11000(t8): " << rf.registers[24] << endl;
            cout << "11001(t9): " << rf.registers[25] << endl;
            cout << "11010(k0): " << rf.registers[26] << endl;
            cout << "11011(k1): " << rf.registers[27] << endl;
            cout << "11100(gp): " << rf.registers[28] << endl;
            cout << "11101(sp): " << rf.registers[29] << endl;
            cout << "11110(s8): " << rf.registers[30] << endl;
            cout << "11111(ra): " << rf.registers[31] << endl;
            cout << "*************************************" << endl << endl;
        }
        int getNumOfLines(char fileName[]){
            fstream source;
            source.open(fileName);
            string line;
            int numOfline;
            for (numOfline = 0; getline(source, line) ; ++numOfline);
            return numOfline;
        }
        string getCurrentLine(char fileName[],int lineNum){
            fstream source;
            source.open(fileName);
            string line;
            int numOfline;
            if (source.is_open()) {
                for (numOfline = 0; numOfline < lineNum ; numOfline++)
                    getline(source, line);
                source.close();
            }
            return line;
        }
        void run(){
            char fileName[] = "sampleCode.txt";

            int numOfLines = getNumOfLines(fileName);

            helper help;

            pc = "00000000000000000000000000000100";

            int pcVal = help.binToDec(pc), address = 0, co1, flag = 1;;

            dataMemory memIns;

            string strcode, opCode, functionCode, alufunc, data1, data2, wrReg;

            controlUnit cu;

            registerFile rf;

            alu aluIns;

            signExtender se;

            shifter shIns;

            while (pcVal/4 <= numOfLines) {
                strcode = getCurrentLine(fileName, pcVal/4);

                for(co1 = 0; co1 < 32; co1++){
                    if (strcode.at(co1) < '0' || strcode.at(co1) > '9')
                        flag = 0;
                }

                if (strcode.length() != 32 || flag == 0){
                    help.showError();
                }

                opCode = strcode.substr(0,6);
                functionCode = strcode.substr(26,6);

                cu.checkCommand(opCode,functionCode);
                alufunc = cu.aluFunc;

                data1 = rf.getRegData(strcode.substr(6,5));

                if (cu.aluSrc == 0){
                    data2 = rf.getRegData(strcode.substr(11,5));
                } else if (cu.aluSrc == 1){// I Format
                    data2 = se.doSignExtending(strcode.substr(16,16));
                }

                aluIns.operate(data1,data2,alufunc,strcode.substr(21,5));


                if (cu.regDst == 0){//I Format
                    wrReg = strcode.substr(11,5);
                } else if (cu.regDst == 1){
                    wrReg = strcode.substr(16,5);
                }

                address = help.binToDec(rf.getRegData(strcode.substr(6,5))) + help.binToDec(se.doSignExtending(strcode.substr(16,16)));

                // Access Violation error prevention
                if (address > 1023 || address < 0){
                    address = 0;
                }

                memIns.setMemData(help.decToBin(address),help.binToDec(rf.getRegData(strcode.substr(11,5))),cu.memWrite);


                if (cu.memToReg == 0){
                    rf.setRegData(wrReg,aluIns.result,cu.regWrite);
                } else if (cu.memToReg == 1){//Load Word
                    address = help.binToDec(rf.getRegData(strcode.substr(6,5))) + help.binToDec(se.doSignExtending(strcode.substr(16,16)));
                    memIns.getMemData(help.decToBin(address),cu.memRead);
                    rf.setRegData(strcode.substr(11,5),help.binToDec(memIns.dataOut), cu.regWrite);
                }

                // This is for making the length of registers as 32 bit. It's not in datapath but it's in Program
                for (co1 = 1; co1 < 32; co1++){
                    rf.registers[co1] = help.fillReg(rf.registers[co1]);
                }

                printer(strcode, pc, memIns.mem[address], address, rf);

                if (aluIns.zero == 1 && cu.pcSrc == 1){//Branch
                    pcVal += 4 + help.binToDec(shIns.shiftLeft(se.doSignExtending(strcode.substr(16,16)),2));
                    pc = help.fillReg(help.decToBin(pcVal));
                } else if (cu.pcSrc == 2){//Jump
                    pcVal = help.binToDec(shIns.shiftLeft(strcode.substr(6,26),2));
                    for (co1 = 4; co1 < 32; co1++){
                        pc.at(co1) = (shIns.shiftLeft(strcode.substr(6,26),2)).at(co1);
                    }
                } else {
                    pcVal += 4;
                    pc = help.fillReg(help.decToBin(pcVal));
                }

            }
            cout << endl << "The end of code." << endl << "Developer: TechieForFun" << endl;
        }
};


int main() {
    dataPath rn;
    rn.run();
    return 0;
}
