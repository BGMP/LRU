////////////////////////////////////////////////////////////////////////////////
//
// LRU (Generic)
//
// Desc: obj_RAM.c
// RAM.
//
// 27/08/2022 (José Benavente & Claudia Villagrán)
// File inception.
//
////////////////////////////////////////////////////////////////////////////////

#include "obj_RAM.h"
#include "sys_Util.h"

////////////////////////////////////////////////////////////////////////////////
// Macros:
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Types:
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Prototypes:
////////////////////////////////////////////////////////////////////////////////

void InitRAM(RAM *ram);
void FreeRAM(RAM *ram);
void ProcessRAMValues(RAM *ram, int v[REQUESTS]);
void ProcessRAMValue(RAM *ram, int v);
void RenderRAM(RAM *ram);
void UpdateRAMLRU(RAM *ram, int page);
int GetLRUPage(RAM *ram);
void WriteLogEntry(RAM *ram, const char *str, ...);

////////////////////////////////////////////////////////////////////////////////
// Globals:
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Procedures:
////////////////////////////////////////////////////////////////////////////////

///
/// initialise the RAM
///

void InitRAM(RAM * ram) {

  // Initialise the log file where all operations will be written to

  // Prepare a file name with a timestamp...
  char filename[256];
  struct tm *tm;
  time_t now = time(NULL);
  tm = gmtime(&now);
  strftime(filename, sizeof(filename), "logs/log_LRU_%Y-%m-%d_%H-%M-%S.txt", tm);

  // Create the log file and use the filename we have prepared
  FILE * logFile = fopen(filename, "w");
  if (logFile == NULL) {
    printf("Error creating log file!\n");
    exit(1);
  }

  ram->logFile = logFile;

  // Add a file header and some basic info
  WriteLogEntry(ram, "Memory Administration Emulator Log File\n");
  WriteLogEntry(ram, "LRU Page Replacement Policy -- Implementation by BGM\n\n");

  char str_date[256];
  strftime(str_date, sizeof(str_date), "%Y-%m-%d %H:%M:%S", tm);
  WriteLogEntry(ram, "Starting at %s\n", str_date);
  WriteLogEntry(ram, "Encoding: %s\n\n", "UTF-8");

  // All initial values for requests are -1 (UNSET_VAL)
  for (int i = 0; i < CountOf(ram->requests); i++) {
    ram->requests[i] = UNSET_VAL;
  }

  // All initial values for contents are -1 (UNSET_VAL)
  for (int i = 0; i < CountOf(ram->contents); i++) {
    for (int j = 0; j < CountOf(ram->contents[0]); j++) {
      ram->contents[i][j] = UNSET_VAL;
    }
  }

  // All initial values for lruMatrix are zero
  for (int i = 0; i < CountOf(ram->lruMatrix); i++) {
    for (int j = 0; j < CountOf(ram->lruMatrix[0]); j++) {
      ram->lruMatrix[i][j] = 0;
    }
  }

  // Set faults to zero
  ram->faults = 0;
}

///
/// free the RAM
///

void FreeRAM(RAM * ram) {
  // We add a footer with some basic info
  struct tm *tm;
  time_t now = time(NULL);
  tm = gmtime(&now);
  char str_date[256];
  strftime(str_date, sizeof(str_date), "%Y-%m-%d %H:%M:%S", tm);

  WriteLogEntry(ram, "Finished at %s\n", str_date);

  // After that just close the file & free
  fclose(ram->logFile);
  free(ram);
}

///
/// queue multiple values into the RAM
///

void ProcessRAMValues(RAM * ram, int values[REQUESTS]) {
  for (int i = 0; i < REQUESTS; i++) {
    ram->requests[i] = values[i];
  }

  for (int i = 0; i < CountOf(ram->requests); i++) {
    ProcessRAMValue(ram, ram->requests[i]);
  }
}

///
/// queue a value into this RAM
///

void ProcessRAMValue(RAM * ram, int v) {
  for (int i = 0; i < CountOf(ram->contents); i++) {
    for (int j = 0; j < CountOf(ram->contents[0]); j++) {

      // If the value has already been set, we continue looking for one which has not.
      if (ram->contents[i][j] != UNSET_VAL) {
        continue;
      }

      // If we are in the top left corner of contents[][] and a value at this position has not yet been set,
      // then we assign the queued value 'v' and return immediately.
      if (j == 0 && ram->contents[i][j] == UNSET_VAL) {
        ram->contents[i][j] = v;
        ram->faults++;

        UpdateRAMLRU(ram, i);
        RenderRAM(ram);
        return;
      }

      int page = 0;   // If the queued value 'v' is found at the left column, then this is the page of said value.
      int row = i;    // Copy of 'i', so that we can alter it.
      int fault = 1;  // We assume a fault will be granted.
      while (ram->contents[row][j - 1] != UNSET_VAL) {
        if (row >= FRAMES) break;

        // If the queued value 'v' is found at the left column, then there is no fault, and we simply update the RAM's
        // LRU matrix.
        if (ram->contents[row][j - 1] == v) {
          UpdateRAMLRU(ram, page);
          fault = 0;
          break;
        }

        row++;
        page++;
      }

      // We copy over the values from the left column to this column
      while (ram->contents[i][j - 1] != UNSET_VAL) {
        if (i >= FRAMES) break;

        ram->contents[i][j] = ram->contents[i][j - 1];
        i++;
      }

      // If there is a fault, then we must replace the value at the LRU page.
      if (fault) {
        // Look for the LRU page, and set the value there.
        int lru = GetLRUPage(ram);
        ram->contents[lru][j] = v;

        // A fault is granted.
        ram->faults++;

        // We update the LRU page.
        UpdateRAMLRU(ram, lru);
      }

      RenderRAM(ram);

      return;
    }
  }
}

