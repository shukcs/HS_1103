#include "FeederDecoder.h"
#include <QSerialPort>
#include <QApplication>
#include <QDateTime>
#include <QTimer>
#include <QApplication>
#include <QSettings>
#include <QDir>
#include <QFileInfo>
#include "RobotMgr.h"
#include "strDecoder/strdecoder.h"
#pragma execution_character_set("utf-8")

#define ModBussAddr 1
#define ConnetTimeOut 4000 ///4s收不到数据超时
#define MaterialMaxNum 32

union DataU
{
    uint8_t cD[4];
    float   fD;
    uint16_t u16D;
    uint32_t u32D;
};

struct WorkItem {
    uint8_t type;
    uint8_t nNumTube;
    uint8_t chStove; ///
    uint8_t index;   ///归还位 type=FeederMgr::J_StoveTubeBack有效
	static WorkItem initFrom(FeederMgr::JobType t, uint8_t n, uint8_t ch = 0, uint8_t idx = 0);
	static WorkItem initFrom(int n);
	int toInt()const;
};

WorkItem WorkItem::initFrom(int n)
{
	WorkItem ret = *(WorkItem*)&n;
	return ret;
}

WorkItem WorkItem::initFrom(FeederMgr::JobType t, uint8_t n, uint8_t ch /*= 0*/, uint8_t idx /*= 0*/)
{
	WorkItem ret = { (uint8_t)t, n, ch, idx };
	return ret;
}

int WorkItem::toInt() const
{
	int ret = *(int*)this;
	return ret;
}

/*
* FeederMgr
*/
FeederMgr::FeederMgr(QObject* p) : QObject(p)
, m_port(new QSerialPort(qApp))
{
    connect(m_port, &QSerialPort::readyRead, this, &FeederMgr::readByets);
    connect(m_port, &QSerialPort::errorOccurred, this, [=](QSerialPort::SerialPortError err) {
        emit serialPortError(err != QSerialPort::NoError);
    });
    m_idTimer = startTimer(2000);
    m_idRead = startTimer(500);
    m_flag = Flag_DoReadMaterial|Flag_DoReadStore;

    QSettings settings(DefaultConfigFile(), QSettings::IniFormat);
    settings.beginGroup("FeederPort");
    m_portName = settings.value("port", "COM3").toString();
    auto baut = settings.value("baut", 9600).toInt();
    settings.endGroup();
    settings.beginGroup("FeederConfig");
    m_port->setPortName(m_portName);
    m_port->setBaudRate(baut);
    m_port->setDataBits(QSerialPort::Data8);
    m_port->setParity(QSerialPort::NoParity);
    m_port->setStopBits(QSerialPort::OneStop);
    connect(m_port, &QSerialPort::baudRateChanged, this, [=] {m_bPortChaned = true; });
    m_port->open(QSerialPort::ReadWrite);
    QTimer::singleShot(50, this, &FeederMgr::ConnectPort);
    writeFunc(406);
	m_nBottle = settings.value("nBottle", 6).toInt();
    for (int i = 0; i < m_nBottle; ++i)
    {
        m_allBottle << new BottleStruct(i);
    }
    m_nTube = settings.value("nTube", 8).toInt();
    for (int i = 0; i < m_nTube; ++i)
    {
        m_allTube << new TubeStruct(i);
    }
    m_nStoreNum = settings.value("StoreNum", 6).toInt();
    settings.endGroup();
    ConnectPort();
    readMaterials();
}

FeederMgr::~FeederMgr()
{
    qDeleteAll(m_allMaterials);
    qDeleteAll(m_allStore);
    qDeleteAll(m_allBottle);
    qDeleteAll(m_allTube);
}

FeederMgr& FeederMgr::Instance()
{
    static FeederMgr s_ins(nullptr);
    return s_ins;
}

uint16_t FeederMgr::Modbus_crc16(const uint8_t* buff, uint16_t len)
{
    unsigned short crc = 0xffff;
    while (len--)
    {
        crc = crc ^ (*buff++);
        for (int i = 0; i < 8; ++i)
        {
            bool b = (crc & 1) == 1;
            crc >>= 1;
            if (b)
                crc ^= 0xA001;
        }
    }

    return (crc >> 8) | (crc << 8);
}

bool FeederMgr::Equal(double f1, double f2)
{
    return fabs(f1 - f2) < 0.000001;
}

void FeederMgr::AddModbusFloat(uint8_t* buff, float f)
{
    DataU tmp;
    tmp.fD = f;
    buff[0] = tmp.cD[2];
    buff[1] = tmp.cD[3];
    buff[2] = tmp.cD[0];
    buff[3] = tmp.cD[1];
}

void FeederMgr::AddModbusData(uint8_t* buff, uint16_t u)
{
    DataU tmp;
    tmp.u16D = u;
    buff[0] = tmp.cD[1];
    buff[1] = tmp.cD[0];
}

void FeederMgr::AddModbusU32(uint8_t* buff, uint32_t u)
{
    DataU tmp;
    tmp.u32D = u;
    buff[0] = tmp.cD[3];
    buff[1] = tmp.cD[2];
    buff[2] = tmp.cD[1];
    buff[3] = tmp.cD[0];
}

float FeederMgr::PichModbusFloat(const void* src)
{
    auto buff = (const uint8_t*)src;
    DataU tmp;
    tmp.cD[2] = buff[0];
    tmp.cD[3] = buff[1];
    tmp.cD[0] = buff[2];
    tmp.cD[1] = buff[3];

    return tmp.fD;
}

