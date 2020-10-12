#ifndef INCLUDES_H
#define INCLUDES_H


#define SYSTIME         500    //系统时间

/*设备类型*/
#define DEV_EFMQ        0//电气火灾
#define DEV_PMFE        1//电源监控

#define N_LOOP          0//节点回路地址
#define N_NODEID        1//节点ID地址
#define N_AREA          3//节点安装区域

/*用户级别*/
#define USERS           0
#define ADMIN           1
#define SUPER           2
/*历史记录*/
#define R_NAME          0//主机名称
#define R_HOST          1//主机地址
#define R_LOOP          2//主机回路
#define R_IDNO          3//探测器地址
#define R_TYPE          4//探测器类型
#define R_STATE         5//探测器状态
#define R_TIME          6//报警时间
#define R_AREA          7//安装位置

#define ROWMUN          10//每页记录的数目

#define COLUMNSIZE      8  //每列节点数目
#define PAGESTEP        490//每页步长
#define NODECOUNT       56 //每页节点总数
#define LOOPERROR       10 //回路心跳丢失次数

/*探测器型号*/
#define MOD_UREG        0x00//
#define MOD_L1T4        0x01//一体式
#define MOD_L12T4       0x02//组合式
#define MOD_EARC        0x03//电弧
#define MOD_PYRO        0x04//热解
#define MOD_THER        0x09//热成像
#define MOD_2VA         0x11//三相-两路电压一路电流
#define MOD_VA          0x12//三相-一路电压一路电流
#define MOD_A           0x13//三相-一路电流
#define MOD_3V          0x14//三相-三路电压
#define MOD_2V          0x15//三相-两路电压
#define MOD_V           0x16//三相-一路电压
#define MOD_6V3A        0x17//单相-六路电压三路电流
#define MOD_6V          0x18//单相-六路电压
#define MOD_3A          0x19//单相-三路电流

/*探测器状态*/
#define N_NORMAL        0x01//通讯正常
#define N_POWERLOST     0x02//电源中断
#define N_OVERVOL       0x03//过压故障
#define N_OVERCUR       0x04//过流故障
#define N_LACKVOL       0x05//节点欠压
#define N_LACKPHA       0x06//节点缺相
#define N_ERRORPHA      0x07//节点错相
#define N_ERROR         0x08//传感器故障
#define N_ALARM         0x09//通道报警
#define N_OFFLINE       0x0A//通讯故障
#define N_COVER         0x0B//遮挡
#define N_DISCON        0x0C//断网
#define N_COVDIS        0x0D//遮挡+断网
#define N_MAINPOW       0xAA//主电故障
#define N_BACKPOW       0xBB//备电故障
#define N_RESET         0xCC//复位操作
#define N_MUTE          0xDD//静音操作
#define N_EXTEERROR     0xEE//分机故障

/*UDP 发送数据格式*/
#define DATA_HEAD       0x00
#define DATA_LOOP       0x01
#define DATA_ID         0x02
#define DATA_TYPE       0x03
#define DATA_STATE      0x04
#define DATA_TAIL       0x05
#define HEAD_AA         0xAA
#define TAIL_FF         0xFF
#define DATASIZE        6

#endif // INCLUDES_H
