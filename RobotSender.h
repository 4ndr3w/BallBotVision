#ifndef ROBOTSENDER_H
#define ROBOTSENDER_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <assert.h>
#include <string.h>

struct VisionMessage {
    double ts;
    double theta;
};


class RobotSender {
    int sock;
    sockaddr_in robot;
public:
    RobotSender();
    void sendAngle(double ts, double angle);
    void close();
};

#endif