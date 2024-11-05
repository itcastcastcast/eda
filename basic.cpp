#include "common.h"
double ManhattonDistance(double x1,double y1,double x2,double y2,double xsize1,double xsize2,double ysize1,double ysize2)
{
    return fabs(x1+xsize1/2-x2-xsize2/2)+fabs(y1+ysize1/2-y2-ysize2/2);
}
double rc(double x1,double y1,double x2,double y2,double xsize1,double xsize2,double ysize1,double ysize2)
{
    return 0.5*constraint.net_unit_r*constraint.net_unit_c*ManhattonDistance(x1,y1,x2,y2,xsize1,xsize2,ysize1,ysize2);
}
Node* initBuffer(double x, double y){
    DriverNode* buffer=new DriverNode();
    buffer->x = x;
    buffer->y = y;
    buffer->width = bufSize.width;
    buffer->height = bufSize.height;
    buffer->type = "BUF";
    buffer->name = "BUF";
    buffer->fanout = 0;
    buffer->rc = 0.0;
    buffer->parent = nullptr;
    buffer->children={};
    buffer->delay=0.0;
    buffer->cluster_id=-1;//不需要用
    buffer->block_x=static_cast<int>((x - dieArea.xmin) / dieArea.block_width);
    buffer->block_y=static_cast<int>((y - dieArea.ymin) / dieArea.block_height);
    return (Node*)buffer;
}