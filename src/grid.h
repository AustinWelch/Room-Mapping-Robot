#ifndef GRID_H_
#define GRID_H_

#include <BSP.h>

#define LEFT 0
#define CENTER 1
#define RIGHT 2

typedef struct detail_coord {
    uint32_t x;
    uint32_t y;
} coord;

typedef struct detail_robotPos {
    uint32_t direction;
    coord coordinates;
} robotPos;

extern int32_t cos_LUT[360];

robotPos Grid_getRobotPos();
void Grid_setRobotPos(robotPos newPos);
coord Grid_convertDistanceToCoordinate(int32_t distance, uint32_t sensor);
uint32_t Grid_getBlockSize();
void Grid_setBlockSize(uint32_t newSize);

#endif