uint16_t FeederMgr::PichModbusU16(const void* src)
{
    auto buff = (const uint8_t*)src;
    DataU tmp;
    tmp.cD[0] = buff[1];
    tmp.cD[1] = buff[0];

    return tmp.u16D;
}

uint32_t FeederMgr::PichModbusU32(const void* src)
{
    auto buff = (const uint8_t*)src;
    DataU tmp;
    tmp.cD[0] = buff[3];
    tmp.cD[1] = buff[2];
    tmp.cD[2] = buff[1];
    tmp.cD[3] = buff[0];

    return tmp.u32D;
}

FeederMgr::RobotPostion FeederMgr::getStovePos(int ch)
{
    switch (ch)
    {
    case 0:
        return Pos_Stove1;
    case 1:
    default:
        break;
    }
    return Pos_None;
}

QString FeederMgr::DefaultConfigFile()
{
    auto iniFile = QFileInfo(QCoreApplication::applicationDirPath() + "/user/config.ini");
    auto dir = QFileInfo(iniFile).dir();
    if (!dir.exists())
        dir.mkdir(dir.absolutePath());
    QFile f(iniFile.absoluteFilePath());
    if (!f.exists())
    {
        f.open(QIODevice::WriteOnly);
        f.close();
    }
    return iniFile.absoluteFilePath();
}

void FeederMgr::ConnectPort()
{
    if (!m_port->isOpen())
    {
        if (m_comStat != PortClose)
            emit connectStatChanged(PortClose);
        m_comStat = PortClose;
        m_portName = m_port->portName();
    }
    else
    {
        if (m_portName!=m_port->portName() || m_bPortChaned)
        {
            m_portName = m_port->portName();
            QSettings settings(DefaultConfigFile(), QSettings::IniFormat);
            settings.beginGroup("FeederPort");
            settings.setValue("port", m_portName);
            settings.setValue("baut", (int)m_port->baudRate());
            settings.endGroup();
        }
        if (PortClose == m_comStat)
        {
            m_comStat = NoData;
            emit connectStatChanged(m_comStat);
        }
    }
}

TubeStruct * FeederMgr::GetInSotveTube(uint8_t ch) const
{
    for (auto &itr : m_feedParams)
    {
        if (auto tb = itr.getTube())
        {
            if (T_Fixed==tb->getFlag() && tb->getStoveCh()==ch)
                return tb;
        }
    }
    return nullptr;
}

void FeederMgr::OnRobotDone(int typeRobot)
{
    bool bDoNext = false;
    auto itr = m_actions.begin();
    for (; itr != m_actions.end(); ++itr)
    {
        if (Dev_Robot == itr->type && itr->robotStep==typeRobot)
        {
            bool bChange = RobotMgr::MoveStore == typeRobot || RobotMgr::StoreBack == typeRobot;
            if (auto c = bChange ? getStore(itr->robotIndex) : nullptr)
                c->setStat(RobotMgr::MoveStore == typeRobot ? S_WaitFeed : S_CanFeed);

            m_actions.erase(itr);
            bDoNext = true;
            break;
        }
        if (itr->bWaitFinish)
            return;
    }

    if (bDoNext)
        doAction();
}

void FeederMgr::OnServoMotor(int pos, bool bReached)
{
    if (!bReached)
        return;

    bool bDoNext = false;
    auto itr = m_actions.begin();
    for (; itr != m_actions.end(); ++itr)
    {
        if (Dev_Servo==itr->type && itr->servoPos==pos)
        {
            itr = m_actions.erase(itr);
            bDoNext = true;
            break;
        }
        if (itr->bWaitFinish)
            return;
    }
    if (bDoNext)
        doAction();
}

void FeederMgr::OnStepMotor(StepMotorStat* st)
{
    if (st->IsRun())
        return;

    bool bDoNext = false;
    auto itr = m_actions.begin();
    for (; itr != m_actions.end(); ++itr)
    {
        if (Dev_StepMotor != itr->type || itr->stepType != st->GetType() || itr->stepCh != st->GetChannel())
            continue;

        bool bReached = itr->stepDirCont ? st->IsLimitH() : st->IsLimitL();
        if (bReached)
        {
            itr = m_actions.erase(itr);
            bDoNext = true;
            break;
        }
        if (itr->bWaitFinish)
            return;
    }
    if (bDoNext)
        doAction();
}

void FeederMgr::readByets()
{
    if (m_port->bytesAvailable() < 1)
        return;

    m_buff += m_port->readAll();
    auto msg = pickMsg();
    bool bRcv = false;
    if (!msg.isEmpty())
    {
        decode(msg);
        bRcv = true;
    }

    if (bRcv)
    {
        if (m_comStat != Communicate)
        {
            m_comStat = Communicate;
            emit connectStatChanged(m_comStat);
        }
        m_lastTmRcv = QDateTime::currentMSecsSinceEpoch();
    }
}

