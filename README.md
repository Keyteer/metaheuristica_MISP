# Metaheuristics for Maximum Independent Set Problem

Instrucciones de uso:

### Clonar
$ git clone https://github.com/Keyteer/metaheuristica_MISP.git
$ cd metaheuristica_MISP
$ git submodule update --remote


### Compilar
$ g++ main_ils.cpp -O2 -o ILS


### Ejecutar
Usage: ./ILS -i <path> [-t <time_limit_seconds>] [-p <perturbation>]

Parameters:
  -i <path>                    : Path to the graph instance/s file/directory (required)
  -t <time_limit_seconds>      : Maximum execution time in seconds (default: 10.00)
  -p <perturbation_factor>     : Fraction of nodes to remove in perturbation step (default: 20)


Al pasar un directorio, se buscan y se filtran todos los archivos con el formato de nombre especifico del dataset.
Por pantalla se imprime estadisticas promediadas para cada densidad en [0.1, 0.2 ,,,, 0.9] en formato CSV.

Al pasar un solo archivo se imprime por pantalla la ejecución AnyTime Behavior de ILS y cada mejora encontrada.