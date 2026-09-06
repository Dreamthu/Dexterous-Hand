/**
 * @file lbot_api_cpp.h
 * @brief 灵心巧手机械臂控制API C++封装头文件
 * @date 2026.1.19
 * @copyright 灵心巧手科技有限公司
 */
#ifndef LBOT_API_CPP_H
#define LBOT_API_CPP_H

#include "lbot_api.h"
#include <string>
#include <vector>
#include <functional>

/**
 * @brief 灵心巧手机械臂控制API C++封装类
 * @details 提供C++友好的接口封装，使用std::string和std::vector简化内存管理
 */
namespace lbot {

class LbotApi {
public:
    // ==============================================
    // 构造函数和析构函数
    // ==============================================
    /**
     * @brief 构造函数
     */
    LbotApi();
    
    /**
     * @brief 析构函数，自动清理资源
     */
    ~LbotApi();

    // ==============================================
    // API初始化和清理函数
    // ==============================================
    /**
     * @brief 初始化LBot API连接
     * @param tcp_host TCP服务器地址格式："192.168.10.21"
     * @return lbot_handle_t 连接句柄，连接成功句柄ID>0，失败返回NULL
     */
    LBOT_API lbot_handle_t *lbot_init(const char* tcp_host);
        
    /**
     * @brief 断开指定连接
     * @param handle 机械臂句柄
     * @return true 断开成功，false 断开失败
     */
    LBOT_API bool lbot_disconnect(lbot_handle_t *handle);

    /**
     * @brief 清理API资源，断开连接
     */
    LBOT_API void lbot_cleanup();

    /**
     * @brief 获取API版本信息
     * @return 版本字符串
     */
    LBOT_API std::string lbot_get_api_version();

    // ==============================================
    // 系统信息获取函数
    // ==============================================
    /**
     * @brief 获取控制器信息
     * @param handle 机械臂句柄
     * @param robot_model 返回的机器人型号字符串
     * @param controller_version 返回的控制器版本字符串
     * @return true 获取成功，false 获取失败
     */
    LBOT_API bool lbot_get_controller_info(lbot_handle_t *handle, std::string& robot_model, std::string& controller_version);
    
    // ==============================================
    // 状态监控和管理函数
    // ==============================================
    /**
     * @brief 启动状态监控
     * @param state_cb 状态回调函数，当机器人状态更新时调用
     * @param error_cb 错误回调函数，当发生错误时调用
     * @return true 启动成功，false 启动失败
     */
    LBOT_API bool lbot_start_state_monitor(lbot_state_callback_t state_cb, lbot_error_callback_t error_cb);
    
    /**
     * @brief 停止状态监控
     */
    LBOT_API void lbot_stop_state_monitor();
    
    /**
     * @brief 获取当前机器人完整状态，此接口需要在启动状态监控后才能调用
     * @param handle 机械臂句柄
     * @param state 返回的机器人状态结构体指针
     * @return true 获取成功，false 获取失败
     */
    LBOT_API bool lbot_get_current_state(lbot_handle_t *handle, lbot_full_state_t* state);

    // ==============================================
    // 运动控制函数
    // ==============================================
    /**
     * @brief 关节空间运动
     * @param handle 机械臂句柄
     * @param arm 机械臂选择：LBOT_LEFT_ARM 或 LBOT_RIGHT_ARM
     * @param joints 7个关节的目标角度（弧度）
     * @param speed 运动速度（0.0~20.0）单位是rad/s, 建议从（0.0~2.0）开始使用后续如有需要逐步提高
     * @param accel 加速度（0.0~20.0）单位是rad/s^2, 建议从（0.0~2.0）开始使用后续如有需要逐步提高
     * @param block 是否阻塞执行：true 等待运动完成，false 立即返回
     * @return true 指令发送成功，false 发送失败
     */
    LBOT_API bool lbot_move_joint(lbot_handle_t *handle, lbot_arm_t arm, const double joints[7], double speed, double accel, bool block);
    
