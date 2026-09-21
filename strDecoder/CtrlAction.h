#ifndef __CtrlAction_H__
#define __CtrlAction_H__
#include "common/ActionItem.h"

class AirClrAction : public ActionAbstrctItem
{
public:
    AirClrAction(uint16_t ch=0, float prs=0, uint16_t sec=0, int16_t seq=-1);

    QString ToString(bool bStart = true)const override;
    void Save(QDataStream *dstr, bool bSaveStat=false);
    void Load(QDataStream *dstr);
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
    void Save(QDataStream *dstr, bool bSaveStat = false);
    void Load(QDataStream *dstr);
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

    void Save(QDataStream *dstr, bool bSaveStat = false);
    void Load(QDataStream *dstr);
private:
    uint16_t    m_ch;
};

class LiquidInAction : public ActionAbstrctItem
{
public:
    LiquidInAction(uint16_t ch = 0, float mlPmin = 0, uint16_t sec = 0, int16_t seq = -1);
    QString ToString(bool bStart = true)const override;

    void Save(QDataStream *dstr, bool bSaveStat = false);
    void Load(QDataStream *dstr);
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

    void Save(QDataStream *dstr, bool bSaveStat = false);
    void Load(QDataStream *dstr);
private:
    uint16_t    m_ch;
};
#endif // !__CtrlAction_H__
