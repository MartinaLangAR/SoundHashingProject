#ifndef EXITMANAGER_H
#define EXITMANAGER_H
#include <camera.h>

class ExitManager
{
public:
    camera cam;
    ExitManager(int camera_device_nr);
};

#endif // EXITMANAGER_H