    /**
     * @brief 关节空间运动（使用vector）
     * @param handle 机械臂句柄
     * @param arm 机械臂选择：LBOT_LEFT_ARM 或 LBOT_RIGHT_ARM
     * @param joints 7个关节的目标角度（弧度）向量
     * @param speed 运动速度（0.0~20.0）单位是rad/s, 建议从（0.0~2.0）开始使用后续如有需要逐步提高
     * @param accel 加速度（0.0~20.0）单位是rad/s^2, 建议从（0.0~2.0）开始使用后续如有需要逐步提高
     * @param block 是否阻塞执行：true 等待运动完成，false 立即返回
     * @return true 指令发送成功，false 发送失败
     */
    LBOT_API bool lbot_move_joint(lbot_handle_t *handle, lbot_arm_t arm, const std::vector<double>& joints, double speed, double accel, bool block);
    
    /**
     * @brief 笛卡尔空间姿态运动（关节插值）
     * @param handle 机械臂句柄
     * @param arm 机械臂选择：LBOT_LEFT_ARM 或 LBOT_RIGHT_ARM
     * @param position 目标位置（x, y, z，单位：米）
     * @param euler 机械臂末端目标欧拉角（roll, pitch, yaw，单位：弧度）
     * @param speed 机械臂末端运动速度（0.0~20.0）单位m/s, 建议从（0.0~2.0）开始使用后续如有需要逐步提高
     * @param accel 机械臂末端加速度（0.0~20.0）单位m/s^2, 建议从（0.0~2.0）开始使用后续如有需要逐步提高
     * @param block 是否阻塞执行：true 等待运动完成，false 立即返回
     * @return true 指令发送成功，false 发送失败
     */
    LBOT_API bool lbot_move_pose(lbot_handle_t *handle, lbot_arm_t arm, const lbot_position_t* position, const lbot_euler_t* euler, 
                       double speed, double accel, bool block);
                       
    /**
     * @brief 笛卡尔空间姿态运动（使用向量）
     * @param handle 机械臂句柄
     * @param arm 机械臂选择：LBOT_LEFT_ARM 或 LBOT_RIGHT_ARM
     * @param position 目标位置（x, y, z，单位：米）向量
     * @param euler 机械臂末端目标欧拉角（roll, pitch, yaw，单位：弧度）向量
     * @param speed 机械臂末端运动速度（0.0~20.0）单位m/s, 建议从（0.0~2.0）开始使用后续如有需要逐步提高
     * @param accel 机械臂末端加速度（0.0~20.0）单位m/s^2, 建议从（0.0~2.0）开始使用后续如有需要逐步提高
     * @param block 是否阻塞执行：true 等待运动完成，false 立即返回
     * @return true 指令发送成功，false 发送失败
     */
    LBOT_API bool lbot_move_pose(lbot_handle_t *handle, lbot_arm_t arm, const std::vector<double>& position, const std::vector<double>& euler,
                       double speed, double accel, bool block);
    
    /**
     * @brief 笛卡尔空间直线运动（直线插值）
     * @param handle 机械臂句柄
     * @param arm 机械臂选择：LBOT_LEFT_ARM 或 LBOT_RIGHT_ARM
     * @param position 目标位置（x, y, z，单位：米）
     * @param euler 目标欧拉角（roll, pitch, yaw，单位：弧度）
     * @param speed 关节运动速度（0.0~20.0）单位是rad/s, 建议从（0.0~2.0）开始使用后续如有需要逐步提高
     * @param accel 关节运动加速度（0.0~20.0）单位是rad/s^2, 建议从（0.0~2.0）开始使用后续如有需要逐步提高
     * @param block 是否阻塞执行：true 等待运动完成，false 立即返回
     * @return true 指令发送成功，false 发送失败
     */
    LBOT_API bool lbot_move_linear(lbot_handle_t *handle, lbot_arm_t arm, const lbot_position_t* position, const lbot_euler_t* euler, 
                         double speed, double accel, bool block);
                         
