#include <cassert>
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
const double T0 = 100;
const double d = 0.993;
const double Tk = 1e-10;
const int INF = 0x3f3f3f3f;
int now_score = 0;
int now_value[4] = {0, 0, 0, 0};

void prepare_dir(int i, int next_value[])
{
    string i_str = to_string(i);
    system(("mkdir tmp/" + i_str).c_str());
    system(("cp -pdr mission tmp/" + i_str).c_str());
    system(("cp player.h tmp/" + i_str + "/mission/code/").c_str());
    system(("cp tmp/" + i_str + "/mission/data/map" + i_str + ".txt tmp/" + i_str + "/mission/data/map.txt").c_str());
    string vfile = "tmp/" + i_str + "/mission/value.tmp";
    fstream voutput;
    voutput.open(vfile, ios::out);
    for (int j = 0; j < 4; j++)
    {
        voutput << next_value[j] << " ";
    }
    voutput.close();
}
void turn_swap(int i)
{
    string code_path = "tmp/" + to_string(i) + "/mission/code/";
    system(("mv " + code_path + "computer.h " + code_path + "computer.h.bak").c_str());
    system(("mv " + code_path + "player.h " + code_path + "computer.h").c_str());
    system(("mv " + code_path + "computer.h.bak " + code_path + "player.h").c_str());
}
int test(int next_value[])
{
    int score = 0;
    int pid = 0;
    for (int i = 1; i <= 4; i++)
    {
        prepare_dir(i, next_value);
        string i_str = to_string(i);
        chdir(("tmp/" + i_str + "/mission").c_str());
        pid = fork();
        if (pid == 0)
        {
            system("./run.sh >> /dev/null");
            exit(0);
        }
        chdir("../../../");
    }
    assert(pid != 0);
    int status;
    while ((pid = wait(&status)) > 0)
        ;
    for (int i = 1; i <= 4; i++)
    {
        string i_str = to_string(i);
        fstream resultfile;
        resultfile.open("tmp/" + i_str + "/mission/result.txt", ios::in);
        int computer, player;
        resultfile >> computer >> player;
        score += player - computer;
        resultfile.close();
        turn_swap(i);
        chdir(("tmp/" + i_str + "/mission").c_str());
        pid = fork();
        if (pid == 0)
        {
            system("./run.sh >> /dev/null");
            exit(0);
        }
        chdir("../../../");
    }
    assert(pid != 0);
    while ((pid = wait(&status)) > 0)
        ;
    for (int i = 1; i <= 4; i++)
    {
        string i_str = to_string(i);
        fstream resultfile;
        resultfile.open("tmp/" + i_str + "/mission/result.txt", ios::in);
        int computer, player;
        resultfile >> computer >> player;
        score += computer - player;
        resultfile.close();
        system(("rm -r tmp/" + i_str).c_str());
    }
    return score;
}

int main()
{
    srand(time(NULL));
    system("rm -r tmp");
    system("mkdir tmp");
    double t = T0;
    while (t > Tk)
    {
        int next_value[4];
        for (int j = 0; j < 4; j++)
        {
            long long nv = now_value[j] + (rand() * 2 - RAND_MAX) * t;
            while (nv > INF)
            {
                nv = now_value[j] + (rand() * 2 - RAND_MAX) * t;
            }
            next_value[j] = nv;
        }
        int next_score = -test(next_value);
        int dE = next_score - now_score;
        if (dE < 0)
        {
            now_score = next_score;
            memcpy(now_value, next_value, 4 * sizeof(int));
        }
        else if (exp(-dE / t) * RAND_MAX > rand())
        {
            now_score = next_score;
            memcpy(now_value, next_value, 4 * sizeof(int));
        }
        cout << "---------------------------\n";
        cout << "当前分数：" << now_score << " 候选分数:" << next_score << " 当前温度：" << t << "\n";
        cout << "当前权值:";
        for (int i = 0; i < 4; i++)
        {
            cout << now_value[i] << " ";
        }
        cout << "\n";
        cout << "---------------------------" << endl;
        t *= d;
    }
    system("rm -r tmp");
}