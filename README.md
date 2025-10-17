# Metaheuristics for Maximum Independent Set Problem

Instrucciones de uso:

$ git clone https://github.com/Keyteer/metaheuristica_MISP.git
$ cd metaheuristica_MISP
$ git submodule update --remote
$ g++ main_ils.cpp -O2
$ ./a.out  -i dataset_grafos_no_dirigidos/new_3000_dataset/ -t 10 -p 6


Usage: ./a.out -i <path> [-t <time_limit_seconds>] [-p <perturbation>]

Parameters:
  -i <path>                    : Path to the graph instance/s file/directory (required)
  -t <time_limit_seconds>      : Maximum execution time in seconds (default: 10.00)
  -p <perturbation_factor>     : Fraction of nodes to remove in perturbation step (default: 20)