    /**
     * @brief 笛卡尔空间直线运动（使用向量）
     * @param handle 机械臂句柄
     * @param arm 机械臂选择：LBOT_LEFT_ARM 或 LBOT_RIGHT_ARM
     * @param position 目标位置（x, y, z，单位：米）向量
     * @param euler 目标欧拉角（roll, pitch, yaw，单位：弧度）向量
     * @param speed 关节运动速度（0.0~20.0）单位是rad/s, 建议从（0.0~2.0）开始使用后续如有需要逐步提高
     * @param accel 关节运动加速度（0.0~20.0）单位是rad/s^2, 建议从（0.0~2.0）开始使用后续如有需要逐步提高
     * @param block 是否阻塞执行：true 等待运动完成，false 立即返回
     * @return true 指令发送成功，false 发送失败
     */
    LBOT_API bool lbot_move_linear(lbot_handle_t *handle, lbot_arm_t arm, const std::vector<double>& position, const std::vector<double>& euler,
                         double speed, double accel, bool block);

    // ==============================================
    // 关节跟随函数（用于遥操作）
    // ==============================================
    /**
     * @brief 关节跟随控制
     * @param handle 机械臂句柄
     * @param arm 机械臂选择：LBOT_LEFT_ARM 或 LBOT_RIGHT_ARM
     * @param joints 包含7个关节目标角度（弧度）的向量
     * @return true 指令发送成功，false 发送失败
     */
    LBOT_API bool lbot_joint_follow(lbot_handle_t *handle, lbot_arm_t arm, const std::vector<double>& joints);
        
    // ==============================================
    // 姿态跟随函数 
    // ==============================================
    /**
     * @brief 笛卡尔空间姿态跟随运动
     * @param handle 机械臂句柄
     * @param arm 机械臂选择：LBOT_LEFT_ARM 或 LBOT_RIGHT_ARM
     * @param pos 目标位置（x, y, z，单位：米）
     * @param eul 目标欧拉角（roll, pitch, yaw，单位：弧度）
     * @return true 指令发送成功，false 发送失败
     */
    LBOT_API bool lbot_pose_follow(lbot_handle_t *handle, lbot_arm_t arm, lbot_position_t pos, lbot_euler_t eul);
    
    // ==============================================
    // L6手控制接口
    // ==============================================
    /**
     * @brief 设置L6手的位置控制
     * @param handle 机械臂句柄
     * @param arm 机械臂选择：LBOT_LEFT_ARM 或 LBOT_RIGHT_ARM
     * @param position 6个手指的目标位置（0~255）向量
     * @return true 指令发送成功，false 发送失败
     */
    LBOT_API bool lbot_l6_set_position(lbot_handle_t *handle, lbot_arm_t arm, const std::vector<uint8_t>& position);
    
    /**
     * @brief 设置L6手的速度控制
     * @param handle 机械臂句柄
     * @param arm 机械臂选择：LBOT_LEFT_ARM 或 LBOT_RIGHT_ARM
     * @param velocity 6个手指的目标速度（0~255）向量
     * @return true 指令发送成功，false 发送失败
     */
    LBOT_API bool lbot_l6_set_velocity(lbot_handle_t *handle, lbot_arm_t arm, const std::vector<uint8_t>& velocity);
    
    /**
     * @brief 设置L6手的力矩控制
     * @param handle 机械臂句柄
     * @param arm 机械臂选择：LBOT_LEFT_ARM 或 LBOT_RIGHT_ARM
     * @param torque 6个手指的目标力矩（0~255）向量
     * @return true 指令发送成功，false 发送失败
     */
    LBOT_API bool lbot_l6_set_effort(lbot_handle_t *handle, lbot_arm_t arm, const std::vector<uint8_t>& torque);
    
    // ==============================================
    // L10手控制接口（10个自由度）
    // ==============================================
    /**
     * @brief 设置L10手的位置控制
     * @param handle 机械臂句柄
     * @param arm 机械臂选择：LBOT_LEFT_ARM 或 LBOT_RIGHT_ARM
     * @param position 10个手指的目标位置（0~255）向量
     * @return true 指令发送成功，false 发送失败
     */
    LBOT_API bool lbot_l10_set_position(lbot_handle_t *handle, lbot_arm_t arm, const std::vector<uint8_t>& position);
    
