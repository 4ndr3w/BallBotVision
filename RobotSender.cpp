#include <arpa/inet.h>
#include "RobotSender.h"
#include "EndianHelper.h"

RobotSender::RobotSender() {
    sock = socket(AF_INET, SOCK_DGRAM, 0);

    sockaddr_in bindaddr;
    memset(&bindaddr, 0, sizeof(sockaddr_in));
    bindaddr.sin_family = AF_INET;
    bindaddr.sin_port = htons(8081);
    bindaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    assert(bind(sock, (sockaddr*)&bindaddr, sizeof(sockaddr_in)) != -1);


    memset(&robot, 0, sizeof(sockaddr_in));
    robot.sin_family = AF_INET;
    robot.sin_port = htons(8081);
    robot.sin_addr.s_addr = inet_addr("10.2.25.2");
}

void RobotSender::sendAngle(double ts, double angle) {
    VisionMessage msg;
    msg.ts = byteswap(ts);
    msg.theta = byteswap(angle);

    sendto(sock, &msg, sizeof(VisionMessage), 0, (sockaddr*)&robot, sizeof(sockaddr_in));
}