///
/// gets the LRU page using this RAM's LRU matrix.
///

int GetLRUPage(RAM * ram) {

  // §NOTE: The underlying logic to determine the Least Recently Used (LRU) page is rather simple. Here's how it works:
  // When a value is queued to the RAM, the LRU matrix is updated with the page into which said value was set to.
  // In order to get the LRU page based on this matrix, we add all the numbers in each row. The number of the row
  // with the lowest result corresponds to the LRU page.

  int lowest = -1;
  int lowestSum = 0;

  for (int i = 0; i < CountOf(ram->lruMatrix); i++) {
    int sum = 0;
    for (int j = 0; j < CountOf(ram->lruMatrix[0]); j++) {
      sum += ram->lruMatrix[i][j];
    }

    if (lowest == -1 || sum < lowestSum) {
      lowest = i;
      lowestSum = sum;
    }
  }

  return lowest;
}

///
/// updates this RAM's LRU matrix with a given page
///

void UpdateRAMLRU(RAM * ram, int page) {

  // §NOTE: The update process works as follows. When a value is queued to the RAM, the number of the page into which
  // said value was set to is used to update the LRU matrix:
  // - All values at row index 'page' are set to 1.
  // - All values at column index 'page' are set to 0.

  for (int i = 0; i < FRAMES; i++) {
    ram->lruMatrix[page][i] = 1;
    ram->lruMatrix[i][page] = 0;
  }
}

//
// renders a visual representation of the RAM
//

void RenderRAM(RAM * ram) {
  // Top left header (requests)
  WriteLogEntry(ram, " RQ |");

  // All values processed by the RAM
  for (int j = 0; j < REQUESTS; j++) WriteLogEntry(ram, "%3i ", ram->requests[j]);

  // LRU Matrix header
  WriteLogEntry(ram, " | LRU Matrix\n");

  // Top margin for contents
  WriteLogEntry(ram, "----|");

  // Top margin for the LRU Matrix
  for (int i = 0; i < CountOf(ram->contents[0]); i++) WriteLogEntry(ram, "----");
  WriteLogEntry(ram, "-|-");

  for (int i = 0; i < FRAMES; i++) WriteLogEntry(ram, "---");
  WriteLogEntry(ram, "-|-\n");

  // RAM contents
  for (int i = 0; i < CountOf(ram->contents); i++) {
    WriteLogEntry(ram, "%3i |", i);

    // Each value in the RAM
    for (int j = 0; j < CountOf(ram->contents[0]); j++) {
      int v = ram->contents[i][j];
      if (v == UNSET_VAL) {
        WriteLogEntry(ram, "    ");
      } else {
        WriteLogEntry(ram, "%3i ", v);
      }
    }

    WriteLogEntry(ram, " |");

    // LRU Matrix
    for (int k = 0; k < CountOf(ram->lruMatrix); k++) WriteLogEntry(ram, "%3i", ram->lruMatrix[i][k]);
    WriteLogEntry(ram, "  |\n");
  }

  // Bottom margin
  WriteLogEntry(ram, "----|");
  for (int i = 0; i < CountOf(ram->contents[0]); i++) WriteLogEntry(ram, "----");
  WriteLogEntry(ram, "-|-");

  for (int i = 0; i < FRAMES; i++) WriteLogEntry(ram, "---");
  WriteLogEntry(ram, "-|-\n");

  // Number of faults in the bottom left corner
  WriteLogEntry(ram, "%2iF |\n\n", ram->faults);
}

//
// writes to this RAM's log file, and prints to console at the same time
//

void WriteLogEntry(RAM *ram, const char *str, ...) {
  char buf[2048];
  va_list arglist;

  va_start(arglist, str);
  vsnprintf(buf, 2048, str, arglist);
  va_end(arglist);

  AsciiToUtf8(buf, 2048);

  printf("%s", buf);
  fprintf(ram->logFile, "%s", buf);
}