    /**
     * @brief 设置L10手的速度控制
     * @param handle 机械臂句柄
     * @param arm 机械臂选择：LBOT_LEFT_ARM 或 LBOT_RIGHT_ARM
     * @param velocity 10个手指的目标速度（0~255）向量
     * @return true 指令发送成功，false 发送失败
     */
    LBOT_API bool lbot_l10_set_velocity(lbot_handle_t *handle, lbot_arm_t arm, const std::vector<uint8_t>& velocity);
    
    /**
     * @brief 设置L10手的力矩控制
     * @param handle 机械臂句柄
     * @param arm 机械臂选择：LBOT_LEFT_ARM 或 LBOT_RIGHT_ARM
     * @param torque 10个手指的目标力矩（0~255）向量
     * @return true 指令发送成功，false 发送失败
     */
    LBOT_API bool lbot_l10_set_effort(lbot_handle_t *handle, lbot_arm_t arm, const std::vector<uint8_t>& torque);

    // ==============================================
    // L20手控制接口（20个自由度）
    // ==============================================
    /**
     * @brief 设置R20手的串联位置控制
     * @param handle 机械臂句柄
     * @param arm 机械臂选择：LBOT_LEFT_ARM 或 LBOT_RIGHT_ARM
     * @param cmd 目标位置命令，包含手指标识和6个电机位置
     * @return true 指令发送成功，false 发送失败
     */
    LBOT_API bool lbot_l20_set_series_position(lbot_handle_t *handle, lbot_arm_t arm, const lbot_l20_series_cmd_t* cmd);
    
    /**
     * @brief 设置R20手所有自由度位置控制
     * @param handle 机械臂句柄
     * @param arm 机械臂选择：LBOT_LEFT_ARM 或 LBOT_RIGHT_ARM
     * @param position 16个自由度的目标位置（度）向量
     * @return true 指令发送成功，false 发送失败
     */
    LBOT_API bool lbot_l20_set_all_position(lbot_handle_t *handle, lbot_arm_t arm, const std::vector<int>& position);
    
    // ==============================================
    // 运动学计算函数
    // ==============================================
    /**
     * @brief 正运动学计算
     * @param handle 机械臂句柄
     * @param arm 机械臂选择：LBOT_LEFT_ARM 或 LBOT_RIGHT_ARM
     * @param joints 7个关节角度（弧度）
     * @param position 返回的末端位置（x, y, z，单位：米）
     * @param euler 返回的末端欧拉角（roll, pitch, yaw，单位：弧度）
     * @return true 计算成功，false 计算失败
     */
    LBOT_API bool lbot_forward_kinematics(lbot_handle_t *handle, lbot_arm_t arm, const double joints[7], 
                                lbot_position_t* position, lbot_euler_t* euler);
    
    /**
     * @brief 正运动学计算（使用向量）
     * @param handle 机械臂句柄
     * @param arm 机械臂选择：LBOT_LEFT_ARM 或 LBOT_RIGHT_ARM
     * @param joints 7个关节角度（弧度）向量
     * @param position 返回的末端位置（x, y, z，单位：米）向量
     * @param euler 返回的末端欧拉角（roll, pitch, yaw，单位：弧度）向量
     * @return true 计算成功，false 计算失败
     */
    LBOT_API bool lbot_forward_kinematics(lbot_handle_t *handle, lbot_arm_t arm, const std::vector<double>& joints,
                                std::vector<double>& position, std::vector<double>& euler);
    
    /**
     * @brief 逆运动学计算
     * @param handle 机械臂句柄
     * @param arm 机械臂选择：LBOT_LEFT_ARM 或 LBOT_RIGHT_ARM
     * @param initial_joints 初始关节角度（弧度），用于求解器迭代
     * @param position 目标位置（x, y, z，单位：米）
     * @param euler 目标欧拉角（roll, pitch, yaw，单位：弧度）
     * @param result_joints 返回的7个关节角度解（弧度）
     * @return true 求解成功，false 求解失败
     */
    LBOT_API bool lbot_inverse_kinematics(lbot_handle_t *handle, lbot_arm_t arm, const double initial_joints[7], 
                                const lbot_position_t* position, const lbot_euler_t* euler, 
                                double result_joints[7]);
                                
