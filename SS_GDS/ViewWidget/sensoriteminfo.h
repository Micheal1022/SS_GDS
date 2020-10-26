#ifndef SENSORITEMINFO_H
#define SENSORITEMINFO_H
#include <QObject>

class SensorItemInfo
{
public:
    SensorItemInfo () {
        m_zoom = 0.0;
        m_posX = 0.0;
        m_posY = 0.0;
        m_idStr.clear();
        m_loopStr.clear();
        m_typeStr.clear();
        m_areaStr.clear();
        m_errorFlag   = false;
        m_alarmFlag   = false;
        m_normalFlag  = false;
        m_offLineFlag =false;
        m_powerLostFlag = false;
        m_overVolFlag = false;
        m_overCurFlag = false;
        m_lackVolFlag = false;
        m_lackPhaFlag = false;
        m_errorPhaFlag= false;

    }
    qreal m_zoom;       //缩放大小比
    qreal m_posX;       //X轴位置
    qreal m_posY;       //Y轴位置
    QString m_idStr;    //探测器地址
    QString m_loopStr;  //探测器回路
    QString m_typeStr;  //探测器类型
    QString m_areaStr;  //安装位置
    bool m_powerLostFlag;
    bool m_overVolFlag;
    bool m_overCurFlag;
    bool m_lackVolFlag;
    bool m_lackPhaFlag;
    bool m_errorPhaFlag;
    bool m_errorFlag;
    bool m_alarmFlag;
    bool m_normalFlag;
    bool m_offLineFlag;
};

#endif // SENSORITEMINFO_H
