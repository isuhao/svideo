#include "my_utils.h"

#include "my_utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stddef.h>
#include <unistd.h>

char szLevel[2][10] = 
{    
	" ERR    ",
	" INFO   "
};

char szLevelEx[] = "NotImportant";

char* GetLevelStr(int iLevel)
{    
	if (iLevel >= 8)
		return szLevelEx;
	else        
		return szLevel[iLevel];
}
