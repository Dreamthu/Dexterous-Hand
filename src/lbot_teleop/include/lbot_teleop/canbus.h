#ifdef __linux__
#ifndef CAN_BUS_H
#define CAN_BUS_H

#include <iostream>
#include <iomanip>
#include <sstream>
#include <unistd.h>
#include <chrono>
#include <thread>
#include <queue>
#include <fcntl.h>
#include <mutex>
#include <atomic>
#include <cstring>
#include <cerrno>
#include <vector>    
#include <stdexcept> 
#include <string>  
#include <sys/socket.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <ifaddrs.h>
#include <map>

#define SEND_DEBUG 0

using CanFrame = struct can_frame;

static constexpr timeval RX_TIMEOUT{0, 100000}; // 100 ms

struct CANFrame
{
    uint32_t can_id;
    uint8_t can_dlc;
    uint8_t data[8];
};

struct Job {
    const char* name;
    uint32_t    id;
    bool        is_ext;
    uint8_t     data0;
};

namespace Communication //Communicator
{
    class CanBus
    {
    public:
        CanBus(const std::string device_type);
        CanBus(const std::string interface, const int bitrate);
        ~CanBus();
        
        void send(const std::vector<uint8_t>& data, uint32_t can_id, const bool wait = false);
        CANFrame recv(const uint32_t id = 0xFF);
        
        void setReceiveTimeout(int seconds, int microseconds);
        void updateSendRate();
        void updateReceiveRate();
        std::string printMillisecondTime();

    private:
        int socket_fd;
        std::string interface;
        int bitrate;
        struct sockaddr_can addr;
        struct ifreq ifr;
        std::mutex mutex_send;
        std::mutex mutex_send2;
        std::mutex mutex_receive;

        std::mutex send_mutex;
        std::atomic<int> send_count;
        std::chrono::steady_clock::time_point last_time;

        std::mutex receive_mutex;
        std::atomic<int> receive_count;
        std::chrono::steady_clock::time_point receive_last_time;
        std::queue<struct can_frame> send_queue;
        
        std::vector<std::string> detect_can_devices();
        bool is_interface_up(const std::string& ifname);
        bool ping_once(const std::string& ifname, const Job& job);
    };
}
#endif
#endif // CAN_BUS_H

