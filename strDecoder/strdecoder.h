#ifndef STRDECODER_H
#define STRDECODER_H

#include <QObject>
#include <QList>

class portThread;
class QTimer;

namespace CtrlType {
    enum {
        StepMotor,
        ServoMotor,
        Robot,
        Count,
    };
    enum StepMotorType {
        Motor35,
        Motor_Pipelet = Motor35,  ///取液管电机
        Motor57,
        Motor_Tube = Motor57,   ///反应管上下电机
        Motor86,
        Motor_Stove = Motor86,  ///炉膛开合电机
    };
}

class StepMotorStat
{
public:
    StepMotorStat();
    StepMotorStat(uint8_t stat);
    void ChangeFromStat(uint8_t stat);
    bool IsLimitL()const;
    bool IsLimitH()const;
    bool IsDown()const;
    bool IsRun()const;
    uint8_t GetType()const;
    void SetType(uint8_t t);
    uint8_t GetChannel()const;
    void SetChannel(uint8_t t);
    void Fresh();
    StepMotorStat &operator=(const StepMotorStat& m);
    friend bool operator==(const StepMotorStat &m1, const StepMotorStat &m2);
private:
    bool m_bLimitL = false;
    bool m_bLimitH = false;
    bool m_bDown = false;
    bool m_bRun = false;
    uint8_t m_type;
    uint8_t m_ch;
};

class ServoMotorStat
{
public:
    ServoMotorStat();
    ServoMotorStat(uint8_t stat);
    void ChangeFromStat(uint8_t stat);
    void Fresh();
    int GetPos()const;
    bool IsReached()const;
    ServoMotorStat& operator=(const ServoMotorStat& m);
    friend bool operator==(const ServoMotorStat& m1, const ServoMotorStat& m2);
private:
    int16_t m_nPos = -1;
    bool m_bReached = false;
};

class DeviceAct;
class strDecoder : public QObject
{
    Q_OBJECT
public:
	enum {
		Job_AirClear = 10,
		Job_AirIn,
	};
public:
    explicit strDecoder(QObject *parent = nullptr, const QString &name = "COM7");
    ~strDecoder();

    void strTocmd(const QString &cmd);   //  添加待处理的命令
    void cmdSend();
    portThread* getThread()const;
    const StepMotorStat* GetStepMotorStatOf(int idx)const;
    const ServoMotorStat* GetServoMotorStat()const;

    static QByteArray floatToBigEndian(float value);
    static float bigEndianToFloat(const QByteArray& bytes);
    static int getStovePos(int ch);
    static strDecoder *Instance();
public slots:
    void timer_out();
    void setTimer(int time);
    void savePortName(QString name);
    bool com_open(bool state,const QString &name);
    void app_connected();
    void app_disconnected();
private:
    void board_msg_request();   // 查询
    void swCtrl(int id,bool state);  // 阀门控制
    void programStepMotor(const QStringList& cmd, CtrlType::StepMotorType tp);//35电机控制
    void programMotorRobot(const QStringList& cmd);//机械臂滑轨控制
    void programHeatMixture(const QStringList& cmd, int tp);//加熱套、保温箱控制
    void programValve3Ch(const QString& cmd);//电动3通阀控制
    void swCtrl_flow(int id,bool state);  // 流量短接控制
    void flowCtrl(int id,double flow);  // 流量控制
    void flowCtrl_Range(int id,double flow);  // 流量量程
    void liqudiCtrl_Range(int id,double flow);  // 液位量程
    void presCtrl_Range(int id,double flow);  // 压力量程
    void programTemp(QString cmd);   // 程序段升温
    void tempStop(int id);                 // 程序升温停止
    void pointTemp(int id,int temp); // 单点升温
    void sloPeTemp(int id,double temp1,int time,double temp2); // 斜率升温
    void tempAdjust(int id,double temp);  // 单次整定
    void stepTemp(int id, int index, int temp); // 修改单段温度
    void stepTime(int id, int index, int time); // 修改单段时间
    void currentCtrl(int current);   // 桥流设置
    void SepuZhipuCtrl(int id);   // 色谱和质谱触发
    void pumpCtrl(uint8_t id, bool state);  // 注射泵控制
    void pumpSpeeed(uint8_t id, double speed);  // 注射泵速率
    void pumpClean(uint8_t id, uint8_t state); // 清洗
    void pumpCali(uint8_t id, double cali); // 校准系数
    void valve_set_pres(uint8_t id, float pres);  // 背压阀压力
    void valve_set_mode(uint8_t id, uint8_t mode);  // 背压阀手动/自动
    void valve_set_manual_state(uint8_t id, uint8_t mode);  // 设置手动模式 全开 全关 停止
    void collector_conn(QString cmd);
    void ctrlStepMotor(CtrlType::StepMotorType tp, uint8_t ch, uint8_t dir, uint16_t rpm = 0); ///rpm=转速*10；
    void ctrlServoMotor(uint8_t pos); ///rpm=转速*10；

    void appendToQue(const uint8_t* cmd, uint32_t len);
	void onActionRun(const DeviceAct *act);
signals:
    void setConnectionState(bool state);
    void startRecord(bool state);
    void autoSavedata(void);
    void baseAdjust(void);
    void setPumpSpeed(uint32_t speed);
    void set_flow_sw_state(bool state);
    void stepMotorStatChanged(StepMotorStat*);
    void servoMotorStatChanged(int pos, bool);
    void jobChaned(uint16_t type, uint16_t idx);
private:
    void onAckRecved(const QByteArray &arr);

    void prcsMotor(const QByteArray& msg);
private:
    portThread* thread;
    QTimer              *m_timer;
    uint32_t    sleepTime;
    uint32_t    m_countReq=0;
    QString portName;
    QList<QByteArray>   m_cmdlist;   // 发送队列
    StepMotorStat   m_stepMotorStat[6];
    ServoMotorStat  m_servoMotorStat;
};

#endif // STRDECODER_H
