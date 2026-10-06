#ifndef __CtrlAction_H__
#define __CtrlAction_H__
#include "common/ActionItem.h"

class LabelItem : public ActionAbstrctItem
{
public:
	LabelItem(const QString &label = QString());

	const QString &Name()const;
	void Save(QDataStream *dstr)override;
	void Load(QDataStream *dstr)override;
	QString ToString(bool bStart = true)const override;
	void Distribute()override;
private:
	QString m_label;
};

class AirClrAction : public ActionAbstrctItem
{
public:
    AirClrAction(uint16_t ch=0, float prs=0, uint16_t sec=0, int16_t seq=-1);

    QString ToString(bool bStart = true)const override;
    void Save(QDataStream *dstr)override;
    void Load(QDataStream *dstr);
    void Distribute()override;
    uint16_t GetChannel()const;
    uint16_t ClearSeconds()const;
    float GetPressure()const;
private:
    uint16_t    m_ch;
    uint16_t    m_tm;
    float       m_pressure;
};

class AirInAction : public ActionAbstrctItem
{
public:
    AirInAction(uint16_t ch=0, float prsIn=0, float mlPsec=0, float prsKp=0, int16_t seq = -1);

    QString ToString(bool bStart = true)const override;
    void Save(QDataStream *dstr)override;
	void Load(QDataStream *dstr)override;
    void Distribute()override;
    uint16_t GetChannel()const;
    float GetPressIn()const;///入气阀压力
    float GetPressKeep()const;///背压阀保持压力
    float GetFlow()const;///气流速度ml/min
private:
    uint16_t    m_ch;
    float       m_prsIn;///入气阀压力
    float       m_prsKp;///背压阀保持压力
    float       m_mlPmin;///气流速度ml/min
};

class AirEndAction : public ActionAbstrctItem
{
public:
    AirEndAction(uint16_t ch=0, int16_t seq = -1);
    QString ToString(bool bStart = true)const override;

    void Save(QDataStream *dstr)override;
	void Load(QDataStream *dstr)override;
    void Distribute()override;
    uint16_t GetChannel()const;
private:
    uint16_t    m_ch;
};

class LiquidInAction : public ActionAbstrctItem
{
public:
    LiquidInAction(uint16_t ch = 0, float mlPmin = 0, uint16_t sec = 0, int16_t seq = -1);
    QString ToString(bool bStart = true)const override;

    void Save(QDataStream *dstr)override;
	void Load(QDataStream *dstr)override;
    void Distribute()override;
    uint16_t GetChannel()const;
    uint16_t ClearAirSeconds()const;
    float GetFlow()const;
private:
    uint16_t    m_ch;
    uint16_t    m_tmAirOut;
    float       m_mlPmin;
};

class LiquidEndAction : public ActionAbstrctItem
{
public:
    LiquidEndAction(uint16_t ch = 0, int16_t seq = -1);
    QString ToString(bool bStart = true)const override;

    void Save(QDataStream *dstr);
	void Load(QDataStream *dstr);
	void Distribute()override;
    uint16_t GetChannel()const;
private:
    uint16_t    m_ch;
};

class DelayAction : public ActionAbstrctItem
{
public:
	DelayAction(float min=0/*, uint16_t ch*/, int16_t seq = -1);
	QString ToString(bool bStart = true)const override;

	void Save(QDataStream *dstr);
	void Load(QDataStream *dstr);
	void Distribute()override;
    float DelayMinutes()const;
    float DelaySeconds(uint32_t msec)const;
private:
	float    m_delayMin;
};

class RecordAction : public ActionAbstrctItem
{
public:
    RecordAction(bool bRec = 0, int16_t seq = -1);
    QString ToString(bool bStart = true)const override;

    void Save(QDataStream *dstr);
    void Load(QDataStream *dstr);
    void Distribute()override;

    bool IsRecord()const;
private:
    bool    m_bRec;
};

class CycleAction : public ActionAbstrctItem
{
public:
    CycleAction(uint16_t nCycle = 0, int16_t seq = -1);
    QString ToString(bool bStart = true)const override;

    void Save(QDataStream *dstr);
    void Load(QDataStream *dstr);
    void Distribute()override;
    bool IsEnd()const;
    uint16_t CycleCount()const;
private:
    uint16_t    m_nCycle;
};

class HeatAction : public ActionAbstrctItem
{
public:
    HeatAction(uint16_t tmp = 0, uint16_t ch = 0, int16_t seq = -1);
    QString ToString(bool bStart = true)const override;

    void Save(QDataStream *dstr);
    void Load(QDataStream *dstr);
    void Distribute()override;
    uint16_t GetTemp()const;
    uint16_t GetChannel()const;
private:
    uint16_t    m_nTmp;
    uint16_t    m_nCh;
};

class KeepAction : public ActionAbstrctItem
{
public:
    KeepAction(uint16_t tmp = 0, uint16_t ch = 0, int16_t seq = -1);
    QString ToString(bool bStart = true)const override;

    void Save(QDataStream *dstr);
    void Load(QDataStream *dstr);
    void Distribute()override;
    uint16_t GetTemp()const;
    uint16_t GetChannel()const;
private:
    uint16_t    m_nTmp;
    uint16_t    m_nCh;
};

class StoveHeatAction : public ActionAbstrctItem
{
public:
    StoveHeatAction(uint8_t ch=0, float tmpBeg = 0, float tmpDst = 0, float min=0, int16_t seq = -1);
    QString ToString(bool bStart = true)const override;

    void Save(QDataStream *dstr)override;
    void Load(QDataStream *dstr)override;
    void Distribute()override;
    float UpMins()const;
    float UpSeconds()const;
    float GetBegTemperature()const;
    float GetDstTemperature()const;
    uint16_t GetChannel()const;
private:
    uint16_t    m_nCh;
    float     m_tmpBeg;
    float     m_tmpDst;
    float       m_upMin;
};
#endif // !__CtrlAction_H__
