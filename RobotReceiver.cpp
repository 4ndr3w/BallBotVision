//
// Created by Andrew on 6/1/16.
//

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <arpa/inet.h>
#include <cstdio>
#include <sys/time.h>
#include "RobotReceiver.h"
#include "EndianHelper.h"

double getRobotTime(TimeSnapshot *snap) {
    struct timeval tp;
    gettimeofday(&tp, NULL);
    long int timeSinceInitI = (tp.tv_sec * 1000 + tp.tv_usec / 1000)-snap->localNow;
    double timeSinceInit = ((double)timeSinceInitI)/1000.0;
    return timeSinceInit+snap->robotNow;
}


TimeSnapshot getTimestampNow() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in addr;
    memset(&addr, 0, sizeof(sockaddr_in));

    TimeSnapshot snap;
    struct timeval tp;

    addr.sin_addr.s_addr = inet_addr("10.2.25.2");
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);


    if ( connect(sock, (sockaddr*)&addr, sizeof(sockaddr_in)) == -1 ) {
        perror("Failed to connect");
    }

    RobotStateMessage tmp;
    if ( recv(sock, &tmp, sizeof(RobotStateMessage), 0) == -1 ) {
        perror("recv Failed");
    }
    gettimeofday(&tp, NULL);

    long int ms = tp.tv_sec * 1000 + tp.tv_usec / 1000;

    snap.robotNow = byteswap(tmp.ts);
    snap.localNow = ms;
    close(sock);
    return snap;
}


void* createRecvThread(void* ptr) {
    ((RobotReceiver*)ptr)->handle();
}


RobotReceiver::RobotReceiver(const char *ip) {
    sock = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in addr;
    memset(&addr, 0, sizeof(sockaddr_in));

    addr.sin_addr.s_addr = inet_addr(ip);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);

    if ( connect(sock, (sockaddr*)&addr, sizeof(sockaddr_in)) == -1 ) {
        perror("Failed to connect");
    }
    else {
        pthread_mutex_init(&recvMutex, NULL);
        pthread_create(&recvThread, NULL, createRecvThread, this);
    }
}

double RobotReceiver::getAngle() {
    double tmp;
    pthread_mutex_lock(&recvMutex);
    tmp = current.yaw;
    pthread_mutex_unlock(&recvMutex);
    return tmp;
}

void RobotReceiver::handle() {
    while ( true ) {
        RobotStateMessage tmp;
        if ( recv(sock, &tmp, sizeof(RobotStateMessage), 0) == -1 ) {
            return;
        }

        tmp.ts = byteswap(tmp.ts);
        tmp.yaw = byteswap(tmp.yaw);

        pthread_mutex_lock(&recvMutex);
        memcpy(&current, &tmp, sizeof(RobotStateMessage));
        pthread_mutex_unlock(&recvMutex);

    }
}


void RobotReceiver::wait() {
    pthread_join(recvThread, NULL);
}