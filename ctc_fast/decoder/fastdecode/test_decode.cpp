#include "decoder.h"
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <boost/timer/timer.hpp>
#include <boost/algorithm/string.hpp>
// Mainly for profiling purposes

void decode(double *pData, int beam, int N, int T, PrefixTree &pTree, std::ofstream &output)
{
    PrefixKey best_prefix;
    double alpha = 1.0;
    double beta = 0.0;
    boost::timer::cpu_timer timer;
    double best_score = decode_bg_lm(pData, N, T, pTree, beam, alpha, beta, best_prefix);
    boost::timer::cpu_times elapsed = timer.elapsed();
    std::cout << "decoding time (wall): " << elapsed.wall / 1e9 << std::endl;

    for (int k = 0; k < best_prefix.size(); k++)
    {
        std::cout << best_prefix[k] << " ";
        output << pTree.lm.int2char[best_prefix[k]]<< " ";
    }
    std::cout << std::endl;
    output<<std::endl;
    return ;
}
int main(int argc, char** argv)
{
    int N = 3668;
    int T = 475;
    //int N = 10;
    //int T = 19;
    FILE *fp = NULL;
    fp = fopen("/home/guangzhilei/stanford-ctc-master/exp/data.bin", "rb");
    fread(&N, sizeof(int), 1, fp);

    // TODO FIXME PARAM Import
    std::string CHARMAP_PATH = "/home/guangzhilei/stanford-ctc-master/exp/hkust/lm";
    PrefixTree ptree(CHARMAP_PATH + "/chars.txt",
                     CHARMAP_PATH + "/wordlist",
                     CHARMAP_PATH + "/lm_bg.arpa");

    // Decode!
    int beam = atoi(argv[1]);
    int count = 0;
    std::ofstream output("output");
    while (!feof(fp))
    {
        int n = fread(&T, sizeof(int), 1, fp);
        if (n == 0)
        {
            break;
        }
        double *data = new double[N*T];
        memset(data, 0, sizeof(double)*N*T);
        fread(data, sizeof(double), N*T, fp);
        decode(data, beam, N, T, ptree, output);
        count++;
        delete [] data;
    }
    std::cout<<"process:"<<count<<std::endl;
    fclose(fp);
    output.close();
    return 0;
}
