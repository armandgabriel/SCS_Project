#define PSAPI_VERSION 1
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <windows.h>
#include <tlhelp32.h>
#include <cstdlib>
#include <ctime>

int len(std::string str) {
	int length = 0;
	for (int i = 0; str[i] != '\0'; i++)
	{
		length++;
	}
	return length;
}

std::string replaceStr(std::string orig_str, const std::string& replace_string, const std::string& with_string){
    int pos = orig_str.find(replace_string);
    //if(pos != std::string::npos)
     while(pos != std::string::npos) {
        orig_str.replace(pos, replace_string.length(),with_string);
        pos = orig_str.find(replace_string, pos + 1);
     }
    return orig_str;
    }

HANDLE GetProcessHandle(const char *procName)
{
    HANDLE hProc = NULL;
    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (Process32First(hSnapshot, &pe32)) {
        do {
            if (!strcmp(pe32.szExeFile, procName)) {
                hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
                break;
            }
        } while (Process32Next(hSnapshot, &pe32));
    }
    CloseHandle(hSnapshot);
    return hProc;
}

DWORDLONG getTotalPhysMemUssage(std::string startAdd) {
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);
    
    DWORDLONG physMemUsed = memInfo.ullTotalPhys - memInfo.ullAvailPhys;
    DWORDLONG virtualMemUsed = memInfo.ullTotalPageFile - memInfo.ullAvailPageFile;
    std::ostringstream sizeInPhysMemStr;
    sizeInPhysMemStr << physMemUsed;

    std::string physMemSizeStr(sizeInPhysMemStr.str());

    std::cout << "STARTING ADD IN MEM : " << startAdd << " AND SIZE : " << physMemUsed << " VIRTUAL MEMORY USED: " << virtualMemUsed << std::endl;

    char const *c = startAdd.c_str();
    unsigned int i = reinterpret_cast<unsigned int>( c );
    physMemUsed = physMemUsed + i;
    DWORDLONG t = 9223372036854775807 - (physMemUsed - virtualMemUsed) << physMemUsed;
     
    return t;
}

int bitExtracted(int number, int k, int p)
{
    return (((1 << k) - 1) & (number >> (p - 1)));
}

long leftRotate(unsigned n, unsigned d){ //rotate n by d bits
     return (n << d)|(n >> (16 - d));
}

long negativeToPositive (long a) { 
    long b = a * 2;
    long c = a + b;
    return c; 
}

long generateRandomNo(int randomNumber) {


    // STEP 1 ++++++++++++++++++++++++
    const char *procName = "main.exe";
    HANDLE hProc = GetProcessHandle(procName);
    if (hProc) {
        std::cout << "STARTING MEMORY ADD IS: " << hProc << std::endl;
    }
    //auto startAdd = reinterpret_cast<std::uintptr_t>(p);
    std::ostringstream startAdd;
    startAdd << hProc;
    // STEP 2 
    DWORDLONG curProcSizeInPhysMem = getTotalPhysMemUssage(startAdd.str());

    std::cout << "SIZE IN MEMORY OF main.exe" << curProcSizeInPhysMem << std::endl;
    
    //int curProcSizeInPhysMemInInt = curProcSizeInPhysMem & 0xffffffff;
    //long tempLong = (int)(curProcSizeInPhysMemInInt >> 32);
    //int yourInt = (int)(curProcSizeInPhysMem - tempLong);
    unsigned int val32 = (curProcSizeInPhysMem >> 16);
    //std::cout << curProcSizeInPhysMemInInt << std::endl;
    //std::cout << tempLong << std::endl;
    //std::cout << yourInt << std::endl;
    std::cout << val32 << std::endl;
    int randomSeed = (randomNumber + val32) ^ 0x3bf23;
    std::cout << " FIRST RANDOM SEED: " << randomSeed << std::endl;

    int n = randomSeed & ((1 << 3) - 1);   

    long randomRotateStep1 = leftRotate(randomSeed, n);
    long finalRandomSeed = 0;
    if(randomRotateStep1 < 0) {
        std::cout << "OVERFLOW!" << std::endl;
        //finalRandomSeed = (randomRotateStep1 >> 8) - 0xFFFF;
        //finalRandomSeed = negativeToPositive(randomRotateStep1);
        finalRandomSeed = randomRotateStep1 & 0xffff;
    } else {
        std::cout << "NOT OVERFLOW!" << std::endl;
        finalRandomSeed = randomRotateStep1;
    }

    std::cout << "FINAL RANDOM SEED1: " << finalRandomSeed << std::endl;

    srand(finalRandomSeed);

    long finalRandom = rand() % 10000000;
    std::cout << "FINAL RANDOM2: "<< finalRandom << std::endl;

    n = 0;

    n = finalRandom & ((1 << 3) -1);

    long finalRandomRotated = leftRotate(finalRandom, n);


    std::cout << "FINAL RANDOM ROTATED: " << finalRandomRotated << std::endl;

    srand(finalRandomRotated);

    long random = rand();

    std::cout << "RANDOM NO: " << random << std::endl;

    return random;
}

