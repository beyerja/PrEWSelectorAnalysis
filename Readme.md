# PrEWSelectorAnalysis - Test influence of PrEWUtils BinSelector

Code to check influence of cutting bins by their predicted value.

## Installation

0. Make sure you installed `PrEW` and `PrEWUtils` (preferably in a directory next to this one).
1. Make sure your software is up-to-date (ROOT, gcc, cmake, ...) *or* load using macro (only on NAF):
 ```sh
 cd macros && source load_env.sh && cd ..
 ```
2. Compile the code:
 ```sh
 cd macros && chmod u+x compile.sh && ./compile.sh && cd ..
 ```
 
### After changing the source code

... simply recompile it using the `macros/compile.sh` macro.

## Running

The code can be easily run after compilation:
```sh
cd bin && ./PrEWSelectorAnalysis && cd ..
```
This will produce an output file in the `output` directory.
It is a plain text file that stores some information about the cuts.

### The source code

The source code is in `source/main.cpp`.

It uses the necessary `PrEW` classes and the interfaces provided by `PrEWUtils`.

For open questions please consult the `PrEW` and `PrEWUtils` source code or open an issue on the GitHub page.

## Interpreting the output

A jupyter (python) notebook is provided to interpret the output.
It sits in the `notebook` directory and can be opened using
```sh
cd notebook && jupyter notebook ResultNotebook.ipynb
```
and will produce histograms in the `output` directory.

The notebook can also be run without opening a browser window using
```sh
cd notebook && jupyter nbconvert --to notebook --inplace --execute ResultNotebook.ipynb
```
Text outputs (which would normally be visible in the browser window) can be found in the `ResultNotebook.ipynb` file.