    /**
     * @brief 逆运动学计算（使用向量）
     * @param handle 机械臂句柄
     * @param arm 机械臂选择：LBOT_LEFT_ARM 或 LBOT_RIGHT_ARM
     * @param initial_joints 初始关节角度（弧度）向量，用于求解器迭代
     * @param position 目标位置（x, y, z，单位：米）向量
     * @param euler 目标欧拉角（roll, pitch, yaw，单位：弧度）向量
     * @param result_joints 返回的7个关节角度解（弧度）向量
     * @return true 求解成功，false 求解失败
     */
    LBOT_API bool lbot_inverse_kinematics(lbot_handle_t *handle, lbot_arm_t arm, const std::vector<double>& initial_joints,
                                const std::vector<double>& position, const std::vector<double>& euler,
                                std::vector<double>& result_joints);

    // ==============================================
    // 工具坐标系管理函数
    // ==============================================
    /**
     * @brief 设置工具坐标系
     * @param handle 机械臂句柄
     * @param arm 机械臂选择：LBOT_LEFT_ARM 或 LBOT_RIGHT_ARM
     * @param name 工具坐标系名称（最大32字符）
     * @param position 工具坐标系相对于法兰盘的位置偏移（x, y, z，单位：米）
     * @param euler 工具坐标系相对于法兰盘的欧拉角偏移（roll, pitch, yaw，单位：弧度）
     * @return true 设置成功，false 设置失败
     */
    LBOT_API bool lbot_set_tool_frame(lbot_handle_t *handle, lbot_arm_t arm, const char* name, 
                            const lbot_position_t* position, const lbot_euler_t* euler);
    
    /**
     * @brief 设置工具坐标系（使用向量）
     * @param handle 机械臂句柄
     * @param arm 机械臂选择：LBOT_LEFT_ARM 或 LBOT_RIGHT_ARM
     * @param name 工具坐标系名称（最大32字符）
     * @param position 工具坐标系相对于法兰盘的位置偏移（x, y, z，单位：米）向量
     * @param euler 工具坐标系相对于法兰盘的欧拉角偏移（roll, pitch, yaw，单位：弧度）向量
     * @return true 设置成功，false 设置失败
     */
    LBOT_API bool lbot_set_tool_frame(lbot_handle_t *handle, lbot_arm_t arm, const std::string& name,
                            const std::vector<double>& position, const std::vector<double>& euler);
    
    /**
     * @brief 获取工具坐标系参数
     * @param handle 机械臂句柄
     * @param arm 机械臂选择：LBOT_LEFT_ARM 或 LBOT_RIGHT_ARM
     * @param name 工具坐标系名称
     * @param position 返回的工具坐标系位置偏移
     * @param euler 返回的工具坐标系欧拉角偏移
     * @return true 获取成功，false 获取失败
     */
    LBOT_API bool lbot_get_tool_frame(lbot_handle_t *handle, lbot_arm_t arm, const char* name, 
                            lbot_position_t* position, lbot_euler_t* euler);
    
    /**
     * @brief 获取工具坐标系参数（使用向量）
     * @param handle 机械臂句柄
     * @param arm 机械臂选择：LBOT_LEFT_ARM 或 LBOT_RIGHT_ARM
     * @param name 工具坐标系名称
     * @param position 返回的工具坐标系位置偏移向量
     * @param euler 返回的工具坐标系欧拉角偏移向量
     * @return true 获取成功，false 获取失败
     */
    LBOT_API bool lbot_get_tool_frame(lbot_handle_t *handle, lbot_arm_t arm, const std::string& name,
                            std::vector<double>& position, std::vector<double>& euler);
    
    /**
     * @brief 获取当前使用的工具坐标系
     * @param handle 机械臂句柄
     * @param arm 机械臂选择：LBOT_LEFT_ARM 或 LBOT_RIGHT_ARM
     * @param name 返回的当前工具坐标系名称
     * @param position 返回的当前工具坐标系位置偏移
     * @param euler 返回的当前工具坐标系欧拉角偏移
     * @return true 获取成功，false 获取失败
     */
    LBOT_API bool lbot_get_current_tool_frame(lbot_handle_t *handle, lbot_arm_t arm, 
                                    std::string& name, 
                                    lbot_position_t& position, 
                                    lbot_euler_t& euler);
    
