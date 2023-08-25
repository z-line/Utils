#ifndef __TYPE_H
#define __TYPE_H

class TypeGroup {
public:
  enum class FunCode {
    // 终端->PC&集控中心
    ChargePointNum = 0x0,      // 充电桩编号
    ChargePointStatus = 0x1,   // 充电桩状态
    ChargeRequestInfo = 0x2,   // 请求电压&电流
    FaultUpload = 0x3,         // 上报故障信息
    FaultIgnoreMask = 0x4,     // 故障屏蔽信息
    RequestPowerModule = 0x5,  // 请求电源模块使用信息
    BMSInfo = 0x6,             // 车端信息
    MeterValue = 0x7,          // 电表读数
    ChargedEnergy = 0x8,       // 当前充电量
    VINCode = 0x9,             // VIN码
    FanThreshold = 0xA,        // 上报风机启动&停止温度
    EngineerPasswd = 0xB,      // 工程师页面密码
    ChargePasswd = 0xC,        // 充电启动密码
    RFID = 0xD,                // 刷卡ID
    StartStopReason = 0xE,     // 启停操作上报
    Heartbeat = 0xF,           // 心跳
    // PC->终端
    QueryTerminalParam = 0xA0,  // 查询充电桩参数
    PCOutput = 0xA2,            // 功率分配控制器输出电压&电流
    FaultAck = 0xA3,            // 故障应答
    SetFaultIgnoreMask = 0xA4,  // 设置终端故障屏蔽代码
    PowerDeliveryReply = 0xA1,  // PC分配模块状态
    SetFanThreshold = 0xA5,     // 设置风机启停温度
    VINRet = 0xA9,              // VIN充电输出标志
    SetEngineerPasswd = 0xAB,   // 工程师界面密码
    SetChargePasswd = 0xAC,     // 充电启动密码
    REIDReply = 0xAD,           // 卡鉴权结果
    TimeSync = 0xAF,            // 时间同步
    // 集控中心->终端
    PlatformSelect = 0x30,     // 运营平台选择
    BillChargedEnergy = 0x31,  // 结算充电电量
    BillCost = 0x32,           // 结算费用
    PlatformData = 0x33,  // 平台数据（电价、充电桩二维码等）
    StartCharge = 0x34,   // 启动充电
    StopCharge = 0x35,    // 停止充电
    BillId = 0x36,        // 充电流水号
    PhotoNum = 0x37,      // 手机号后4位
  };

};

#endif