bool FeederMgr::prcsRead(uint16_t addr, const QByteArray& msg)
{
    if (addr >= 600 && addr < 600 * 32 * 16)
    {
        auto idx = (addr - 600) / 16;
        MaterialStruct m(msg.mid(3, 8).toHex().toUpper());
        if (m.nfcid=="0000000000000000")
        {
            m_flag &= ~Flag_DoReadMaterial;
            return false;
        }

        m.name = QString::fromStdString(std::string(msg.data() + 11, 10).c_str());
        m.type = PichModbusFloat(msg.data() + 21);
        m.usedRate = PichModbusFloat(msg.data() + 23);
        m.weight = PichModbusFloat(msg.data() + 27);
        memcpy(&m.ch, msg.data() + 31, 2);

        addMaterial(m, false);
		if (idx + 1 < MaterialMaxNum)
		{
			readMaterials(idx + 1);
			return true;
		}
	}
	return false;
}

bool FeederMgr::prcsStat(uint16_t addr, const QByteArray& msg)
{
	bool ret = false;
    switch (addr)
    {
    case 103:
        if (m_nStoreNum >0 && m_nStoreNum < 12 && PichModbusU16(msg.data() + 3) != m_nStoreNum)
			writeFunc(402, m_nStoreNum);
        m_flag |= Flag_StoreNumRead;
		readStore();
		ret = true;
		break;
    case 105:
        ret = prcsFeederStat(PichModbusU16(msg.data() + 3));
		break;
    case 106:
        if (0==PichModbusU16(msg.data()+3))
        {
            m_flag |= Flag_CanReadStore;
            readStore();
			ret = true;
        }
        break;
    case 107:
        if (0 == PichModbusU16(msg.data() + 3))
        {
            m_flag |= Flag_CanReadMaterial;
            readMaterials();
			ret = true;
        }
    case 108:
        emit feedingChanged(PichModbusFloat(msg.data() + 3), false);
        break;
    default:
        if (138 <= addr && addr < 138 + 4 * m_nStoreNum)
        {
            uint32_t idx = (addr - 138) / 4;
            QString nfcif(QByteArray(msg.data() + 3, 8).toHex());
            bool b = nfcif != "0100000000000000" && nfcif != "0000000000000000";
            updateStore(b ? nfcif.toUpper() : QString(), idx);
			if (idx+1 < m_nStoreNum)
			{
				readStore(idx + 1);
				ret = true;
			}
        }
        break;
    }
	return ret;
}

bool FeederMgr::prcsWriteCmd(uint16_t addr, uint16_t cmd)
{
	if (2000 != addr)
		return false;

	readFeedStat();
	return true;
}

void FeederMgr::checkMatesCanFeed()
{
    QStringList ret;
    for (auto& itr : m_allMaterials)
    {
        if (GetStore(itr->nfcid))
            ret << itr->name;
    }
    ret.removeDuplicates();

    if (m_canFeedMatesNames != ret)
    {
        m_canFeedMatesNames = ret;
        emit matesCanFeedChanged();
    }
}

void FeederMgr::sumFeederWeight(QMap<int, float> *feeds) const
{
    if (!feeds)
        return;

    QMap<int, float> &ref = *feeds;
    for (auto &fb : m_feedParams)
    {
        for (auto &it : fb.feedMaterial())
        {
            auto itT = ref.find(it.first);
            if (itT == ref.end())
                ref[it.first] = it.second;
            else
                ref[it.first] += it.second;
        }
    }
}

bool FeederMgr::prcsFeederStat(uint16_t stat)
{
    bool bDoNext = false;
    for (auto itr = m_actions.begin(); itr != m_actions.end(); ++itr)
    {
        if (Dev_Feeder == itr->type && itr->cmdAck == stat)
        {
			if (auto c = itr->cmdFeeder == 104 ? getStore(itr->idStore) : nullptr)
            {
                c->setStat(S_Feeded);
                m_feedStore = nullptr;
            }
            m_actions.erase(itr);
            bDoNext = true;
            break;
        }
        if (itr->bWaitFinish)
            return false;
    }
    if (bDoNext)
        return doAction();

	return false;
}

bool FeederMgr::doAction()
{
    checkActions();
    if (m_actions.isEmpty())
        return false;

	bool bWaiNext = false;
	bool ret = false;
    for (auto &act : m_actions)
    {
        if (act.bStart || bWaiNext)
        {
            bWaiNext = (act.type == Dev_NextWait);
            if (act.bWaitFinish)
                break;
            continue;
        }

        switch (act.type)
        {
        case Dev_Robot:
        case Dev_Servo:
        case Dev_StepMotor:
            emit actionRun(&act);
            break;
        case Dev_Feeder:
			writeCmd(act);
			ret = true;
			break;
        case Dev_NextWait:
            bWaiNext = true;
            QTimer::singleShot(act.fWaitTime*1000, this, &FeederMgr::onWait);
            break;
        }
        act.bStart = true;
        if (act.bWaitFinish)
            break;
    }
	return ret;
}

StoreStruct* FeederMgr::getStore(const QString& id)const
{
    if (id.isEmpty())
        return nullptr;

    for (auto itr : m_allStore)
    {
        if (id == itr->nfcid)
            return itr;
    }
    return nullptr;
}

StoreStruct* FeederMgr::getStore(uint16_t num) const
{
    for (auto itr : m_allStore)
    {
        if (num == itr->numb)
            return itr;
    }
    return nullptr;
}

MaterialStruct* FeederMgr::getMaterial(const QString& id)const
{
    if (id.isEmpty())
        return nullptr;

    for (auto itr : m_allMaterials)
    {
        if (itr->nfcid == id)
            return itr;
    }
    return NULL;
}

