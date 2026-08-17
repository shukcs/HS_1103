#ifndef PORTTHREAD_H
#define PORTTHREAD_H

#include <QThread>

#define DATA_LEN    56
#define STOVE_NUM   2
#define PONIT_NUM   2
#define FLOW_NUM    2
#define PUMP_NUM    2
#define VALVE_NUM   2
#define SWITCH_NUM  4
#define PRESS_NUM   6
#define LIQUID_NUM   2


class  ReceiveData
{
public:
    double PointTemp[STOVE_NUM+PONIT_NUM]; // 温度
    double Flow[FLOW_NUM+PUMP_NUM];  // 流量
    double Flow_range[FLOW_NUM];  // 量程
    double pres[PRESS_NUM+PUMP_NUM+VALVE_NUM]; // 压力
    double pres_range[PRESS_NUM];  // 量程
    int liquid[LIQUID_NUM]; // 液位
    int liquid_range[LIQUID_NUM]; // 量程

    int ProgStep[STOVE_NUM];    // 程序段
    int ProgTime[STOVE_NUM];    // 运行时间
    double Aimtemp[STOVE_NUM];  // 目标温度
    uint8_t mv[STOVE_NUM];      // 输出MV
    bool adjState[STOVE_NUM];   // 整定状态
    uint8_t pump_code[PUMP_NUM];
    uint8_t valve_code[VALVE_NUM];
    uint8_t Switch_state[SWITCH_NUM];
    uint8_t collector[50];      // 收集器
    double pump_cali[PUMP_NUM]; // 泵校准系数
public:
    ReceiveData()
    {
       int i;
       for(i=0;i<(STOVE_NUM+PONIT_NUM);i++)
       {
            PointTemp[i] = 0;
       }
       for(i=0;i<(FLOW_NUM+PUMP_NUM);i++)
       {
            Flow[i] = 0;
       }
       for(i=0;i<FLOW_NUM;i++)
       {
            Flow_range[0] = 0;
       }
       for(i=0;i<STOVE_NUM;i++)
       {
            ProgStep[i] = 0;
            ProgTime[i] = 0;
            Aimtemp[i] = 0;
            mv[i] = 0;
            adjState[i] = false;
       }
       for(i=0;i<(PRESS_NUM+PUMP_NUM+VALVE_NUM);i++)
       {
            pres[i] = 0;
       }
       for(i=0;i<PRESS_NUM;i++)
       {
            pres_range[i] = 0;
       }
       for(i=0;i<LIQUID_NUM;i++)
       {
            liquid[i] = 0;
            liquid_range[i] = 0;
       }
       for(i=0;i<PUMP_NUM;i++)
       {
            pump_code[i] = 0;
            pump_cali[i] = 0;
       }
       for(i=0;i<PUMP_NUM;i++)
       {
            valve_code[i] = 0;
       }
       for(i=0;i<SWITCH_NUM;i++)
       {
            Switch_state[i] = 0;
       }
       for(i=0;i<50;i++)
       {
            collector[i] = 0;
       }
    }
};

class QSerialPort;
class QTimer;
class portThread : public QThread
{
    Q_OBJECT
public:
    typedef struct TriEleValveStat {
        bool bReached = false;
        uint8_t pos = 0xff;
    }TriEleValveStat;
public:
    explicit portThread(QWidget *parent = nullptr);
    ~portThread();

    bool port_setup(const QString &name);
    int port_refresh(QVector<QString> *name);
    void port_open(bool state);
    bool port_state(void);
    void port_write(uint8_t *data, int len);
    bool get_connection_state(void);
    
    float GetTemperatureOfHeatAndKeep(int idx)const;
    const TriEleValveStat* GetTriEleValveStat(int idx)const;
public slots:
    void timer_timeout(void);
    void stop_timer();
protected:
    void serial_ready(void);
    void prcsReport(const QByteArray& msg);
    void prcsHeatAndKeep(const QByteArray& msg);
    void prcsTriEleValve(const QByteArray& msg);
    QByteArray pickMsg();
signals:
    void port_connected();
    void port_disconnected();
    void timer_stop();
    void ReceiceDone(ReceiveData* data);
    void ackRecved(const QByteArray&);
    void heatAndKeepChanged(int idx, float);
    void triEleValveStat(int, int, bool);
private:
    ReceiveData     *m_recdata;
    QSerialPort     *m_serialport;
    QTimer          *m_timer;
    bool            connection_state;
    QString         com_name;
    QByteArray      m_buff;
    bool buff_state[2];
    uint16_t        m_tempHeatAndKeep[4];
    TriEleValveStat m_triEleValveStat[2];
};
#endif // PORTTHREAD_H
