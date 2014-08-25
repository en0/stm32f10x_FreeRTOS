#ifndef _MAX1704X_H
#define _MAX1704X_H 1

#include <stdint.h>

int max1704x_VoltageRefresh(uint32_t minor, int flags);
int max1704x_PercRefresh(uint32_t minor, int flags);
int max1704x_AlertRefresh(uint32_t minor, int flags);
int max1704x_AlertCommit(uint32_t minor);


#endif /** _MAX1704X_H **/