const FeederParam * FeederMgr::GetfeedParamsByBottleNum(int numb) const
{
    for (auto &itr : m_feedParams)
    {
        if (itr.getBottleNumb() == numb)
            return &itr;
    }
    return nullptr;
}

const FeederParam *FeederMgr::getfeedParamsByTube(int numb) const
{
    for (auto &itr : m_feedParams)
    {
        if (itr.getTubeNumb() == numb)
            return &itr;
    }
    return nullptr;
}

StoreStruct* FeederMgr::getPropStore(float weight, const QString& name, const QMap<int, float> &preDistrs)const
{
    for (auto& itr : m_allStore)
    {
        if (!itr->pMate || itr->pMate->name != name)continue;

        auto itTm = preDistrs.find(itr->numb);
        auto preDist = itTm == preDistrs.end() ? 0 : itTm.value();
        if (itr->pMate->weight - preDist >= weight)
            return itr;
    }
    return nullptr;
}

BottleStruct * FeederMgr::getBottle(int numb) const
{
    for (auto itr : m_allBottle)
    {
        if (itr->m_numb == numb)
            return itr;
    }
    return nullptr;
}

void FeederMgr::onWait()
{
    bool bDoNext = false;
    auto itr = m_actions.begin();
    for (; itr != m_actions.end(); ++itr)
    {
        if (Dev_NextWait == itr->type)
        {
            m_actions.erase(itr);
            bDoNext = true;
            break;
        }
        if (itr->bWaitFinish)
            return;
    }
    if (bDoNext)
        doAction();
}

void FeederMgr::addFeederAct(uint16_t cmd, bool bWait, int32_t ack, uint8_t numStore, float wFeed)
{
    DeviceAct act(Dev_Feeder, bWait);
    act.SetFeedCmd(cmd, ack);
    if (104 == cmd)
    {
        act.idStore = numStore;
        act.wFeed = wFeed;
    }
    m_actions << act;
}

void FeederMgr::addServoMotorAct(RobotPostion pos, bool bWait /*= true*/)
{
    if (pos < 0)
        return;

    DeviceAct actServo(Dev_Servo, bWait);
    actServo.servoPos = pos;
    m_actions << actServo;
}

void FeederMgr::addStepMotorAct(uint8_t type, uint8_t ch, bool bCont, bool bWait /*= true*/)
{
    DeviceAct actStep(Dev_StepMotor, bWait);
    actStep.stepCh = ch;
    actStep.stepType = type;
    actStep.stepDirCont = bCont;
    m_actions << actStep;
}


void FeederMgr::adddRobotAct(uint16_t type, uint8_t index /*= 0*/, bool bWait /*= true*/)
{
    DeviceAct actRobot(Dev_Robot, bWait);
    actRobot.robotStep = type;
    actRobot.robotIndex = index;
    m_actions << actRobot;
}

bool FeederMgr::CanAddWork(JobType t, int numTub, bool bProg)const
{
	auto tb = getTube(numTub);
	auto flg = tb ? tb->getFlag() : T_None;
	if (T_None==flg && flg>=T_WaitRecycle)
		return false;
	auto ls = tubeJobs(numTub);
	if (ls.contains(t))
		return false;
	else if (J_PrepareMate==t)
		return T_WaitPrepare == flg;

	switch (flg)
	{
	case T_WaitPrepare:
		return ls.contains(JobType(t-1)) || bProg;
	case T_Preparing:
	case T_Prepared:
		if (J_StoveFixTube == t)
			return true;
		return ls.contains(JobType(t-1)) || bProg;
	case T_WaitFix:
	case T_Fixing:
	case T_Fixed:
		return J_StoveTubeBack == t;
	}

	return false;
}

QList<FeederMgr::JobType> FeederMgr::tubeJobs(int numTub)const
{
	QList<JobType> ret;
	for (auto itr : m_jobs)
	{
		WorkItem it = WorkItem::initFrom(itr);
		if (it.nNumTube == numTub)
			ret << (JobType)it.type;
	}
	return ret;
}

TubeStruct *FeederMgr::getTube(int idx)const
{
    for (auto itr : m_allTube)
    {
        if (itr->getNumber() == idx)
            return itr;
    }
    return nullptr;
}

void FeederMgr::decode(const QByteArray& msg)
{
    if (m_sends.isEmpty())
        return;

    auto arr = m_sends.first();
    auto addr = PichModbusU16(arr.data()+2);
    if (arr.at(1) != msg.at(1))
        return;
    m_sends.removeFirst();
	bool bSnd = false;
    switch (arr.at(1))
    {
    case 3:
        bSnd = prcsRead(addr, msg);
        break;
    case 4:
        bSnd = prcsStat(addr, msg);
        break;
    case 0x10:
		bSnd = prcsWriteCmd(addr, PichModbusU16(arr.data() + 7));
        break;
    default:
        break;
    }
	if (!bSnd && !m_sends.isEmpty())
		send(m_sends.first());
}

int FeederMgr::send(const QByteArray& arr, bool bWaitWAck)
{
    auto ret = m_port->write(arr.data(), arr.size());
    if (!bWaitWAck && !m_sends.isEmpty() && 404== PichModbusU16(arr.data() + 2) && arr==m_sends.first())
        m_sends.removeFirst();
    else
        m_lastTmSnd = QDateTime::currentMSecsSinceEpoch();
    return ret;
}