    /**
     * @brief 获取当前使用的工具坐标系（使用向量）
     * @param handle 机械臂句柄
     * @param arm 机械臂选择：LBOT_LEFT_ARM 或 LBOT_RIGHT_ARM
     * @param name 返回的当前工具坐标系名称
     * @param position 返回的当前工具坐标系位置偏移向量
     * @param euler 返回的当前工具坐标系欧拉角偏移向量
     * @return true 获取成功，false 获取失败
     */
    LBOT_API bool lbot_get_current_tool_frame(lbot_handle_t *handle, lbot_arm_t arm,                                     std::string& name,
                                    std::vector<double>& position,
                                    std::vector<double>& euler);
    
    /**
     * @brief 切换当前工具坐标系
     * @param handle 机械臂句柄
     * @param arm 机械臂选择：LBOT_LEFT_ARM 或 LBOT_RIGHT_ARM
     * @param name 要切换到的工具坐标系名称
     * @return true 切换成功，false 切换失败
     */
    LBOT_API bool lbot_change_tool_frame(lbot_handle_t *handle, lbot_arm_t arm, const char* name);
    
    /**
     * @brief 切换当前工具坐标系（使用string）
     * @param handle 机械臂句柄
     * @param arm 机械臂选择：LBOT_LEFT_ARM 或 LBOT_RIGHT_ARM
     * @param name 要切换到的工具坐标系名称
     * @return true 切换成功，false 切换失败
     */
    LBOT_API bool lbot_change_tool_frame(lbot_handle_t *handle, lbot_arm_t arm, const std::string& name);
    
    /**
     * @brief 删除工具坐标系
     * @param handle 机械臂句柄
     * @param arm 机械臂选择：LBOT_LEFT_ARM 或 LBOT_RIGHT_ARM
     * @param name 要删除的工具坐标系名称
     * @return true 删除成功，false 删除失败
     */
    LBOT_API bool lbot_delete_tool_frame(lbot_handle_t *handle, lbot_arm_t arm, const char* name);
    
    /**
     * @brief 删除工具坐标系（使用string）
     * @param handle 机械臂句柄
     * @param arm 机械臂选择：LBOT_LEFT_ARM 或 LBOT_RIGHT_ARM
     * @param name 要删除的工具坐标系名称
     * @return true 删除成功，false 删除失败
     */
    LBOT_API bool lbot_delete_tool_frame(lbot_handle_t *handle, lbot_arm_t arm, const std::string& name);
    
    /**
     * @brief 获取所有工具坐标系名称
     * @param handle 机械臂句柄
     * @param arm 机械臂选择：LBOT_LEFT_ARM 或 LBOT_RIGHT_ARM
     * @param names 返回的工具坐标系名称向量
     * @return true 获取成功，false 获取失败
     */
    LBOT_API bool lbot_get_all_tool_frames(lbot_handle_t *handle, lbot_arm_t arm, std::vector<std::string>& names);
    
    // ==============================================
    // 工作坐标系管理函数
    // ==============================================
    /**
     * @brief 设置工作坐标系
     * @param handle 机械臂句柄
     * @param arm 机械臂选择：LBOT_LEFT_ARM 或 LBOT_RIGHT_ARM
     * @param name 工作坐标系名称（最大32字符）
     * @param position 工作坐标系相对于基坐标系的位置偏移（x, y, z，单位：米）
     * @param euler 工作坐标系相对于基坐标系的欧拉角偏移（roll, pitch, yaw，单位：弧度）
     * @return true 设置成功，false 设置失败
     */
    LBOT_API bool lbot_set_work_frame(lbot_handle_t *handle, lbot_arm_t arm, const char* name, 
                            const lbot_position_t* position, const lbot_euler_t* euler);
    
