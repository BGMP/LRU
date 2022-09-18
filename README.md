LRU
===
Memory administration emulator built in C.

This program emulates memory management implementing the LRU (Least Recently Used) page replacement algorithm. The 
implementation is rather simple and aims to be easy to read and understand, as it is a project I made for an
Operative Systems class, and decided to tidy up and upload here as an academic resource for anybody to peek into.

### General Overview
As stated in the brief summary above, this program is meant to be a resource for people who would like to understand
and experiment with page replacement algorithms. The project scope is rather minimal, and there are limitations to its
structure, yet those limitations are meant to be easily tweakable!

Everything relevant to the program is located at `obj_RAM.h`, which represents the memory in question. There are
a few macros you may modify at will:

```c
#define UNSET_VAL -1      // Represents values pending to be set
#define REQUESTS 20       // Number of requests the memory will be processing
#define FRAMES 4          // Number of page frames
```

And there is the main structure which holds the relevant information all together.

```c
typedef struct RAM {
    FILE *logFile;
    int requests[REQUESTS];
    int contents[FRAMES][REQUESTS];
    int lruMatrix[FRAMES][FRAMES];
    int faults;
} RAM;
```

When executing, the program will both log all the steps to console and to a TXT log file you will find located at
`logs/`. The name of the file will be something like `log_LRU_2022-09-15_16-47-05.txt`.

### Output
Each step of the page assignment process is written to the log file. Below is the last step, or rather the
memory representation after the last process has been computed and assigned to a page.

Value set:
```c
int sample[REQUESTS] = {
        1, 3, 2, 1, 4, 2, 5, 6, 2, 1,
        2, 3, 7, 6, 3, 2, 1, 2, 3, 6
};
```

Output:
```
 RQ |  1   3   2   1   4   2   5   6   2   1   2   3   7   6   3   2   1   2   3   6  | LRU Matrix
----|---------------------------------------------------------------------------------|--------------|-
  0 |  1   1   1   1   1   1   1   6   6   6   6   6   7   7   7   7   1   1   1   1  |  0  0  0  0  |
  1 |      3   3   3   3   3   5   5   5   5   5   3   3   3   3   3   3   3   3   3  |  1  0  1  0  |
  2 |          2   2   2   2   2   2   2   2   2   2   2   2   2   2   2   2   2   2  |  1  0  0  0  |
  3 |                  4   4   4   4   4   1   1   1   1   6   6   6   6   6   6   6  |  1  1  1  0  |
----|---------------------------------------------------------------------------------|--------------|-
11F |
```

At the top, all processes are listed horizontally after RQ (requests). The LRU Matrix is also logged for each step,
updated with the latest data. Underneath RQ are the page frame numbers, and under them the number of faults (11F in
this case). All processes are in the middle, already assigned to their corresponding page in this final step.

### Building

You may build the project using CMake.

```sh
mkdir build && cd build
cmake ..
make
```

The project requires one directory to be present physically within its root folder, along with a couple of DLLs to
function properly:
  - logs/
  - libiconv-2.dll
  - libunistring-2.dll

You may also get a pre-packaged version for Windows 64.

### Running

You may run the `LRU` binary and the program will function normally, just make sure all the required files/directories
are present within the directory you're executing it at.
