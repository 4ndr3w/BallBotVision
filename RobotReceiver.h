//
// Created by Andrew on 6/1/16.
//

#ifndef BALLFINDER_ROBOTRECEIVER_H
#define BALLFINDER_ROBOTRECEIVER_H

#include "stdint.h"
#include "pthread.h"



struct TimeSnapshot {
    double robotNow;
    long int localNow;
};

TimeSnapshot getTimestampNow();
double getRobotTime(TimeSnapshot *snap);

struct PIDSnapshot {
    float error;
    float setpoint;
    float input;

    float p;
    float i;
    float d;
    float f;

    float out;
};


struct RobotStateMessage {
    double ts;
    PIDSnapshot shooter;
    int16_t intakeState;
    double yaw;
};


class RobotReceiver {
    int sock;
    pthread_t recvThread;
    pthread_mutex_t recvMutex;
    RobotStateMessage current;
public:
    RobotReceiver(const char* ip);
    double getAngle();
    void handle();
    void wait();
};


#endif //BALLFINDER_ROBOTRECEIVER_H