void FeederMgr::genPrepareActions(const WorkItem &item)
{
    if(!m_actions.isEmpty())
		return;
    auto fb = getfeedParamsByTube(item.nNumTube);
    if (!fb)
        return;

    addFeederAct(100);
    int iGen = 0;
	for (auto &itr : fb->feedMaterial())
    {
        iGen++;
        addServoMotorAct(Pos_BlanceDoor);
        adddRobotAct(RobotMgr::OpenDoor);
        //addServoMotorAct(Pos_InOutBlance);
        adddRobotAct(RobotMgr::BottleInBlance, fb->getBottleNumb());
        //addServoMotorAct(Pos_BlanceDoor);
        adddRobotAct(RobotMgr::CloseDoor);

        addFeederAct(102);

        adddRobotAct(RobotMgr::MoveStore, itr.first);

        addFeederAct(104, true, 105, itr.first, itr.second);///投料

        adddRobotAct(RobotMgr::StoreBack, itr.first);

        if (iGen < fb->feedMaterial().count())
            addFeederAct(106, true, 103);
    }
    adddRobotAct(RobotMgr::OpenDoor);
    adddRobotAct(RobotMgr::BottleOutBlance, fb->getBottleNumb());
    adddRobotAct(RobotMgr::CloseDoor);
    addFeederAct(108);
    addFeederAct(110, false, 0);

    adddRobotAct(RobotMgr::Bottle2Tube, fb->getBottleNumb());
    if (auto bottle = fb->getBottle())
        bottle->setFlag(B_Using);
    if (auto tb = fb->getTube())
        tb->setFlag(T_Preparing);
 
    doAction();
}

void FeederMgr::genTubToStvoe(const WorkItem &bt)
{
    if (!m_actions.isEmpty())
		return;
	auto fb = getfeedParamsByTube(bt.nNumTube);
	if (!fb)
		return;

    addServoMotorAct(Pos_BlanceDoor);
    adddRobotAct(RobotMgr::MoveTube, bt.nNumTube);
    addStepMotorAct(CtrlType::Motor_Stove, bt.chStove, true, false);
    m_actions << DeviceAct(1.5);//等1.5S
    addStepMotorAct(CtrlType::Motor_Tube, bt.chStove, false, false);

    auto pos = getStovePos(bt.chStove);
    if (pos > 0)
        addServoMotorAct(pos);

    adddRobotAct(RobotMgr::Tube2Stove, bt.chStove);
    addStepMotorAct(CtrlType::Motor_Tube, bt.chStove, true);
    adddRobotAct(RobotMgr::OutStove, bt.chStove);
    addStepMotorAct(CtrlType::Motor_Stove, bt.chStove, false);

	doAction();
	if (auto tb = fb->getTube())
		tb->setFlag(T_Fixing);
}

void FeederMgr::genBackActions(const WorkItem &bt)
{
    if (!m_actions.isEmpty())
		return;
	auto fb = getfeedParamsByTube(bt.nNumTube);
	if (!fb)
		return;

    addStepMotorAct(CtrlType::Motor_Stove, bt.chStove, true, false);
    addServoMotorAct(bt.chStove==0? Pos_Stove1 : Pos_Stove2);
    adddRobotAct(RobotMgr::ClampStoveTube, bt.chStove);
    addStepMotorAct(CtrlType::Motor_Tube, bt.chStove, false);
    adddRobotAct(RobotMgr::StoveTubeOut, bt.chStove);
    addServoMotorAct(Pos_Home);
    adddRobotAct(RobotMgr::TubeBack, bt.nNumTube);

	doAction();
	if (auto tb = fb->getTube())
		tb->setFlag(T_Recycling);
}

void FeederMgr::checkActions()
{
    if (m_actions.isEmpty() && !m_jobs.isEmpty())
    {
        auto item = WorkItem::initFrom(m_jobs.takeFirst());
        if (auto fp = getfeedParamsByTube(item.nNumTube))
        {
            fp->feederFinish(item.type);
            QTimer::singleShot(10, this, [=] {emit feedTubeChanged(item.type, item.chStove); });

            if (!m_jobs.isEmpty())
            {
                item = WorkItem::initFrom(m_jobs.first());
                switch (item.type)
                {
                case J_PrepareMate:
                    genPrepareActions(item); break;
                case J_StoveFixTube:
                    genTubToStvoe(item); break;
                case J_StoveTubeBack:
                    genBackActions(item); break;
                default:
                    break;
                }
            }
        }
    }
}

bool FeederMgr::addWorkItem(const struct WorkItem &item)
{
	auto tp = (JobType)item.type;
	if (auto tb = getTube(item.nNumTube))
	{
		switch (tp)
		{
		case FeederMgr::J_PrepareMate:
			break;
		case FeederMgr::J_StoveFixTube:
			if (tb->getFlag() == T_Prepared)
				tb->setFlag(T_WaitFix);
			break;
		case FeederMgr::J_StoveTubeBack:
			if (tb->getFlag() == T_Fixed)
				tb->setFlag(T_WaitRecycle);
			break;
		default:
			return false;
		}
		m_jobs << item.toInt();
		return true;
	}
	return false;
}

