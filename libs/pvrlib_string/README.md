# String functions
Creator: Palver  
Date of creation: 30.12.2025  
Standard: C99  

## How to use
Example:  
```c
#include "pvr_string.h" // include prototypes

int main() {return 0;}

#define PVR_STRING_IMPLEMENTATION
#include "pvr_string.h" // include implementations
```

## Documentation
```c
char* PVR_replace(char *str, char char_old, char char_new);
```
Replaces characters in a string.
This function currently doesn't have any options - it replaces all the characters in a string.
