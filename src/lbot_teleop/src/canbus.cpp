
#ifdef __linux__
#include "canbus.h"

namespace Communication
{
    CanBus::CanBus(const std::string interface,const int bitrate) : interface(interface), bitrate(bitrate)
    {
    	#if 0
        socket_fd = socket(PF_CAN, SOCK_RAW, CAN_RAW);
        if (socket_fd < 0)
        {
            throw std::runtime_error("Failed to create CAN socket");
        }

        memset(&ifr, 0, sizeof(ifr));
        strncpy(ifr.ifr_name, interface.c_str(), IFNAMSIZ - 1);
        if (ioctl(socket_fd, SIOCGIFINDEX, &ifr) < 0)
        {
            throw std::runtime_error("Failed to get CAN interface index");
        }

        addr.can_family = AF_CAN;
        addr.can_ifindex = ifr.ifr_ifindex;

        if (bind(socket_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0)
        {
            throw std::runtime_error("Failed to bind CAN socket");
        }

        std::string command = "sudo ip link set " + interface + " up type can bitrate " + std::to_string(bitrate);
        
        std::cout << "command : " << command << std::endl;
        
        if (system(command.c_str()) != 0)
        {
            throw std::runtime_error("Failed to set CAN bitrate");
        }
        #endif
        
        socket_fd = socket(PF_CAN, SOCK_RAW, CAN_RAW); // 打开套接字
        
        struct ifreq ifr = {};
        strcpy(ifr.ifr_name, interface.c_str());            // 绑定 socket 到 can0 接口
        ioctl(socket_fd, SIOCGIFINDEX, &ifr); // 获取接口索引

        // 设置 CAN 接口的波特率, 替代命令行命令 sudo ip link set can0 up type can bitrate 1000000
        // struct can_bittiming can_bt;
        // can_bt.bitrate = 1000000;
        // struct ifreq ifr_bt = {0};
        // strcpy(ifr_bt.ifr_name, "can0");
        // ifr_bt.ifr_ifru.ifru_data = static_cast<void*>(&can_bt);
        // if (ioctl(socket_fd, SIOCSCANBITTIMING, &ifr_bt) < 0) // 使接口"up"
        // {
        //     std::cerr << "Error setting CAN bitrate: " << strerror(errno) << std::endl;
        //     close(socket_fd);
        //     return false;
        // }
        
        int result;// = system(std::string("sudo ip link set "+ interface +" down").c_str());

        result = system(std::string("sudo ip link set " + interface + " up type can bitrate " + std::to_string(bitrate)).c_str());
        if (result == 0) {
            std::cout << "\033[1;32m" << interface << " interface configured successfully.\033[0m" << std::endl;
        } else {
            std::cerr << "Failed to configure CAN interface." << std::endl;
        }

        struct sockaddr_can addr;
        addr.can_family = AF_CAN;
        addr.can_ifindex = ifr.ifr_ifindex;
        if (bind(socket_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
        {
            std::cerr << "Error in socket bind." << std::endl;
            close(socket_fd);
        }
        setReceiveTimeout(0, 500000);
    }
    
    CanBus::CanBus(const std::string device_type) 
    {
        std::vector<std::string> buses;
        
        buses = detect_can_devices();
        if (buses.empty()) {
            throw std::runtime_error("No CAN interface detected, please load the driver or create a VCAN first！\n");
        }
        
        // printf("Automatically discovered CAN interface：");
        for (auto& b : buses) {
            if (!is_interface_up(b)) {
                system(std::string("sudo ip link set " + b + " up type can bitrate 1000000").c_str());
                system(std::string("sudo chmod 0666 /sys/class/net/" + b + "/tx_queue_len").c_str());
                // system(std::string("echo 1024 > /sys/class/net/" + b + "/tx_queue_len").c_str());
            }
            // printf(" %s", b.c_str());
        }
        // printf("\n");
        
        const std::vector<Job> jobs = {
            {"master_arm",  0x64, false, 0x00}
        };
        std::map<std::string, std::string> hit;

        for (auto& bus : buses) {
            for (auto& j : jobs) {
                if (ping_once(bus, j)) {
                    // printf("%-4s : %s\n", j.name, bus.c_str());
                    hit[j.name] = bus;
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        }
        
        auto it = hit.find(device_type);
        if (it == hit.end()) throw std::runtime_error("No available devices !\n");
        interface = it->second;
   
        socket_fd = socket(PF_CAN, SOCK_RAW, CAN_RAW);
        
        struct ifreq ifr = {};
        strcpy(ifr.ifr_name, interface.c_str());
        ioctl(socket_fd, SIOCGIFINDEX, &ifr);
        
        struct sockaddr_can addr;
        addr.can_family = AF_CAN;
        addr.can_ifindex = ifr.ifr_ifindex;
        if (bind(socket_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
            std::cerr << "Error in socket bind." << std::endl;
            close(socket_fd);
        }
        setReceiveTimeout(0, 500000);
    }

    CanBus::~CanBus()
    {
        // int result = system(std::string("sudo ip link set " + interface + " down").c_str());
    
        close(socket_fd);
    }

    std::string CanBus::printMillisecondTime() {
        auto now = std::chrono::high_resolution_clock::now();
    
        std::time_t time = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    
        auto duration = now.time_since_epoch();
        auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(duration % std::chrono::seconds(1)).count();
        int milliseconds = microseconds / 1000;
        ss << "." << std::setfill('0') << std::setw(3) << milliseconds;
        // ss << "." << std::setfill('0') << std::setw(6) << microseconds;

        // std::cout << "send time: " << ss.str() << std::endl;
        return ss.str();
    }

    void CanBus::send(const std::vector<uint8_t>& data, uint32_t can_id, const bool)
    {
        std::unique_lock<std::mutex> lock(mutex_send);

        if (SEND_DEBUG) {
            static std::string hand_str = "ARM";
            std::cout << "\033[1;32m# " << hand_str << "-Send\033[0m " << printMillisecondTime() << " | can_id:" << std::hex << can_id << std::dec << " can_dlc:" << data.size() << " data:";
            for (auto &can : data) std::cout << std::hex << (int)can << std::dec << " ";
            std::cout << std::endl;
        }
        
        struct can_frame frame;
        frame.can_id = can_id;
        frame.can_dlc = data.size();
        memcpy(frame.data, data.data(), frame.can_dlc);

        #if 0
        if (write(socket_fd, &frame, sizeof(frame)) != sizeof(frame))
        {
            std::cout << "Failed to send CAN frame" << std::endl;
            // std::this_thread::sleep_for(std::chrono::milliseconds(100));
            // if (write(socket_fd, &frame, sizeof(frame)) != sizeof(frame)) {
            //     close(socket_fd);
            //     system("sudo /usr/sbin/ip link set can0 down");
            //     throw std::runtime_error("Failed to send CAN frame");
            // }
        }
        #endif
        
        ssize_t nbytes = write(socket_fd, &frame, sizeof(frame));
        if (nbytes != sizeof(frame)) {
            std::cerr << "Failed to send CAN frame: wrote " << nbytes
                      << " / " << sizeof(frame)
                      << "  errno=" << errno << " (" << strerror(errno) << ")" << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(4));
    }

    CANFrame CanBus::recv(const uint32_t)
    {
        struct can_frame frame;
        if (read(socket_fd, &frame, sizeof(frame)) != sizeof(frame)) {
            throw std::runtime_error("Failed to receive CAN frame");
        }

		// if(frame.can_id == id) 
		{
			CANFrame result;
			result.can_id = frame.can_id;
			result.can_dlc = frame.can_dlc;
			for (int i = 0; i < frame.can_dlc; ++i){
                result.data[i] = frame.data[i];
            }
			return result;
		}
		return CANFrame{};
    }

    void CanBus::updateSendRate() {
        std::lock_guard<std::mutex> lock(send_mutex);
        send_count++;

        auto current_time = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(current_time - last_time).count();

        if (elapsed >= 1) {
            std::cout << "CAN帧发送速率: " << send_count << " 帧/秒" << std::endl;
            send_count = 0;
            last_time = current_time;
        }
    }
    
    void CanBus::updateReceiveRate() {
        std::lock_guard<std::mutex> lock(receive_mutex);
        receive_count++;

        auto current_time = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(current_time - receive_last_time).count();

        if (elapsed >= 1) {
            std::cout << "CAN帧接收速率: " << receive_count << " 帧/秒" << std::endl;
            receive_count = 0;
            receive_last_time = current_time;
        }
    }
    
    void CanBus::setReceiveTimeout(int seconds, int microseconds)
    {
        struct timeval timeout;
        timeout.tv_sec = seconds;
        timeout.tv_usec = microseconds;

        if (setsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout)) < 0)
        {
            throw std::runtime_error("Failed to set receive timeout");
        }
    }

    bool CanBus::ping_once(const std::string& ifname, const Job& job) {
        int sock = socket(PF_CAN, SOCK_RAW, CAN_RAW);
        if (sock < 0) return false;

        ifreq ifr{};
        strncpy(ifr.ifr_name, ifname.c_str(), IFNAMSIZ - 1);
        if (ioctl(sock, SIOCGIFINDEX, &ifr) < 0) { close(sock); return false; }

        sockaddr_can addr{};
        addr.can_family  = AF_CAN;
        addr.can_ifindex = ifr.ifr_ifindex;

        if (bind(sock, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) { close(sock); return false; }
        setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &RX_TIMEOUT, sizeof(RX_TIMEOUT));

        can_frame tx{};
        tx.can_id  = job.id | (job.is_ext ? CAN_EFF_FLAG : 0);
        if (tx.can_id == 0x27 || tx.can_id == 0x28) {
        	tx.can_dlc = 1;
        } else {
        	tx.can_dlc = 8;
        }
        memset(tx.data, 0, 8);
        tx.data[0] = job.data0;

        if (write(sock, &tx, sizeof(tx)) != sizeof(tx)) { close(sock); return false; }

        can_frame rx{};
        // ssize_t n = read(sock, &rx, sizeof(rx));
        close(sock);
        
        if ((tx.can_id == 0x64 && rx.can_id == 0x64) || (tx.can_id == 0x64 && rx.can_id == 0x40000064)) return true;
        if (tx.can_id == 0x8000FD3D && rx.can_id == 0x80003DFE) return true;
        if (tx.can_id == 0x8000FD33 && rx.can_id == 0x800033FE) return true;
        if (tx.can_id == 0x28 && rx.can_id == 0x28) return true;
        if (tx.can_id == 0x27 && rx.can_id == 0x27) return true;
        
        return false;
    }

    bool CanBus::is_interface_up(const std::string& ifname)
    {
        int fd = socket(PF_CAN, SOCK_RAW, CAN_RAW);
        if (fd < 0) return false;

        ifreq ifr{};
        strncpy(ifr.ifr_name, ifname.c_str(), IFNAMSIZ - 1);

        bool up = (ioctl(fd, SIOCGIFFLAGS, &ifr) == 0) && (ifr.ifr_flags & IFF_UP);
        close(fd);
        return up;
    }

    std::vector<std::string> CanBus::detect_can_devices()
    {
        std::vector<std::string> cans;
        ifaddrs *ifa = nullptr;
        if (getifaddrs(&ifa) == -1) {
            perror("getifaddrs");
            return cans;
        }
        for (ifaddrs *p = ifa; p; p = p->ifa_next) {
            if (!p->ifa_name) continue;
            std::string name(p->ifa_name);
            if (name.rfind("can", 0) == 0 || name.rfind("vcan", 0) == 0)
                cans.emplace_back(name);
        }
        freeifaddrs(ifa);
        return cans;
    }
}
#endif