void obfuscateProgram(long randomNo, std::string fileName) {
    std::ifstream myFile(fileName);
    std::stringstream buffer;
    buffer << myFile.rdbuf();
    
    //std::cout << buffer.str() << std::endl;

    int noOfLines = 0;

    std::vector<std::string> inputVector;

    for(std::string line; std::getline(buffer, line);) {
        std::cout << line << std::endl;
        noOfLines++;
        inputVector.push_back(line);
    }
    std::cout << "NO OF LINES: " << noOfLines << std::endl;

    for(std::string &line : inputVector) {
        std::cout << line << std::endl;
    }   

    // add defines
    std::string def1("\n#define zero true\n\0");
    std::string def2("#define one false\n\0");
    //std::string def3("#define i _______\n\0");
    //std::string def4("#define r ret\n\0");
    //std::string def5("#define x ch\n\0");
    std::string def7("#define ctrl el\n\0");
    std::string def8("#define vote long\n\0");
    //std::string def9("#define _24_g main");
    std::string def10("#define _______________ printf\n\0");
    std::string def11("#define __ int\n\0");
    std::string def12("#define ___ void\n\0");
    std::string def13("#define _ main\n\0");
    std::string def14("#define ______ if\n\0");
    std::string def15("#define ____VV____ else\n\0");
    //std::string def16("#define y __________\n\0");
    //std::string def17("#define z ____________\n\0");
    //std::string def18("#define b __________\n\0");
    std::string def19("#define _______________________________ return\n\0");
    std::string def20("#define b_s_c getchar\n\0");
    std::string def21("#define _VAS__ getch\n\0");
    std::string def22("#define _c_i_ scanf\n\0");
    std::string def23("#define v_u char\n\0");
    std::string def24("#define ____r_ while\n\0");

    std::string def25("#define _nox_ ftell\n\0");
    std::string def26("#define bar__ fseek\n\0");
    std::string def27("#define rom memset\n\0");
    std::string def28("#define _c_p_u unsigned\n\0");
    std::string def29("#define d_eg_e_t long\n\0");
    std::string def30("#define d_a_n double\n\0");
    std::string def31("#define _c_a_p memcp\n\0");
    std::string def32("#define _f_a_ do\n\0");
    std::string def33("#define _marz sizeof\n\0");
    std::string def34("#define _t_o_f_ float\n\0");

    std::string def35("#define _yp__ strcpy\n\0");
    std::string def36("#define _e_v___ strcmp\n\0");
    std::string def37("#define ___b_o strtok\n\0");
    std::string def38("#define _v_a_c_i_n_e_ memcpy\n\0");

    std::string def39("#define __kkk___k for\n\0");

    std::string includeString("#include");


    std::cout << "===============================" << std::endl;

    std::vector<std::string> outputVector;
    int lastIndexOfDefine = 0;
    for(int i = 0; i < inputVector.size(); i++) {
        std::string lastIndex((i == 0) ? inputVector[0] : inputVector[i - 1]);
        std::string curLine(inputVector[i]);

        if((lastIndex.find(includeString) != std::string::npos) && 
                (curLine.find(includeString) == std::string::npos)) {
                //std::cout << "WARNING HERE IS THE END OF #INCLUDE " << std::endl;
                outputVector.push_back(def1);
                lastIndexOfDefine++;
                outputVector.push_back(def2);
                lastIndexOfDefine++;
                outputVector.push_back(def7);
                lastIndexOfDefine++;
                outputVector.push_back(def8);
                lastIndexOfDefine++;
                outputVector.push_back(def10);
                lastIndexOfDefine++;
                outputVector.push_back(def11);
                lastIndexOfDefine++;
                outputVector.push_back(def12);
                lastIndexOfDefine++;
                outputVector.push_back(def13);
                lastIndexOfDefine++;
                outputVector.push_back(def14);
                lastIndexOfDefine++;
                outputVector.push_back(def15);
                lastIndexOfDefine++;
                outputVector.push_back(def19);
                lastIndexOfDefine++;
                outputVector.push_back(def20);
                lastIndexOfDefine++;
                outputVector.push_back(def21);
                lastIndexOfDefine++;
                outputVector.push_back(def22);
                lastIndexOfDefine++;
                outputVector.push_back(def23);
                lastIndexOfDefine++;
                outputVector.push_back(def24);
                lastIndexOfDefine++;

                outputVector.push_back(def25);
                lastIndexOfDefine++;
                outputVector.push_back(def26);
                lastIndexOfDefine++;
                outputVector.push_back(def27);
                lastIndexOfDefine++;
                outputVector.push_back(def28);
                lastIndexOfDefine++;
                outputVector.push_back(def29);
                lastIndexOfDefine++;
                outputVector.push_back(def30);
                lastIndexOfDefine++;
                outputVector.push_back(def31);
                lastIndexOfDefine++;
                outputVector.push_back(def32);
                lastIndexOfDefine++;
                outputVector.push_back(def33);
                lastIndexOfDefine++;
                outputVector.push_back(def34);
                lastIndexOfDefine++;
                outputVector.push_back(def35);
                lastIndexOfDefine++;
                outputVector.push_back(def36);
                lastIndexOfDefine++;
                outputVector.push_back(def37);
                lastIndexOfDefine++;
                outputVector.push_back(def38);
                lastIndexOfDefine++;
                outputVector.push_back(def39);
                lastIndexOfDefine++;

                lastIndexOfDefine++;
                outputVector.push_back("\n\0");
                lastIndexOfDefine++;
                
        }

        std::cout << inputVector[i] << std::endl;
        outputVector.push_back(inputVector[i]);
    }

    std::cout << "================================== OUTPUT VECTOR ==================================" << std::endl;

    for(int i = 0; i < outputVector.size(); i++) {
        std::cout << outputVector[i] << std::endl;
    }

    std::vector<std::string> formatedOutputData;

    std::cout << "FORMATING LINE BY LINE AND REPLACING DEF1 { #define true 0 }" << std::endl;
    std::string trueW = "true";
    std::string trueR = "zero";

    std::string falseW = "false";
    std::string falseR = "one";

    std::string ctrlW = "ctrl";
    std::string ctrlR = "el";

    std::string longW = "long";
    std::string longR = "vote";

    std::string printfW = "printf";
    std::string printfR = "_______________";

    std::string intW = "int";
    std::string intR = "__";

    std::string voidW = "void";
    std::string voidR = "___";

    std::string mainW = "main";
    std::string mainR = "_";

    std::string ifW = "if";
    std::string ifR = "______";

    std::string elseW = "else";
    std::string elseR = "____VV____";

    std::string returnW = "return";
    std::string returnR = "_______________________________";

    std::string getCharW = "getchar";
    std::string getCharR = "b_s_c";

    std::string getChW = "getch";
    std::string getChR = "_VAS__";

    std::string getScanfW = "scanf";
    std::string getScanfR = "_c_i_";

    std::string getCW = "char";
    std::string getCR = "v_u";

    std::string getWhileW = "while";
    std::string getWhileR = "____r_";



    std::string getFtellW = "ftell";
    std::string getFtellR = "_nox_";

    std::string getFseekW = "fseek";
    std::string getFseekR = "bar__";

    std::string getUnsignedW = "unsigned";
    std::string getUnsignedR = "_c_p_u";

    std::string getLongW = "long";
    std::string getLongR = "d_eg_e_t";

    std::string getDoubleW = "double";
    std::string getDoubleR = "d_a_n";

    std::string getMemcpW = "memcp";
    std::string getMemcpR = "_c_a_p";

    std::string getDoW = "do";
    std::string getDoR = "_f_a_";

    std::string getSizeofW = "sizeof";
    std::string getSizeofR = "_marz";

    std::string getFloatW = "float";
    std::string getFloatR = "_t_o_f_";
    

    std::string getStrCpyW = "strcpy";
    std::string getStrCpyR = "_yp__";

    std::string getStrCmpW = "strcmp";
    std::string getStrCmpR = "_e_v___";

    std::string getStrTokW = "strtok";
    std::string getStrTokR = "___b_o";

    std::string getMemCpyW = "memcpy";
    std::string getMemCpyR = "_v_a_c_i_n_e_";

    std::string getForW = "for";
    std::string getForR = "__kkk___k";
  
    for(int i = 0; i < outputVector.size(); i++) {
        std::string line(outputVector[i]);
        if(i > lastIndexOfDefine) {
            if(line.find(trueW) != std::string::npos) {
                std::cout << "LINE BEFORE FORMATING: " << line << std::endl;
                line = replaceStr(line, trueW, trueR);
                std::cout << "LINE AFTER FORMATING:" << line << std::endl;
            }
            if(line.find(falseW) != std::string::npos) {
                std::cout << "LINE BEFORE FORMATING: " << line << std::endl;
                line = replaceStr(line, falseW, falseR);
                std::cout << "LINE AFTER FORMATING:" << line << std::endl;
            }
            if(line.find(ctrlW) != std::string::npos) {
                std::cout << "LINE BEFORE FORMATING: " << line << std::endl;
                line = replaceStr(line, ctrlW, ctrlR);
                std::cout << "LINE AFTER FORMATING:" << line << std::endl;
            }
            if(line.find(longW) != std::string::npos) {
                std::cout << "LINE BEFORE FORMATING: " << line << std::endl;
                line = replaceStr(line, longW, longR);
                std::cout << "LINE AFTER FORMATING:" << line << std::endl;
            }
            if(line.find(printfW) != std::string::npos) {
                std::cout << "LINE BEFORE FORMATING: " << line << std::endl;
                line = replaceStr(line, printfW, printfR);
                std::cout << "LINE AFTER FORMATING:" << line << std::endl;
            }
            if(line.find(intW) != std::string::npos) {
                std::cout << "LINE BEFORE FORMATING: " << line << std::endl;
                line = replaceStr(line, intW, intR);
                std::cout << "LINE AFTER FORMATING:" << line << std::endl;
            }
            if(line.find(voidW) != std::string::npos) {
                std::cout << "LINE BEFORE FORMATING: " << line << std::endl;
                line = replaceStr(line, voidW, voidR);
                std::cout << "LINE AFTER FORMATING:" << line << std::endl;
            }
            if(line.find(mainW) != std::string::npos) {
                std::cout << "LINE BEFORE FORMATING: " << line << std::endl;
                line = replaceStr(line, mainW, mainR);
                std::cout << "LINE AFTER FORMATING:" << line << std::endl;
            }
            if(line.find(ifW) != std::string::npos) {
                std::cout << "LINE BEFORE FORMATING: " << line << std::endl;
                line = replaceStr(line, ifW, ifR);
                std::cout << "LINE AFTER FORMATING:" << line << std::endl;
            }
            if(line.find(elseW) != std::string::npos) {
                std::cout << "LINE BEFORE FORMATING: " << line << std::endl;
                line = replaceStr(line, elseW, elseR);
                std::cout << "LINE AFTER FORMATING:" << line << std::endl;
            }
            if(line.find(returnW) != std::string::npos) {
                std::cout << "LINE BEFORE FORMATING: " << line << std::endl;
                line = replaceStr(line, returnW, returnR);
                std::cout << "LINE AFTER FORMATING:" << line << std::endl;
            }
            if(line.find(getCharW) != std::string::npos) {
                std::cout << "LINE BEFORE FORMATING: " << line << std::endl;
                line = replaceStr(line, getCharW, getCharR);
                std::cout << "LINE AFTER FORMATING:" << line << std::endl;
            }
            if(line.find(getChW) != std::string::npos) {
                std::cout << "LINE BEFORE FORMATING: " << line << std::endl;
                line = replaceStr(line, getChW, getChR);
                std::cout << "LINE AFTER FORMATING:" << line << std::endl;
            }
            if(line.find(getScanfW) != std::string::npos) {
                std::cout << "LINE BEFORE FORMATING: " << line << std::endl;
                line = replaceStr(line, getScanfW, getScanfR);
                std::cout << "LINE AFTER FORMATING:" << line << std::endl;
            }
            if(line.find(getCW) != std::string::npos) {
                std::cout << "LINE BEFORE FORMATING: " << line << std::endl;
                line = replaceStr(line, getCW, getCR);
                std::cout << "LINE AFTER FORMATING:" << line << std::endl;
            }
            if(line.find(getWhileW) != std::string::npos) {
                std::cout << "LINE BEFORE FORMATING: " << line << std::endl;
                line = replaceStr(line, getWhileW, getWhileR);
                std::cout << "LINE AFTER FORMATING:" << line << std::endl;
            }
            if(line.find(getFtellW) != std::string::npos) {
                std::cout << "LINE BEFORE FORMATING: " << line << std::endl;
                line = replaceStr(line, getFtellW, getFtellR);
                std::cout << "LINE AFTER FORMATING:" << line << std::endl;
            }
            if(line.find(getFseekW) != std::string::npos) {
                std::cout << "LINE BEFORE FORMATING: " << line << std::endl;
                line = replaceStr(line, getFseekW, getFseekR);
                std::cout << "LINE AFTER FORMATING:" << line << std::endl;
            }
            if(line.find(getUnsignedW) != std::string::npos) {
                std::cout << "LINE BEFORE FORMATING: " << line << std::endl;
                line = replaceStr(line, getUnsignedW, getUnsignedR);
                std::cout << "LINE AFTER FORMATING:" << line << std::endl;
            }
            if(line.find(getLongW) != std::string::npos) {
                std::cout << "LINE BEFORE FORMATING: " << line << std::endl;
                line = replaceStr(line, getLongW, getLongR);
                std::cout << "LINE AFTER FORMATING:" << line << std::endl;
            }
            if(line.find(getDoubleW) != std::string::npos) {
                std::cout << "LINE BEFORE FORMATING: " << line << std::endl;
                line = replaceStr(line, getDoubleW, getDoubleR);
                std::cout << "LINE AFTER FORMATING:" << line << std::endl;
            }
            if(line.find(getMemcpW) != std::string::npos) {
                std::cout << "LINE BEFORE FORMATING: " << line << std::endl;
                line = replaceStr(line, getMemcpW, getMemcpR);
                std::cout << "LINE AFTER FORMATING:" << line << std::endl;
            }
            if(line.find(getDoW) != std::string::npos) {
                std::cout << "LINE BEFORE FORMATING: " << line << std::endl;
                line = replaceStr(line, getDoW, getDoR);
                std::cout << "LINE AFTER FORMATING:" << line << std::endl;
            }
            if(line.find(getSizeofW) != std::string::npos) {
                std::cout << "LINE BEFORE FORMATING: " << line << std::endl;
                line = replaceStr(line, getSizeofW, getSizeofR);
                std::cout << "LINE AFTER FORMATING:" << line << std::endl;
            }
            if(line.find(getFloatW) != std::string::npos) {
                std::cout << "LINE BEFORE FORMATING: " << line << std::endl;
                line = replaceStr(line, getFloatW, getFloatR);
                std::cout << "LINE AFTER FORMATING:" << line << std::endl;
            }
            if(line.find(getStrCpyW) != std::string::npos) {
                std::cout << "LINE BEFORE FORMATING: " << line << std::endl;
                line = replaceStr(line, getStrCpyW, getStrCpyR);
                std::cout << "LINE AFTER FORMATING:" << line << std::endl;
            }
            if(line.find(getStrCmpW) != std::string::npos) {
                std::cout << "LINE BEFORE FORMATING: " << line << std::endl;
                line = replaceStr(line, getStrCmpW, getStrCmpR);
                std::cout << "LINE AFTER FORMATING:" << line << std::endl;
            }
            if(line.find(getStrTokW) != std::string::npos) {
                std::cout << "LINE BEFORE FORMATING: " << line << std::endl;
                line = replaceStr(line, getStrTokW, getStrTokR);
                std::cout << "LINE AFTER FORMATING:" << line << std::endl;
            }
            if(line.find(getMemCpyW) != std::string::npos) {
                std::cout << "LINE BEFORE FORMATING: " << line << std::endl;
                line = replaceStr(line, getMemCpyW, getMemCpyR);
                std::cout << "LINE AFTER FORMATING:" << line << std::endl;
            }
            if(line.find(getForW) != std::string::npos) {
                std::cout << "LINE BEFORE FORMATING: " << line << std::endl;
                line = replaceStr(line, getForW, getForR);
                std::cout << "LINE AFTER FORMATING:" << line << std::endl;
            }
            
        }
        if(line.find("//") == std::string::npos) {
            formatedOutputData.push_back(line);   
        }              

    }



    // ********************** REMOVE LINES AFTER #defines *******************************

    int lastIndexOfDefine2 = 0;
    int lastIndexOfInclude2 = 0;
    std::string defineSearch = "#define";
    std::string includeSearch = "#include";
    for(int i = 0; i < formatedOutputData.size(); i++) {
        std::string line(formatedOutputData[i]);
        if(line.find(defineSearch) != std::string::npos) {
            lastIndexOfDefine2++;
        }
        if(line.find(includeSearch) != std::string::npos) {
            lastIndexOfInclude2++;
        }
    }
    lastIndexOfDefine2 = lastIndexOfDefine2 + lastIndexOfInclude2 + 3;


    std::cout << "*************************************************************" << std::endl;
    std::string randomString = std::to_string(randomNo);



    std::string delimiter = ".";
    std::vector<std::string> fileNameTokens;
    size_t pos = 0;
    std::string token;
    while ((pos = fileName.find(delimiter)) != std::string::npos) {
        token = fileName.substr(0, pos);
        std::cout << token << std::endl;
        fileNameTokens.push_back(token);
        fileName.erase(0, pos + delimiter.length());
    }
    std::cout << "RANDOM STRING: " << randomString << std::endl;
    std::cout << " POS1: " << fileNameTokens[0] << "pos2: " << fileName << std::endl;


    

    std::string outputFileName = fileNameTokens[0] + randomString + "." + fileName;
    std::ofstream outputFile(outputFileName);
    for(int i = 0; i < formatedOutputData.size(); i++) {
        std::cout << formatedOutputData[i] << std::endl;
        if( i < lastIndexOfDefine2) {
            outputFile << formatedOutputData[i] << "\n\0";
        } else {
             outputFile << formatedOutputData[i];
        }
        
    }
    outputFile.close();
}

int main(int argc, char *argv[]) {

    srand((unsigned) time(0));
    int randomNumber = rand();


    std::string fileName = "";
    if(argc > 0) {
        fileName = argv[1];
    }

    std::cout << randomNumber << std::endl;
    long x = generateRandomNo(randomNumber) ;
   

    obfuscateProgram(x, fileName);


    return 0;
}