////////////////////////////////////////////////////////////////////////////////
//
// LRU (Generic)
//
// Desc: sys_Util.h
// Utility.
//
// 12/09/2022 (José Benavente)
// File inception.
//
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Macros:
////////////////////////////////////////////////////////////////////////////////

#define CountOf(x) (sizeof((x)) / sizeof((x)[0]))

////////////////////////////////////////////////////////////////////////////////
// Types:
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Prototypes:
////////////////////////////////////////////////////////////////////////////////

bool CheckUtf8(char *str, int size, bool truncate);
bool AsciiToUtf8(char *str, int size);

////////////////////////////////////////////////////////////////////////////////
// Globals:
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Procedures:
////////////////////////////////////////////////////////////////////////////////