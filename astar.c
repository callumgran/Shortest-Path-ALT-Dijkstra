#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

/* 
    PSEUDOCODE
    Priority = 
    Dist from start +
    Estimated dist to target +

    Estimated =
    Largest of:
    (Dist from first landmark to target - Dist from landmark to node) and (Dist from target to first landmark - Dist from node to landmark);
    (Dist from second landmark to target - Dist from second landmark to node) and (Dist from target to second landmark - Dist from node to second landmark);    
    (Dist from third landmark to target - Dist from third landmark to node) and (Dist from target to third landmark - Dist from node to third landmark);
*/