    /**
     * @brief 设置工作坐标系（使用向量）
     * @param handle 机械臂句柄
     * @param arm 机械臂选择：LBOT_LEFT_ARM 或 LBOT_RIGHT_ARM
     * @param name 工作坐标系名称（最大32字符）
     * @param position 工作坐标系相对于基坐标系的位置偏移（x, y, z，单位：米）向量
     * @param euler 工作坐标系相对于基坐标系的欧拉角偏移（roll, pitch, yaw，单位：弧度）向量
     * @return true 设置成功，false 设置失败
     */
    LBOT_API bool lbot_set_work_frame(lbot_handle_t *handle, lbot_arm_t arm, const std::string& name,
                            const std::vector<double>& position, const std::vector<double>& euler);
    
    /**
     * @brief 获取工作坐标系参数
     * @param handle 机械臂句柄
     * @param arm 机械臂选择：LBOT_LEFT_ARM 或 LBOT_RIGHT_ARM
     * @param name 工作坐标系名称
     * @param position 返回的工作坐标系位置偏移
     * @param euler 返回的工作坐标系欧拉角偏移
     * @return true 获取成功，false 获取失败
     */
    LBOT_API bool lbot_get_work_frame(lbot_handle_t *handle, lbot_arm_t arm, const char* name, 
                            lbot_position_t* position, lbot_euler_t* euler);
    
    /**
     * @brief 获取工作坐标系参数（使用向量）
     * @param handle 机械臂句柄
     * @param arm 机械臂选择：LBOT_LEFT_ARM 或 LBOT_RIGHT_ARM
     * @param name 工作坐标系名称
     * @param position 返回的工作坐标系位置偏移向量
     * @param euler 返回的工作坐标系欧拉角偏移向量
     * @return true 获取成功，false 获取失败
     */
    LBOT_API bool lbot_get_work_frame(lbot_handle_t *handle, lbot_arm_t arm, const std::string& name,
                            std::vector<double>& position, std::vector<double>& euler);
    
    /**
     * @brief 切换当前工作坐标系
     * @param handle 机械臂句柄
     * @param arm 机械臂选择：LBOT_LEFT_ARM 或 LBOT_RIGHT_ARM
     * @param name 要切换到的工作坐标系名称
     * @return true 切换成功，false 切换失败
     */
    LBOT_API bool lbot_change_work_frame(lbot_handle_t *handle, lbot_arm_t arm, const char* name);
    
    /**
     * @brief 切换当前工作坐标系（使用string）
     * @param handle 机械臂句柄
     * @param arm 机械臂选择：LBOT_LEFT_ARM 或 LBOT_RIGHT_ARM
     * @param name 要切换到的工作坐标系名称
     * @return true 切换成功，false 切换失败
     */
    LBOT_API bool lbot_change_work_frame(lbot_handle_t *handle, lbot_arm_t arm, const std::string& name);
    
    /**
     * @brief 删除工作坐标系
     * @param handle 机械臂句柄
     * @param arm 机械臂选择：LBOT_LEFT_ARM 或 LBOT_RIGHT_ARM
     * @param name 要删除的工作坐标系名称
     * @return true 删除成功，false 删除失败
     */
    LBOT_API bool lbot_delete_work_frame(lbot_handle_t *handle, lbot_arm_t arm, const char* name);
    
    /**
     * @brief 删除工作坐标系（使用string）
     * @param handle 机械臂句柄
     * @param arm 机械臂选择：LBOT_LEFT_ARM 或 LBOT_RIGHT_ARM
     * @param name 要删除的工作坐标系名称
     * @return true 删除成功，false 删除失败
     */
    LBOT_API bool lbot_delete_work_frame(lbot_handle_t *handle, lbot_arm_t arm, const std::string& name);
    
    /**
     * @brief 获取所有工作坐标系名称
     * @param handle 机械臂句柄
     * @param arm 机械臂选择：LBOT_LEFT_ARM 或 LBOT_RIGHT_ARM
     * @param names 返回的工作坐标系名称向量
     * @return true 获取成功，false 获取失败
     */
    LBOT_API bool lbot_get_all_work_frames(lbot_handle_t *handle, lbot_arm_t arm, std::vector<std::string>& names);
    