QByteArray FeederMgr::pickMsg()
{
    if (m_sends.isEmpty())
    {
        m_buff.clear();
        return QByteArray();
    }
    int idx = 0;
    while (1)
    {
        idx = m_buff.indexOf(char(ModBussAddr), idx);
        if (idx < 0)
        {
            m_buff.clear();
            return QByteArray();
        }
        auto remian = m_buff.size() - idx;
        if (remian < 6)
            break;
        auto len = getAckLen((uint8_t*)m_buff.data()+idx, m_buff.size()-idx);
        if (len < 3)
        {
            ++idx;
            continue;
        }

        if (remian < len)
            break;

        auto data = (uint8_t*)m_buff.data() + idx;
        uint16_t crc = PichModbusU16(data+len-2);
        if (crc == Modbus_crc16(data, len - 2))
        {
            auto arr = m_buff.mid(idx, len);
            m_buff.remove(0, len + idx);
            return arr;
        }
        idx++;
    }
    if (idx > 0)
        m_buff.remove(0, idx);

    return QByteArray();
}

void FeederMgr::timerEvent(QTimerEvent* e)
{
    if (e->timerId() == m_idTimer)
    {
        if (m_sends.isEmpty())
        {
	        if (Flag_DoReadMaterial & m_flag)
	            readMaterials();
	        else if (Flag_DoReadStore & m_flag)
	            readStore();
	        else
                readFeedStat();
        }
    }
    else if (e->timerId()==m_idRead)
    {
        if (m_comStat==Communicate && QDateTime::currentMSecsSinceEpoch() - m_lastTmRcv > ConnetTimeOut)
        {
            m_comStat = NoData;
            emit connectStatChanged(m_comStat);
        }
        if (m_port->isOpen() && QDateTime::currentMSecsSinceEpoch()-m_lastTmSnd>500)
        {
            if (m_feedStore)
                readFeedWeight();
            else if (!m_sends.isEmpty())
                send(m_sends.first(), false);
        }
    }
}

void FeederMgr::readMaterials(int idx)
{
    if (m_flag & Flag_CanReadMaterial)
    {
        uint8_t buff[6] = { 1, 3, 0, 0, 0, 16 };
        uint16_t addr = 600 + idx * 16;
        buff[2] = (addr >> 8);
        buff[3] = addr & 0xff;
        append(buff, 6);
    }
    else if (Flag_DoReadMaterial & m_flag)
    {
        uint8_t buff[6] = { 1, 4, 0, 107, 0, 1 };
        append(buff, 6);
    }
}

void FeederMgr::readStore(int idx)
{
    if (Flag_ReadStore == (m_flag & Flag_ReadStore))
    {
        uint8_t buff[6] = { 1, 4, 0, 0, 0, 4 };
        uint16_t addr = 138 + idx * 4;
        buff[2] = (addr >> 8);
        buff[3] = addr & 0xff;
        append(buff, 6);
    }
    else if ((Flag_DoReadStore & m_flag) && Flag_ReadyReadStore!=(Flag_ReadyReadStore&m_flag))
    {
        uint8_t buff[6] = { 1, 4, 0, (m_flag & Flag_StoreNumRead) ? 106:103, 0, 1 };
        append(buff, 6);
    }
}

void FeederMgr::append(uint8_t* buff, uint16_t len)
{
    QByteArray arr(len+2, 0);
    memcpy(arr.data(), buff, len);
    AddModbusData((uint8_t*)arr.data()+len, Modbus_crc16(buff, len));
    if (m_sends.isEmpty())
        send(arr);
    m_sends << arr;
}

int FeederMgr::getAckLen(uint8_t *buff, uint32_t)const
{
    int ret = -1;
    switch (buff[1])
    {
    case 6:
        ret = 8; break;	///回fu源数据
    case 0x10:
        ret = 8; break;	///回fu源数据
    case 3:
    case 4:
        ret = buff[2] + 5; break;
    }

    return ret < 256 ? ret : -1;
}

const QStringList &FeederMgr::AllAvalidMaterials() const
{
    return m_canFeedMatesNames;
}

float FeederMgr::MaterialsWeight(const QString& name) const
{
    float ret = 0;
    if (name.isEmpty())
        return ret;

    for (auto& itr : m_allMaterials)
    {
        if (GetStore(itr->nfcid) && itr->name == name)
            ret += itr->weight-itr->weight;
    }
    return ret;
}

void FeederMgr::AddMaterial(const QString& nfcid, const QString& name, float weight)
{
	MaterialStruct m(nfcid, name, weight);

    auto date = QDate::currentDate();
    m.data.year = date.year()-2000;
    m.data.month = date.month();
    m.data.day = date.day();
    addMaterial(m);
}

void FeederMgr::ChangeMaterial(const QString& nfcid, float weight, const QString& name, const QString& idNew)
{
    int i = 0;
    for (auto itr : m_allMaterials)
    {
        if (nfcid == itr->nfcid)
        {
            bool bChange = false;
            if (itr->name != name || !Equal(itr->weight, weight))
            {
                itr->name = name;
                itr->weight = weight;
                bChange = true;
            }
            if (!idNew.isEmpty() && idNew!=nfcid)
            {
                itr->nfcid = idNew;
                bChange = true;
                if (auto c = getStore(nfcid))
                    c->nfcid = nfcid;
            }
            if (bChange)
            {
                auto date = QDate::currentDate();
                itr->data.year = date.year() - 2000;
                itr->data.month = date.month();
                itr->data.day = date.day();
                writeMaterial(itr, i);
                if (auto c = getStore(nfcid))
                    emit storeChanged(c);
                emit materialChanged(nfcid, *itr);
                checkMatesCanFeed();
            }
            return;
        }
        ++i;
    }
}

