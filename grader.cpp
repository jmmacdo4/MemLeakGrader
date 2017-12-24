#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

// Usage
//./grader executable input1.txt input2.txt input3.txt ... inputn.txt

#define LEAK_DEDUCTION 3.0
#define GARBAGE_LINES 9

// Each input file will have it's own LeakInfo
typedef struct LeakInfo {
    // the number of bytes taken by the memory leak
    int bytes;

    // number of blocks taken by the memory leak
    int blocks;

    // the points taken off for this leak
    double deductions;
} Leak;

int main(int argc, char* argv[])
{
    // We create the valgrind command to run each time
    // and attach the executable name to the end
    string command = "valgrind --log-file=valout.txt --tool=memcheck --leak-check=full ./";
    command.append(argv[1]);

    // Our list of Leaks so we can check for duplicates. There will be one
    // for each input file entered on the command line.
    vector<Leak*> report;

    // Purely for effeciency purposes, if a student has no leaks,
    // we can short circuit the program
    bool hasLeak = false;

    for (int i = 2; i < argc; i++) {
        string temp = command;
        temp.append(" ");
        temp.append(argv[i]);
        temp.append(" >| output.txt 2> error.txt");

        //cout << temp << endl;

        system( temp.c_str() );

        fstream stream;
        stream.open("valout.txt");

        char str[100];
        for (int i = 0; i < GARBAGE_LINES; i++) {
            stream.getline(str, 100);
        }

        temp = str;
        int index = -1;
        while ((index = temp.find(',', 0)) != -1) {
            temp.erase(index, 1);
        }

        Leak* leak = new Leak;
        leak->bytes = -1;
        leak->blocks = -1;
        sscanf(temp.c_str(), "%*s      %*s %*s %*s %*s %d %*s %*s %d", &leak->bytes,
            &leak->blocks);
        if (leak->bytes == 0 && leak->blocks == 0) {
            leak->deductions = 0;
        } else {
            leak->deductions = LEAK_DEDUCTION;
            hasLeak = true;
        }
        report.push_back(leak);

        stream.close();
    }

    // Now we look at each leak and see if it is a duplicate
    // If the students didn't have any leaks, this part is skipped
    if (hasLeak) {
        for (int i = 1; i < report.size(); i++) {
            Leak* leak = report[i];
            for (int j = 0; j < i; j++) {
                Leak* temp = report[j];
                if (leak->bytes == temp->bytes && leak->blocks == temp->blocks) {
                    // It is most likely a duplicate leak
                    leak->deductions = LEAK_DEDUCTION / 2;
                    break;
                }
            }
        }
    }

    // Print out how many points in deductions for each test
    for ( int i = 0; i < report.size(); i++ ) {
        Leak *leak = report.at(i);
        cout << argv[i + 2] << " deductions: " << leak->deductions << endl;
        delete leak;
    }
    
}