    // ==============================================
    // 系统功能函数
    // ==============================================
    /**
     * @brief 重新标定电机零位，设置当前位置为零位
     * @param handle 机械臂句柄
     * @param arm 机械臂选择：LBOT_LEFT_ARM 或 LBOT_RIGHT_ARM
     * @return true 设置成功，false 设置失败
     */
    LBOT_API bool lbot_set_zero(lbot_handle_t *handle, lbot_arm_t arm);
    
    /**
     * @brief 使能/掉使能机械臂
     * @param handle 机械臂句柄
     * @param arm 机械臂选择：LBOT_LEFT_ARM 或 LBOT_RIGHT_ARM
     * @param enable true 使能，false 掉使能
     * @return true 指令发送成功，false 发送失败
     */
    LBOT_API bool lbot_enable_arm(lbot_handle_t *handle, lbot_arm_t arm, bool enable);
    
    /**
     * @brief 紧急停止/恢复
     * @param handle 机械臂句柄
     * @param arm 机械臂选择：LBOT_LEFT_ARM 或 LBOT_RIGHT_ARM
     * @param enable true 紧急停止，false 恢复运行
     * @return true 指令发送成功，false 发送失败
     */
    LBOT_API bool lbot_emergency_stop(lbot_handle_t *handle, lbot_arm_t arm, bool enable);
    
    /**
     * @brief 清除所有错误
     * @param handle 机械臂句柄
     * @return true 清除成功，false 清除失败
     */
    LBOT_API bool lbot_clear_errors(lbot_handle_t *handle);

    /**
     * @brief 设置机械臂关节限位
     * @param handle 机械臂句柄
     * @param arm 机械臂选择：LBOT_LEFT_ARM 或 LBOT_RIGHT_ARM
     * @param upper_joint_limit 最大关节限位参数
     * @param lower_joint_limit 最小关节限位参数
     * @return true 设置成功，false 设置失败
     */
    LBOT_API bool lbot_set_joint_limit(lbot_handle_t *handle, lbot_arm_t arm, const double upper_joint_limit[7], const double lower_joint_limit[7]);
    /**
     * @brief 获取机械臂关节限位参数
     * @param handle 机械臂句柄
     * @param arm 机械臂选择：LBOT_LEFT_ARM 或 LBOT_RIGHT_ARM
     * @param upper_joint_limit 返回的最大关节限位参数
     * @param lower_joint_limit 返回的最小关节限位参数
     * @return true 获取成功，false 获取失败
     */
    LBOT_API bool lbot_get_joint_limit(lbot_handle_t *handle, lbot_arm_t arm, double upper_joint_limit[7], double lower_joint_limit[7]);
    /**
     * @brief 获取机械臂默认关节限位参数
     * @param handle 机械臂句柄
     * @param arm 机械臂选择：LBOT_LEFT_ARM 或 LBOT_RIGHT_ARM
     * @param upper_joint_limit 返回的最大关节限位参数
     * @param lower_joint_limit 返回的最小关节限位参数
     * @return true 获取成功，false 获取失败
     */
    LBOT_API bool lbot_get_default_joint_limit(lbot_handle_t *handle, lbot_arm_t arm, double upper_joint_limit[7], double lower_joint_limit[7]);

    // ==============================================
    // 工具函数
    // ==============================================
    /**
     * @brief 获取最后一次错误信息
     * @return 错误信息字符串
     */
    LBOT_API std::string lbot_get_last_error(lbot_handle_t *handle);
    
    /**
     * @brief 设置日志级别
     * @param level 日志级别：0-ERROR, 1-WARN, 2-INFO, 3-DEBUG
     */
    LBOT_API void lbot_set_log_level(int level);

    // ==============================================
    // 内存管理辅助函数
    // ==============================================
    /**
     * @brief 释放字符串数组内存
     * @param array 要释放的字符串数组
     * @param count 数组元素数量
     */
    LBOT_API void lbot_free_string_array(char** array, int count);

private:
    // 禁用拷贝构造和赋值操作
    LbotApi(const LbotApi&) = delete;
    LbotApi& operator=(const LbotApi&) = delete;
};

} // namespace lbot

#endif // LBOT_API_CPP_H