const MaterialStruct* FeederMgr::GetMaterial(const QString& id)const
{
    return getMaterial(id);
}

const StoreStruct* FeederMgr::GetStore(const QString& id)const
{
    if (id.isEmpty())
        return NULL;

    for (auto itr : m_allStore)
    {
        if (itr->pMate && itr->pMate->nfcid==id)
            return itr;
    }
    return NULL;
}

const StoreStruct* FeederMgr::GetStore(uint32_t idx)const
{
    if (idx >= m_nStoreNum)
        return NULL;

    for (auto& itr : m_allStore)
    {
        if (itr->numb == idx)
            return itr;
    }
    return NULL;
}

QStringList FeederMgr::GetStoreNfcId(bool bContainUse, const QString& cur)
{
    QStringList ret;
    if (!cur.isEmpty())
        ret << cur;

    for (auto& itr : m_allStore)
    {
        if (bContainUse || !GetMaterial(itr->nfcid))
            ret << itr->nfcid;
    }
    ret.removeDuplicates();
    return ret;
}

const QList<BottleStruct*>& FeederMgr::AllBottles()const
{
    return m_allBottle;
}

const QList<TubeStruct*>& FeederMgr::AllTubes()const
{
    return m_allTube;
}

TubeStruct *FeederMgr::ValidTube(int index)const
{
    for (auto itr : m_allTube)
    {
        if (T_WaitPrepare!=itr->getFlag())
            continue;
        if (index <0)
            return itr;
        if (itr->getNumber() == index)
            return itr;
    }
    return nullptr;
}

QList<TubeStruct*> FeederMgr::ValidTubes(JobType t)const
{
    QList<TubeStruct*> ret;
    for (auto itr : m_allTube)
    {
		switch (t)
		{
		case FeederMgr::J_PrepareMate:
			if (T_WaitPrepare == itr->getFlag())
				ret << itr;
			break;
		case FeederMgr::J_StoveFixTube:
			if (T_WaitFix == itr->getFlag())
				ret << itr;
			break;
		case FeederMgr::J_StoveTubeBack:
			if (T_WaitRecycle == itr->getFlag())
				ret << itr;
			break;
		}
    }
    return ret;
}

QList<BottleStruct*> FeederMgr::ValidBottls() const
{
    QList<BottleStruct*> ret;
    for (auto itr : m_allBottle)
    {
        if (B_CanUse == itr->getFlag())
            ret << itr;
    }
    return ret;
}

bool FeederMgr::FeedSolidMaterial(const QMap<QString, float> &feeds, int numb, int nTube, bool bFix, int ch)
{
    if (feeds.isEmpty() || GetfeedParamsByBottleNum(numb))
        return false; 

    if (nTube < 0)
    {
        for (auto itr : m_allTube)
        {
            if (T_WaitPrepare == itr->getFlag())
            {
                nTube = itr->getNumber();
                break;
            }
        }
    }
	if (!CanAddWork(J_PrepareMate, nTube))
		return false;
    auto bt = getBottle(numb);
    if (!bt || bt->getFlag() != B_CanUse)
        return false;

    bt->setFlag(B_WaitStart);
    QMap<int, float> preDistrs;
    sumFeederWeight(&preDistrs);
    QList<QPair<int, float> > preNumDistrs;
    for (auto itr = feeds.begin(); itr != feeds.end(); ++itr)
    {
        auto store = getPropStore(itr.value(), itr.key(), preDistrs);
        if (!store)
            return false;
        preNumDistrs << QPair<int, float>(store->numb, itr.value());
    }
    m_feedParams << FeederParam(preNumDistrs, (uint16_t)numb, nTube);
    auto item = WorkItem::initFrom(J_PrepareMate, nTube, ch);
	addWorkItem(item);
    genPrepareActions(item);
    if (bFix)
    {
        item.type = J_StoveFixTube;
        addWorkItem(item);
    }

    readFeedStat();
    return true;
}

bool FeederMgr::FixTube(uint16_t nTb, uint16_t ch)
{
	auto fb = getfeedParamsByTube(nTb);
	auto tb = getTube(nTb);
	if (!tb || !fb || !CanAddWork(J_StoveFixTube, nTb))
		return false;

	WorkItem item = WorkItem::initFrom(J_StoveFixTube, nTb, ch);
	if (tb->getFlag() == T_Prepared)
    {
        tb->setStoveCh(ch);
		addWorkItem(item);
		genTubToStvoe(item);
		return true;
	}

	addWorkItem(item);
	return false;
}

bool FeederMgr::StoveTubeBack(int ch, int nBack)
{
    auto tb = GetInSotveTube(ch);
    if (!tb)
        return false;

    auto fb = getfeedParamsByTube(tb->getNumber());
    if (!fb && !CanAddWork(J_StoveFixTube, tb->getNumber()))
        return false;

	WorkItem item = WorkItem::initFrom(J_StoveTubeBack, tb->getNumber(), ch, nBack < 0 ? 0 : nBack);
    if (tb->getFlag() == T_Fixed)
	{
		addWorkItem(item);
        genBackActions(item);
		return true;
    }

	addWorkItem(item);
    return false;
}

void FeederMgr::CancleFeed(BottleStruct*)
{
}

