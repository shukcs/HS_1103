#include "FeederDecoder.h"

/*
* MaterialStruct
*/
MaterialStruct::MaterialStruct(const QString &nfc, const QString &name/*=QString()*/, float w/*=0.0f*/)
: name(name), nfcid(nfc), weight(w)
{
}

/*
* StoreStruct
*/
StoreStruct::StoreStruct(int num, const MaterialStruct *m, const QString &id) : numb(num),
pMate(m), nfcid(id)
{
}

StoreStat StoreStruct::getStat() const
{
	return stat;
}

void StoreStruct::setStat(StoreStat s)
{
	if (s != stat)
	{
		stat = s;
		emit FeederMgr::Instance().storeChanged(this);
	}
}
/*
* BottleStruct
*/
BottleStruct::BottleStruct(uint16_t num, uint16_t flag, uint16_t stWork) : m_numb(num)
, m_flag(flag), m_stWork(stWork)
{
}

BottleStat BottleStruct::getFlag() const
{
    return (BottleStat)m_flag;
}

void BottleStruct::setFlag(BottleStat f)
{
    if (f != m_flag)
    {
        bool b = B_CanUse == m_flag || B_CanUse == f;
        m_flag = f;
        emit FeederMgr::Instance().bottleChanged(this);
        if (b)
            emit FeederMgr::Instance().canUsedTubeChanged();
    }
}

/*
* FeederParam
*/
FeederParam::FeederParam(const QList<QPair<int, float> > &feeds, uint16_t numb, int16_t nTube) : m_numbBottle(numb)
, m_feedMaterials (feeds), m_numbTube(nTube)
{
}

int16_t FeederParam::getBottleNumb() const
{
	return m_numbBottle;
}

BottleStruct *FeederParam::getBottle() const
{
    return FeederMgr::Instance().getBottle(m_numbBottle);
}

int16_t FeederParam::getTubeNumb() const
{
    return m_numbTube;
}

TubeStruct *FeederParam::getTube() const
{
    return FeederMgr::Instance().getTube(m_numbTube);
}

const QList<QPair<int, float> > & FeederParam::feedMaterial() const
{
    return m_feedMaterials;
}

void FeederParam::getFeedNameAndWeight(QList<QPair<QString, float> > *ret) const
{
    if (!ret)
        return;

    auto &feeder = FeederMgr::Instance();
    ret->clear();
    for (auto &itr : m_feedMaterials)
    {
        auto c = feeder.GetStore(itr.first);
        if (auto m = c ? c->pMate : nullptr)
            *ret << QPair<QString, float>(m->name, itr.second);
    }
}

void FeederParam::feederFinish(int type) const
{
    auto &feeder = FeederMgr::Instance();
    if (auto tb = feeder.getTube(m_numbTube))
    {
		TubeStat st = T_Prepared;
		switch (type)
		{
		case FeederMgr::J_StoveFixTube:
			st = T_Fixed; break;
		case FeederMgr::J_StoveTubeBack:
			st = T_Recyced; break;
		default:
			break;
		}
        tb->setFlag(st);
        if (auto bt = FeederMgr::J_PrepareMate==type ? getBottle() : nullptr)
            bt->setFlag(B_Used);
    }
}

/*
* FeederParam
*/
TubeStruct::TubeStruct(uint16_t n, uint16_t f)
: m_numb(n), m_flag(f)
{
}

TubeStat TubeStruct::getFlag() const
{
    return (TubeStat)m_flag;
}

void TubeStruct::setFlag(TubeStat f)
{
    if (m_flag != f)
    {
        bool b = B_CanUse == m_flag || B_CanUse == f;
        m_flag = f;
        emit FeederMgr::Instance().tubeChanged(this);

        if (b)
            emit FeederMgr::Instance().canUsedTubeChanged();
    }
    m_flag = f;
}

uint16_t TubeStruct::getNumber() const
{
    return m_numb;
}

int TubeStruct::getStoveCh() const
{
    return m_chStove;
}

void TubeStruct::setStoveCh(int8_t ch)
{
    m_chStove = ch;
}

/*
* DeviceAct
*/
DeviceAct::DeviceAct(DeviceType t, bool bWait) : type(t), bStart(false), bWaitFinish(bWait)
{
}

DeviceAct::DeviceAct(float tmWait) : type(Dev_NextWait), fWaitTime(tmWait), bStart(false), bWaitFinish(false)
{
}

void DeviceAct::SetFeedCmd(uint16_t cmd, int ack /*= -1*/)
{
    if (type == Dev_Feeder)
    {
        cmdFeeder = cmd;
        cmdAck = ack < 0 ? cmd + 1 : ack;
    }
}