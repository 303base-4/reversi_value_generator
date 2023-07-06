#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <sys/wait.h>
#include <unistd.h>
using namespace std;
const int JOB = 4;
void prepare_file(int myid)
{
    stringstream myid_str;
    myid_str << myid;
    system(("mkdir tmp/" + myid_str.str()).c_str());
    system(("cp -pdr mission tmp/" + myid_str.str() + "/").c_str());
    system(("cp player.h tmp/" + myid_str.str() + "/mission/code/").c_str());
}
int main()
{
    srand(time(NULL));
    system("mkdir tmp");
    int pid = 0;
    for (int i = 0; i < JOB; i++)
    {
        pid = fork();
        if (pid == 0)
            break;
    }
    if (pid != 0)
    {
        const double T0 = 100000;
        const double d = 0.97;
        const double Tk = 0.001;
        int now_result = 0;
        int value[4];
        double t = T0;
        int id, result;
        while (t > Tk)
        {
            id = wait(&result);
            pid = fork();
            if (pid == 0)
            {
                break;
            }
            result = -result;
            int dE = result - now_result;
            fstream input;
            stringstream id_str;
            id_str << id;
            input.open("tmp/" + id_str.str() + "/value.result", ios::in);
            if (dE < 0)
            {
                now_result = result;
                for (int i = 0; i < 4; i++)
                {
                    input >> value[i];
                }
                cout << "--------------------------------\n";
                cout << "now_result: " << now_result << "\n";
                cout << "--------------------------------" << endl;
            }
            else if (exp(-dE / t) * RAND_MAX > rand())
            {
                now_result = result;
                for (int i = 0; i < 4; i++)
                {
                    input >> value[i];
                }
                cout << "--------------------------------\n";
                cout << "now_result: " << now_result << "\n";
                cout << "--------------------------------" << endl;
            }
            input.close();
            t *= d;
            system(("rm -r tmp/" + id_str.str()).c_str());
        }
        if (pid != 0)
        {
            for (int i = 0; i < 4; i++)
                cout << value[i];
        }
    }
    if (pid == 0)
    {
        int myid = getpid();
        int score = 0;
        stringstream myid_str;
        myid_str << myid;
        prepare_file(myid);
        chdir(("tmp/" + myid_str.str()).c_str());
        int value[4] = {rand() % 10, rand() % 10, rand() % 10, rand() % 10};
        fstream output;
        output.open("mission/code/value.result", ios::out);
        for (int i = 0; i < 4; i++)
        {
            output << value[i] << " ";
        }
        output.close();
        chdir("mission");
        for (int i = 1; i <= 4; i++)
        {
            int computer, player;
            stringstream i_str;
            i_str << i;
            system(("cp data/map" + i_str.str() + ".txt data/map.txt").c_str());
            system("./run.sh");
            fstream result;
            result.open("result.txt", ios::in | ios::out);
            result >> computer >> player;
            score += player - computer;
            result.close();
            system("mv code/computer.h code/computer.h.bak");
            system("mv code/player.h code/computer.h");
            system("mv code/computer.h.bak code/player.h");
            system("./run.sh");
            result.open("result.txt", ios::in | ios::out);
            result >> computer >> player;
            result.close();
            score += computer - player;
            system("mv code/computer.h code/computer.h.bak");
            system("mv code/player.h code/computer.h");
            system("mv code/computer.h.bak code/player.h");
        }
        chdir("../");
        system("cp mission/code/value.result ./");
        exit(score);
    }
    system("rm -r tmp");
    return 0;
}