uint32_t FeederMgr::GetStoreNum()const
{
    return m_nStoreNum;
}

QString FeederMgr::GetCurPortName()const
{
    return m_port ? m_port->portName() : QString();
}

int FeederMgr::GetCurPortBaut()const
{
    return m_port ? m_port->baudRate() : 9600;
}

QSerialPort * FeederMgr::serialPort() const
{
    return m_port;
}

FeederMgr::PortStat FeederMgr::serialPortStat() const
{
    return m_comStat;
}

void FeederMgr::writeMaterial(const MaterialStruct *m, uint16_t index)
{
    uint8_t buff[39] = { 1, 0x10, 0, 0, 0, 16, 32 };
    uint16_t addr = 600+ index * 16;
    AddModbusData(buff + 2, addr);
    if (m)
    {
        auto tmp = QByteArray::fromHex(m->nfcid.toUtf8());
        auto sz = tmp.size() > 8 ? 8 : tmp.size();
        memcpy((char*)buff+15-sz, tmp.data(), sz);
        strncpy((char*)buff + 15, m->name.toStdString().c_str(), 10);
        AddModbusData(buff + 25, m->type);
        AddModbusFloat(buff + 27, m->usedRate);
        AddModbusFloat(buff + 31, m->weight);
        memcpy((char*)buff + 35, (char*)&m->ch, 2);
    }
    append(buff, 39);

    writeFunc(404);
}

void FeederMgr::writeCmd(const DeviceAct &act)
{
    if (Dev_Feeder != act.type)
        return;

    uint8_t buff[23] = { 1, 0x10, 0, 0, 0, 1, 2, 0};
    int len = 9;
    AddModbusData(buff + 2, 2000);
    AddModbusData(buff + 7, act.cmdFeeder);
    if (auto c = act.cmdFeeder == 104 ? getStore(act.idStore) : nullptr)
    {
        auto arr = QByteArray::fromHex(c->nfcid.toUtf8());
        uint16_t sz = arr.size();
        memcpy(buff + 9, arr.data(), sz > 8 ? 8 : sz);
        AddModbusFloat(buff + 17, act.wFeed);
        len = 23;
        buff[5] = 8;
        buff[6] = 16;
        c->setStat(S_Feeding);
        m_feedStore = c;
        emit feedingChanged(act.wFeed, true);
    }
    append(buff, len);
}

void FeederMgr::writeFunc(uint16_t cmd, uint16_t val)
{
    uint8_t buff2[6] = { 1, 6, 0, 0, 0, 1 };
    AddModbusData(buff2 + 2, cmd);
    AddModbusData(buff2 + 4, val);
    append(buff2, 6);
}

void FeederMgr::readFeedStat()
{
    uint8_t buff[6] = { ModBussAddr, 4, 0, 105, 0, 1 };
    append(buff, 6);
}

void FeederMgr::readFeedWeight()
{
    uint8_t buff[6] = { ModBussAddr, 4, 0, 108, 0, 1 };
    append(buff, 6);
}

int FeederMgr::indexOfMaterial(const QString& id)
{
    int i = 0;
    for (auto itr = m_allMaterials.begin(); itr != m_allMaterials.end(); ++itr, ++i)
    {
        if ((*itr)->nfcid == id)
            return i;
    }
    return i;
}

void FeederMgr::addMaterial(const MaterialStruct& m, bool bAdd)
{
    int i = 0;

    for (auto itr : m_allMaterials)
    {
        if (m.nfcid==itr->nfcid) ///相同id，看时间顺序使用最新的
        {
            if (itr->name != m.name || !Equal(itr->weight, m.weight))
            {
                itr->name = m.name;
                itr->weight = m.weight;
                if (bAdd)
                    writeMaterial(&m, i);

                emit materialChanged(itr->nfcid, *itr);
                if (auto c = getStore(itr->nfcid))
                {
					if (c->getStat() == S_NoMate)
						c->setStat(S_CanFeed);
					else
						emit storeChanged(c);
                    checkMatesCanFeed();
                }
            }
            return;
        }
        ++i;
    }
    if (auto tmp = new MaterialStruct(m))
    {
        m_allMaterials << tmp;
        if (auto c = getStore(m.nfcid))
        { 
            c->pMate = tmp;
            emit storeChanged(c);
        }
        if(bAdd)
            writeMaterial(&m, i);

        emit materialAdded(m);
        checkMatesCanFeed();
    }
}

void FeederMgr::updateStore(const QString& nfcid, int idx)
{
	auto stat = nfcid.isEmpty() ? S_None : S_NoMate;
    auto m = S_NoMate==stat ? GetMaterial(nfcid) : nullptr;
	if (m)
		stat = S_CanFeed;

    if (idx + 1 == m_nStoreNum)
        m_flag &= ~Flag_DoReadStore;

    for (auto& itr : m_allStore)
    {
        if (itr->numb==idx)
        {
            if (itr->pMate != m)
            {
                itr->pMate = m;
                if (itr->getStat() < S_CanFeed)
                    itr->setStat(stat);
                else
                    emit storeChanged(itr);

                checkMatesCanFeed();
            }
            return;
        }
    }
	auto tmp = new StoreStruct(idx, m, nfcid);
	tmp->setStat(stat);
    m_allStore << tmp;
    emit storeChanged(tmp);
    checkMatesCanFeed();
}
