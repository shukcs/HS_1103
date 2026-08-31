#include "FeederStruct.h"
#include <QApplication>
#include <QDataStream>
#include <QFile>

#include "FeederMgr.h"
#include "RobotMgr.h"
#include "strDecoder/strdecoder.h"
#pragma execution_character_set("utf-8")
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

void StoreStruct::setStat(StoreStat s, bool bRvr)
{
	if (s != stat)
    {
		stat = s;
        if (bRvr && FeederMgr::Instance().getStore(numb))
            FeederRecover::Instance().AddStore(*this);
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

void BottleStruct::setFlag(BottleStat f, bool bRvr)
{
    if (f != m_flag)
    {
        bool b = B_CanUse == m_flag || B_CanUse == f;
        m_flag = f;

        if (!bRvr)
        {
            FeederRecover::Instance().AddBottle(*this);
            emit FeederMgr::Instance().bottleChanged(this);
            if (b)
                emit FeederMgr::Instance().canUsedTubeChanged();
        }
    }
}

/*
* FeederParam
*/
FeederParam::FeederParam(const QList<QPair<int, float> > &feeds, uint16_t numb, uint16_t nTube) : m_numbBottle(numb)
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

uint16_t FeederParam::getTubeNumb() const
{
    return m_numbTube;
}

TubeStruct *FeederParam::getTube() const
{
    return FeederMgr::Instance().getTube(m_numbTube);
}

const QList<FeederParam::FeedItem> & FeederParam::feedMaterial() const
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

void TubeStruct::setFlag(TubeStat f, bool bRvr)
{
    if (m_flag != f)
    {
        bool b = T_WaitPrepare == m_flag || T_WaitPrepare == f;
        m_flag = f;
        if (!bRvr)
        {
            emit FeederMgr::Instance().tubeChanged(this);
            FeederRecover::Instance().AddTube(*this);
            if (b)
                emit FeederMgr::Instance().canUsedTubeChanged();
        }
    }
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

DeviceAct::DeviceAct(float tmWait) : type(Dev_NextWait), fWaitTime(tmWait), bStart(false), bWaitFinish(true)
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

QString DeviceAct::ToString(bool b) const
{
    auto str = b ? QApplication::translate("DeviceAct", "开始") : QApplication::translate("DeviceAct", "完成");
    switch (type)
    {
    case Dev_Robot:
        return QApplication::translate("DeviceAct", "机械臂") + RobotMgr::actionDescribe((RobotMgr::RobotAction)robotStep, robotIndex) + str;
    case Dev_Feeder:
        if (!b)
            break;
        if (auto c = cmdFeeder == 104 ? FeederMgr::Instance().GetStore(idStore) : nullptr)
            return  QApplication::translate("DeviceAct", "进料器") + FeederMgr::CmdDescrib(cmdFeeder) + (c->pMate ? c->pMate->name : QString());
        return  QApplication::translate("DeviceAct", "进料器") + FeederMgr::CmdDescrib(cmdFeeder);
    case Dev_Servo:
        return QApplication::translate("DeviceAct", "伺服电机执行")+ FeederMgr::ServoPosDescrib((FeederMgr::RobotPostion)servoPos)+str;
    case Dev_StepMotor:
        return stepMotorActToString() + str;
    case Dev_NextWait:
        if (!b)
            break;
        return QApplication::translate("DeviceAct", "等待%1秒").arg(fWaitTime);
    default:
        break;
    }
    return QString();
}

QString DeviceAct::stepMotorActToString() const
{
    switch ((CtrlType::StepMotorType)stepType)
    {
    case CtrlType::Motor_Tube:   ///反应管上下电机
        return stepDirCont ? QApplication::translate("DeviceAct", "反应管上升") : QApplication::translate("DeviceAct", "反应管下降");
    case CtrlType::Motor_Stove:  ///炉膛开合电机
        return stepDirCont ? QApplication::translate("DeviceAct", "炉膛打开") : QApplication::translate("DeviceAct", "炉膛闭合");
    default:
        break;
    }
    return QString();
}

/*
*FeederRecover::RecoverItem
*/
FeederRecover::RecoverItem::RecoverItem(uint16_t len, RvcType type, uint32_t id, uint32_t offset)
:_len(len), _type(type), _id(id), _offset(offset)
{
}
/*
*FeederRecover
*/
FeederRecover::FeederRecover(const QString &file) : m_mapFile(new QFile(file))
{
    if (m_mapFile->open(QIODevice::ReadWrite))
    {
        auto sz = m_mapFile->size();
        m_buff = m_mapFile->map(0, m_mapFile->size());
        if (sz >= 6)
        {
            m_size = *(uint32_t*)m_buff;
            m_curAct = *(uint16_t*)(m_buff+4);
        }
        
        if (m_size < 6)
            setSize(6);

        for (uint32_t i = 6; i < m_size -4; )
        {
            RecoverItem item;
            item._len = *(uint16_t*)(m_buff + i);
            item._offset = i;
            if (item._len < 6)
            {
                setSize(i);
                break;
            }
            item._type = *(uint16_t*)(m_buff + i + 2);
            item._id = *(uint16_t*)(m_buff + i + 4);
            i += item._len;
            m_items << item;
        }
    }
}

FeederRecover::~FeederRecover()
{
    m_mapFile->close();
    delete m_mapFile;
}

void FeederRecover::Clear()
{
    m_items.clear();
    m_curAct = 0;
    m_mapFile->resize(6);
}

const QList<FeederRecover::RecoverItem> & FeederRecover::AllRecoverItems() const
{
    return m_items;
}

void FeederRecover::FeedJobStart(uint16_t actSz)
{
    if (m_buff && m_curAct != actSz)
        *(uint16_t*)(m_buff + 4) = actSz;

    m_curAct = actSz;
}

void FeederRecover::FeedActionDone()
{
    m_curAct--;
    if (m_buff)
        memcpy(m_buff + 4, &m_curAct, 2);
}

void FeederRecover::AddBottle(const BottleStruct &bt)
{
    auto itr = getItem(R_Bottle, bt.m_numb);
    if (itr != m_items.end())
    {
        writeData(&bt, *itr);
    }
    else if (bt.getFlag() > B_CanUse)
    {
        RecoverItem item(8, R_Bottle, bt.m_numb, m_size);
        m_items << item;
        if (!setSize(m_size + m_items.last()._len))
            writeData(&bt, item);
    }
}

void FeederRecover::RecoverBottle(BottleStruct *bt)
{
    if (!bt || !m_buff)
        return;

    auto itr = getItem(R_Bottle, bt->m_numb);
    if (itr != m_items.end())
    {
        auto tmp = *(uint16_t*)(m_buff+itr->_offset+6);
        bt->setFlag((BottleStat)tmp, true);
    }
}

void FeederRecover::AddTube(const TubeStruct &tb)
{
    auto itr = getItem(R_Tube, tb.getNumber());
    if (itr != m_items.end())
    {
        writeData(&tb, *itr);
    }
    else if (tb.getFlag() > T_WaitPrepare)
    {
        m_items << RecoverItem(8, R_Tube, tb.getNumber(), m_size);
        if (!setSize(m_size + m_items.last()._len))
            writeData(&tb, m_items.last());
    }
}

void FeederRecover::RecoverTube(TubeStruct *bt)
{
    if (!bt || !m_buff)
        return;

    auto itr = getItem(R_Tube, bt->getNumber());
    if (itr != m_items.end())
    {
        auto tmp = *(uint16_t*)(m_buff + itr->_offset + 6);
        bt->setFlag((TubeStat)tmp, true);
    }
}

void FeederRecover::AddStore(const StoreStruct &st)
{
    auto itr = getItem(R_Store, st.numb);
    if (itr != m_items.end())
    {
        writeData(&st, *itr);
    }
    else if (st.getStat() > S_CanFeed)
    {
        m_items << RecoverItem(18, R_Store, st.numb, m_size);
        if (!setSize(m_size + m_items.last()._len))
            writeData(&st, m_items.last());
    }
}

void FeederRecover::RecoverStore(StoreStruct *bt)
{
    if (!bt || !m_buff)
        return;

    auto itr = getItem(R_Store, bt->numb);
    if (itr != m_items.end())
    {
        if (bt->nfcid.isEmpty())
        {
            auto tmp = *(uint16_t*)(m_buff + itr->_offset + 8);
            if (tmp > 8)
                tmp = 8;

            QByteArray arr(tmp, 0);
            memcpy(arr.data(), m_buff + itr->_offset + 10, tmp);
            bt->nfcid = arr.toHex();
        }
        auto tmp = *(uint16_t*)(m_buff + itr->_offset + 6);
        bt->setStat((StoreStat)tmp, true);
    }
}

void FeederRecover::AddFeedParam(const FeederParam &pr)
{
    auto itr = getItem(R_Param, pr.getBottleNumb());
    int len = pr.feedMaterial().size() * 6 + 10;
    if (itr!=m_items.end())
    {
        int diff = len - itr->_len;
        if (diff>0)
        {
            itr->_len = len;
            if (!setSize(m_size + diff))
                writeItem(itr - m_items.begin());
        }
        else
        {
            writeData(&pr, *itr);
        }
    }
    else
    {
        m_items << RecoverItem(len, R_Param, pr.getBottleNumb(), m_size);
        if (!setSize(m_size + len))
            writeData(&pr, m_items.last());
    }
}

void FeederRecover::Removed(const FeederParam &pr)
{
    auto itr = getItem(R_Param, pr.getBottleNumb());
    if (itr != m_items.end())
    {
        auto idx = itr - m_items.begin();
        m_items.erase(itr);
        writeItem(idx);
    }
}

void FeederRecover::RecoverFeedParam(QList<FeederParam> &feeds)
{
    feeds.clear();
    for (auto itr : m_items)
    {
        if (R_Param != itr._type)
            continue;

        auto nTmp = *(uint16_t*)(m_buff + itr._offset + 8);
        QList<FeederParam::FeedItem> its;
        for (int i = 0; i < nTmp; ++i)
        {
            auto offset = itr._offset + 10 + i * 6;
            auto id = *(uint16_t*)(m_buff + offset);
            offset += 2;
            auto w = *(uint16_t*)(m_buff + offset);
            its << FeederParam::FeedItem(id, w);
        }
        nTmp = *(uint16_t*)(m_buff + itr._offset + 6);
        feeds << FeederParam(its, itr._id, nTmp);
    }
}

void FeederRecover::AddJobs(const QList<int> &jobs)
{
    auto itr = getItem(R_Jobs, 0);
    auto len = jobs.size() * sizeof(int32_t) + 6;
    if (itr != m_items.end())
    {
        int diff = len - itr->_len;
        if (diff > 0)
        {
            itr->_len = len;
            if (!setSize(m_size + diff))
                writeItem(itr - m_items.begin());
        }
        else
        {
            writeData(jobs, *itr);
        }
    }
    else
    {
        m_items << RecoverItem(len, R_Jobs, jobs.size(), m_size);
        if (!setSize(m_size + m_items.last()._len))
            writeData(jobs, m_items.last());
    }
}

void FeederRecover::RecoverJobs(QList<int> &feeds)
{
    auto itr = getItem(R_Jobs, 0);
    if (itr != m_items.end())
    {
        feeds.clear();
        for (int i = 0; i < itr->_id; ++i)
        {
            auto offset = itr->_offset + 6 + i * sizeof(int32_t);
            int32_t t = 0;
            memcpy(&t, m_buff + offset, sizeof(int32_t));
            feeds << t;
        }
    }
}

uint16_t FeederRecover::GetRemainActions() const
{
    return m_curAct;
}

void FeederRecover::Save()
{
    m_mapFile->flush();
}

FeederRecover & FeederRecover::Instance()
{
    static FeederRecover sRcv = FeederMgr::AppDir("user") + "/recover";
    return sRcv;
}

bool FeederRecover::setSize(uint32_t sz)
{
    if (m_size!=sz && m_mapFile)
    {
        auto bf = m_buff;
        if (m_mapFile->size() < sz)
        {
            auto nsz = (sz / 2048 + 1) * 2048;
            m_mapFile->resize(nsz);
            bf = m_mapFile->map(0, nsz);
            memcpy(bf + 4, &m_curAct, sizeof(uint16_t));
        }

        if (bf!= m_buff)
        {
            m_buff = bf;
            if (bf != m_buff)
            {
                writeItem(0);
                m_mapFile->flush();
                return true;
            }
        }
        memcpy(m_buff, &sz, sizeof(uint32_t));
        m_size = sz;
    }
    return false;
}

QList<FeederRecover::RecoverItem>::iterator FeederRecover::getItem(RvcType type, uint32_t id)
{
    for (auto itr = m_items.begin(); itr != m_items.end(); ++itr)
    {
        auto &tmp = *itr;
        if (tmp._type == type && (tmp._id==id||R_Jobs==type))
            return itr;
    }
    return m_items.end();
}

void FeederRecover::writeItem(int32_t from)
{
    uint32_t offset = 6;
    for (auto &itr : m_items)
    {
        if (from > 0)
        {
            offset += itr._len;
            from--;
            continue;
        }
        itr._offset = offset;
        auto &ist = FeederMgr::Instance();
        switch (itr._type)
        {
        case R_Bottle:
            offset += writeData(ist.getBottle(itr._id), itr); break;
        case R_Tube:
            offset += writeData(ist.getTube(itr._id), itr); break;
        case R_Store:
            offset += writeData(ist.getStore(itr._id), itr); break;
        case R_Param:
            offset += writeData(ist.GetfeedParamsByBottleNum(itr._id), itr); break;
        case R_Jobs:
            offset += writeData(ist.m_jobs, itr); break;
        default:
            break;
        }
    }
    memcpy(m_buff, &offset, sizeof(m_buff));
}

uint16_t FeederRecover::writeData(const BottleStruct *bt, const RecoverItem &r)
{
    if (bt && m_buff && m_size >= r._len + r._offset && r._len >=8)
    {
        auto offset = r._offset + writeBase(r);
        *(uint16_t*)(m_buff + offset) = bt->getFlag();
        return r._len;
    }
    return 0;
}

uint16_t FeederRecover::writeData(const TubeStruct *bt, const RecoverItem &r)
{
    if (bt && m_buff && m_size >= r._len + r._offset && r._len >= 8)
    {
        auto offset = r._offset + writeBase(r);
        *(uint16_t*)(m_buff + offset) = bt->getFlag();
        return r._len;
    }
    return 0;
}

uint16_t FeederRecover::writeData(const StoreStruct *st, const RecoverItem &r)
{
    if (st && m_buff && m_size>=r._len+r._offset && r._len>=18)
    {
        auto offset = r._offset + writeBase(r);
        *(uint16_t*)(m_buff+offset) = st->getStat();
        offset += 2;
        auto arr = QByteArray::fromHex(st->nfcid.toLatin1());
        if (arr.size() > 8)
            arr = arr.right(8);
        offset += 2;
        *(uint16_t*)(m_buff + offset) = arr.size();
        offset += 2;
        memcpy(m_buff + offset, arr.data(), arr.size());
        return r._len;
    }
    return 0;
}

uint16_t FeederRecover::writeData(const FeederParam *pr, const RecoverItem &r)
{
    auto len = pr->feedMaterial().size() * 6 + 10;
    if (pr && m_buff && m_size >= r._len + r._offset && r._len>=len)
    {
        auto offset = r._offset + writeBase(r);
        *(uint16_t*)(m_buff + offset) = pr->getTubeNumb();
        offset += 2;
        *(uint16_t*)(m_buff + offset) = pr->feedMaterial().size();
        offset += 2;
        for (auto &itr : pr->feedMaterial())
        {
            *(uint16_t*)(m_buff + offset) = itr.first;
            offset += 2;
            *(float*)(m_buff + offset) = itr.second;
            offset += sizeof(float);
        }
        return r._len;
    }
    return 0;
}

uint16_t FeederRecover::writeData(const QList<int> &jobs, const RecoverItem &r)
{
    auto sz = jobs.size();
    if (sz==r._id && m_buff && m_size >= r._len + r._offset && r._len >= 6 + sz * 4)
    {
        auto offset = r._offset + writeBase(r);
        for (auto itr : jobs)
        {
            memcpy(m_buff + offset, &itr, sizeof(int32_t));
            offset += sizeof(int32_t);
        }
        return r._len;
    }
    return 0;
}

uint16_t FeederRecover::writeBase(const RecoverItem &r)
{
    *(uint16_t*)(m_buff + r._offset) = r._len;
    *(uint16_t*)(m_buff + r._offset+2) = r._type;
    *(uint16_t*)(m_buff + r._offset+4) = r._id;
    return 6;
}
