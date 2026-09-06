#include "teleopmasterarm.h"

namespace telebot
{

TeleopMasterArm::TeleopMasterArm(const std::string device_type) : running(true)
{
    joint_position.resize(14, 0.0);
    joint_error_code.resize(14, false);

    bus = std::make_unique<Communication::CanBus>(device_type);
    receiveThread = std::thread(&TeleopMasterArm::receiveResponse, this);
    
    bus->send({}, FRAME_PROPERTY::LINKER_HAND_VERSION);
}

TeleopMasterArm::TeleopMasterArm(const std::string &canChannel, int baudrate) : running(true)
{
    joint_position.resize(14, 0.0);
    joint_error_code.resize(14, false);
    
    bus = std::make_unique<Communication::CanBus>(canChannel, baudrate);
    receiveThread = std::thread(&TeleopMasterArm::receiveResponse, this);
    
    bus->send({}, FRAME_PROPERTY::LINKER_HAND_VERSION);
}

TeleopMasterArm::~TeleopMasterArm()
{
    running = false;
    if (receiveThread.joinable()) {
        receiveThread.join();
    }
}

void TeleopMasterArm::printJointInfo() {
    #if 0
    std::cout << "\033[2J\033[H" << std::flush << std::endl;
    for (size_t i = 0; i < joint_position.size(); i++) {
        // if (i == 7) std::cout << "---------------------" << std::endl;
        std::cout << i << " : " << joint_position[i] << "  |  " << (joint_error_code[i] ? "FAULT" : "OK") << std::endl;
    }
    #endif
    
    #if 1
    static std::vector<float> pos_last;
    static std::vector<bool>   err_last;
    static std::mutex          print_mtx;
    std::lock_guard<std::mutex> lock(print_mtx);

    if (pos_last != joint_position || err_last != joint_error_code) {
        pos_last = joint_position;
        err_last = joint_error_code;

        std::cout << "\033[2J\033[H" << std::flush;
        std::cout << std::endl;
        
        for (size_t i = 0; i < pos_last.size(); ++i) {
            std::cout << std::setw(2) << i << " : "
                      // << std::fixed << std::setprecision(1) << pos_last[i]
                      << pos_last[i]
                      << "  |  "
                      << (err_last[i] ? "\033[31mFAULT\033[0m" : "\033[32mOK\033[0m")
                      << '\n';
        }
        std::cout << std::flush;
    }
    #endif
}

std::vector<bool> TeleopMasterArm::getJointErrorCode() 
{
    std::lock_guard<std::mutex> lock(err_mtx);
    return joint_error_code;
}

std::vector<float> TeleopMasterArm::getJointPosition() 
{
    // bus->send({}, FRAME_PROPERTY::GET_JOINT_POSITION_1);
    // bus->send({}, FRAME_PROPERTY::GET_JOINT_POSITION_2);
    bus->send({}, FRAME_PROPERTY::GET_JOINT_POSITION);
    
    std::lock_guard<std::mutex> lock(pos_mtx);
    return joint_position;
}

std::string TeleopMasterArm::getVersion() 
{
    std::stringstream ss;
    if (version.size() > 0) {
        int major = version[0] / 100;
        int minor = (version[0] % 100) / 10;
        int patch = version[0] % 10;
        ss << "Hardware Version: v" << major << '.' << minor << '.' << patch << std::endl;
        ss << "Software Version: v" << "1.0.3";
    }
    return ss.str();
}

void TeleopMasterArm::resetZero(const uint8_t joint) 
{
    if (joint != 0xFF) {
        bus->send({joint}, FRAME_PROPERTY::RESET_ZERO_COMMAND_1);
    } else {
        bus->send({}, FRAME_PROPERTY::RESET_ZERO_COMMAND_2);
    }
}

std::string TeleopMasterArm::getCurrentTime()
{
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

void TeleopMasterArm::receiveResponse()
{
    while (running) {
        try {
            auto frame = bus->recv();
            
            if (frame.can_dlc == 0) continue;
            
            std::vector<uint8_t> data(frame.data, frame.data + frame.can_dlc);
            // uint8_t frame_property = data[0];
            std::vector<uint8_t> payload(data.begin(), data.end());
            
            if (RECV_DEBUG) {
                static std::string hand_str = "ARM";
            std::cout << "\033[1;32m# " << hand_str << "-Recv\033[0m " << getCurrentTime() << " | can_id:" << std::hex << frame.can_id << std::dec << " can_dlc:" << (int)frame.can_dlc << " data:";
                for (auto &can : data) std::cout << std::hex << (int)can << std::dec << " ";
                std::cout << std::endl;
            }
            
            if (frame.can_id == FRAME_PROPERTY::LEFT_JOINT_POSITION_2 || frame.can_id == FRAME_PROPERTY::RIGHT_JOINT_POSITION_2) {
                int base = (frame.can_id == FRAME_PROPERTY::LEFT_JOINT_POSITION_2) ? 0 : 7;
                std::lock_guard<std::mutex> lock(err_mtx);
                if (frame.can_dlc >= 7) {
                    uint8_t fault_byte = frame.data[6];
                    for (int i = 0; i < 7; ++i) {
                        bool is_fault = (fault_byte >> i) & 0x01;
                        // printf("Joint %d: %s\n", base + i, is_fault ? "FAULT" : "OK");
                        joint_error_code[base + i] = is_fault;
                    }
                } else if (frame.can_dlc == 6) {
                    for (int i = 0; i < 7; ++i) {
                        joint_error_code[base + i] = false;
                    }
                }
            }
            
            std::lock_guard<std::mutex> lock(pos_mtx);
            switch(frame.can_id) {
                case FRAME_PROPERTY::LEFT_JOINT_POSITION_1:
                    joint_position[0] = bytesToInt16(joint_position[0], frame.data[0], frame.data[1]);
                    joint_position[1] = bytesToInt16(joint_position[1], frame.data[2], frame.data[3]);
                    joint_position[2] = bytesToInt16(joint_position[2], frame.data[4], frame.data[5]);
                    joint_position[3] = bytesToInt16(joint_position[3], frame.data[6], frame.data[7]);
                    break;
                case FRAME_PROPERTY::LEFT_JOINT_POSITION_2:
                    joint_position[4] = bytesToInt16(joint_position[4], frame.data[0], frame.data[1]);
                    joint_position[5] = bytesToInt16(joint_position[5], frame.data[2], frame.data[3]);
                    joint_position[6] = bytesToInt16(joint_position[6], frame.data[4], frame.data[5]);
                    break;
                case FRAME_PROPERTY::RIGHT_JOINT_POSITION_1:
                    joint_position[7] = bytesToInt16(joint_position[7], frame.data[0], frame.data[1]);
                    joint_position[8] = bytesToInt16(joint_position[8], frame.data[2], frame.data[3]);
                    joint_position[9] = bytesToInt16(joint_position[9], frame.data[4], frame.data[5]);
                    joint_position[10] = bytesToInt16(joint_position[10], frame.data[6], frame.data[7]);
                    break;
                case FRAME_PROPERTY::RIGHT_JOINT_POSITION_2:
                    joint_position[11] = bytesToInt16(joint_position[11], frame.data[0], frame.data[1]);
                    joint_position[12] = bytesToInt16(joint_position[12], frame.data[2], frame.data[3]);
                    joint_position[13] = bytesToInt16(joint_position[13], frame.data[4], frame.data[5]);
                    break;
                case FRAME_PROPERTY::RESET_ZERO_COMMAND_1:

                    break;
                case FRAME_PROPERTY::RESET_ZERO_COMMAND_2:
                   
                    break;
                case FRAME_PROPERTY::LINKER_HAND_VERSION:
                    version = payload;
                    break;
                default:
                    // if (RECV_DEBUG) std::cout << "Unknown data type: " << std::hex << (int)frame_property << std::endl;
                    continue;
            }
        } catch (const std::runtime_error &e) {
            // std::cerr << "Error receiving data: " << e.what() << std::endl;
        }
    }
}
}

