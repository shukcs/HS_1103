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

/*
* FeederBottle
*/
FeederBottle::FeederBottle(const QList<QPair<int, float> > &feeds, uint16_t numb, int16_t nTube) : m_numbBottle(numb)
, m_feedMaterials (feeds), m_ch(0), m_numbTube(nTube)
{
}

int16_t FeederBottle::getBottleNumb() const
{
	return m_numbBottle;
}

int16_t FeederBottle::getTubeNumb() const
{
    return m_numbTube;
}

TubeStruct *FeederBottle::getTube() const
{
    return FeederDecoder::Instance().getTube(m_numbTube);
}

const QList<QPair<int, float> > & FeederBottle::feedMaterial() const
{
    return m_feedMaterials;
}

void FeederBottle::getFeedNameAndWeight(QList<QPair<QString, float> > *ret) const
{
    if (!ret)
        return;

    auto &feeder = FeederDecoder::Instance();
    ret->clear();
    for (auto &itr : m_feedMaterials)
    {
        auto c = feeder.GetContainer(itr.first);
        if (auto m = c ? c->pMate : nullptr)
            *ret << QPair<QString, float>(m->name, itr.second);
    }
}

bool FeederBottle::isRepeat() const
{
    return m_bRepeat;
}

void FeederBottle::setRepeat(bool b)
{
    m_bRepeat = b;
}

int16_t FeederBottle::getChannel() const
{
    return m_ch;
}

void FeederBottle::setChannel(int16_t ch)
{
    m_ch = ch;
}

void FeederBottle::feedFinish()const
{
    auto &feeder = FeederDecoder::Instance();
    if (auto tb = feeder.getTube(m_numbTube))
    {
        tb->flag = C_Using;
        tb->bFeeded = true;
        feeder.tubeChanged(tb);
    }
}

/*
* FeederBottle
*/
BottleStruct::BottleStruct(uint16_t num, uint16_t flag, uint16_t stWork) : m_numb(num)
, m_flag(flag), m_stWork(stWork)
{
}

/*
* FeederBottle
*/
TubeStruct::TubeStruct(uint16_t n, uint16_t f, uint16_t c)
: numb(n), flag(f), chStove(c)
{
}

/*
* FeederDecoder
*/
FeederDecoder::FeederDecoder(QObject* p) : QObject(p)
, m_port(new QSerialPort(qApp))
{
    connect(m_port, &QSerialPort::readyRead, this, &FeederDecoder::readByets);
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
    QTimer::singleShot(50, this, &FeederDecoder::ConnectPort);
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

	QTimer::singleShot(50, this, [=] {
		AddMaterial("123", "固体1", 52.5);
		AddMaterial("124", "固体2", 62.5);
		updateStore("123", 0);
		updateStore(QString(), 1);
	});
}

FeederDecoder::~FeederDecoder()
{
    qDeleteAll(m_allMaterials);
    qDeleteAll(m_allStore);
    qDeleteAll(m_allBottle);
    qDeleteAll(m_allTube);
}

FeederDecoder& FeederDecoder::Instance()
{
    static FeederDecoder s_ins(nullptr);
    return s_ins;
}

uint16_t FeederDecoder::Modbus_crc16(const uint8_t* buff, uint16_t len)
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
bool FeederDecoder::Equal(double f1, double f2)
{
    return fabs(f1 - f2) < 0.000001;
}

void FeederDecoder::AddModbusFloat(uint8_t* buff, float f)
{
    DataU tmp;
    tmp.fD = f;
    buff[0] = tmp.cD[2];
    buff[1] = tmp.cD[3];
    buff[2] = tmp.cD[0];
    buff[3] = tmp.cD[1];
}

void FeederDecoder::AddModbusData(uint8_t* buff, uint16_t u)
{
    DataU tmp;
    tmp.u16D = u;
    buff[0] = tmp.cD[1];
    buff[1] = tmp.cD[0];
}

float FeederDecoder::PichModbusFloat(const void* src)
{
    auto buff = (const uint8_t*)src;
    DataU tmp;
    tmp.cD[2] = buff[0];
    tmp.cD[3] = buff[1];
    tmp.cD[0] = buff[2];
    tmp.cD[1] = buff[3];

    return tmp.fD;
}

uint16_t FeederDecoder::PichModbusU16(const void* src)
{
    auto buff = (const uint8_t*)src;
    DataU tmp;
    tmp.cD[0] = buff[1];
    tmp.cD[1] = buff[0];

    return tmp.u16D;
}

FeederDecoder::RobotPostion FeederDecoder::getStovePos(int ch)
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

QString FeederDecoder::DefaultConfigFile()
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

void FeederDecoder::ConnectPort()
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
            emit connectStatChanged(PortClose);
        }
    }
}

TubeStruct * FeederDecoder::GetInSotveTube(uint8_t ch) const
{
    for (auto &itr : m_feedParams)
    {
        if(!itr.canConvert<FeederBottle>())
            continue;

        auto fb = itr.value<FeederBottle>();
        if (auto tb = fb.getTube())
        {
            if (tb->bFeeded && C_Using==tb->flag && fb.getChannel()==ch)
                return tb;
        }
    }
    return nullptr;
}

void FeederDecoder::OnRobotDone(int typeRobot)
{
    auto itr = m_actions.begin();
    for (; itr != m_actions.end(); ++itr)
    {
        if (Dev_Robot == itr->type && itr->robotStep==typeRobot)
        {
            bool bChange = RobotMgr::MoveStore == typeRobot || RobotMgr::StoreBack == typeRobot;
            if (auto c = bChange ? getContainer(itr->robotIndex) : nullptr)
            {
                c->stat = RobotMgr::MoveStore == typeRobot ? 2 : 0;
                emit containerChanged(c);
            }
            if (auto tb = (RobotMgr::Bottle2Tube == typeRobot) ? getTube(itr->robotIndex) : nullptr)
                tb->flag = C_Using;

            if (auto c = RobotMgr::StoreBack==typeRobot ? getContainer(itr->robotIndex) : nullptr)
                c->stat = 0;

            itr = m_actions.erase(itr);
            break;
        }
        if (itr->bWaitFinish)
            return;
    }
    doAction();
}

void FeederDecoder::OnServoMotor(int pos, bool bReached)
{
    if (!bReached || m_actions.isEmpty())
        return;

    auto itr = m_actions.begin();
    for (; itr != m_actions.end(); ++itr)
    {
        if (Dev_Servo==itr->type && itr->servoPos==pos)
        {
            itr = m_actions.erase(itr);
            break;
        }
        if (itr->bWaitFinish)
            return;
    }
    if (itr == m_actions.end())
        return;
    doAction();
}

void FeederDecoder::OnStepMotor(StepMotorStat* st)
{
    if (st->IsRun() || m_actions.isEmpty())
        return;

    auto itr = m_actions.begin();
    for (; itr != m_actions.end(); ++itr)
    {
        if (Dev_StepMotor != itr->type || itr->stepType != st->GetType() || itr->stepCh != st->GetChannel())
            continue;
        bool bReached = itr->stepDirCont ? st->IsLimitH() : st->IsLimitL();
        if (bReached)
        {
            itr = m_actions.erase(itr);
            break;
        }
        if (itr->bWaitFinish)
            return;
    }
    if (itr == m_actions.end())
        return;

    doAction();
}

void FeederDecoder::readByets()
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

void FeederDecoder::prcsRead(uint16_t addr, const QByteArray& msg)
{
    if (addr >= 600 && addr < 600 * 32 * 16)
    {
        auto idx = (addr - 600) / 16;
        MaterialStruct m(msg.mid(3, 8).toHex().toUpper());
        if (m.nfcid=="0000000000000000")
        {
            m_flag &= ~Flag_DoReadMaterial;
            return;
        }

        m.name = QString::fromStdString(std::string(msg.data() + 11, 10).c_str());
        m.type = PichModbusFloat(msg.data() + 21);
        m.usedRate = PichModbusFloat(msg.data() + 23);
        m.weight = PichModbusFloat(msg.data() + 27);
        memcpy(&m.ch, msg.data() + 31, 2);

        addMaterial(m, false);
        readMaterials(idx + 1);
    }
}
void FeederDecoder::prcsStat(uint16_t addr, const QByteArray& msg)
{
    switch (addr)
    {
    case 103:
        if (m_nStoreNum >0 && m_nStoreNum < 12 && PichModbusU16(msg.data() + 3) != m_nStoreNum)
            writeFunc(402, m_nStoreNum);
        m_flag |= Flag_StoreNumRead;
        readStore();
        break;
    case 105:
        prcsFeederStat(PichModbusU16(msg.data() + 3));
        break;
    case 106:
        if (0==PichModbusU16(msg.data()+3))
        {
            m_flag |= Flag_CanReadStore;
            readStore();
        }
        break;
    case 107:
        if (0 == PichModbusU16(msg.data() + 3))
        {
            m_flag |= Flag_CanReadMaterial;
            readMaterials();
        }
        break;
    default:
        if (138 <= addr && addr < 138 + 4 * m_nStoreNum)
        {
            uint32_t idx = (addr - 138) / 4;
            QString nfcif(QByteArray(msg.data() + 3, 8).toHex());
            bool b = nfcif != "0100000000000000" && nfcif != "0000000000000000";
            updateStore(b ? nfcif.toUpper() : QString(), idx);
            readStore(idx + 1);
        }
        break;
    }
}

void FeederDecoder::checkMatesCanFeed()
{
    QStringList ret;
    for (auto& itr : m_allMaterials)
    {
        if (GetContainer(itr->nfcid))
            ret << itr->name;
    }
    ret.removeDuplicates();

    if (m_canFeedMatesNames != ret)
    {
        m_canFeedMatesNames = ret;
        emit matesCanFeedChanged();
    }
}

void FeederDecoder::sumFeederWeight(QMap<int, float> *feeds) const
{
    if (!feeds)
        return;

    QMap<int, float> &ref = *feeds;
    for (auto &itr : m_feedParams)
    {
        if(!itr.canConvert<FeederBottle>())
            continue;

        auto fb = itr.value<FeederBottle>();
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

void FeederDecoder::prcsFeederStat(uint16_t stat)
{
    for (auto itr = m_actions.begin(); itr != m_actions.end(); ++itr)
    {
        if (Dev_Feeder == itr->type && itr->cmdAck == stat)
        {
            itr = m_actions.erase(itr);
            break;
        }
        if (itr->bWaitFinish)
            return;
    }
    doAction();
}

void FeederDecoder::doAction()
{
    checkActions();
    if (m_actions.isEmpty())
        return;

    bool bWaiNext = false;
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
            writeCmd(act.cmdFeeder);
            break;
        case Dev_NextWait:
            bWaiNext = true;
            QTimer::singleShot(act.fWaitTime*1000, this, &FeederDecoder::onWait);
            break;
        }
        act.bStart = true;
        if (act.bWaitFinish)
            break;
    }
}

StoreStruct* FeederDecoder::getContainer(const QString& id)const
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

StoreStruct* FeederDecoder::getContainer(uint16_t num) const
{
    for (auto itr : m_allStore)
    {
        if (num == itr->numb)
            return itr;
    }
    return nullptr;
}

MaterialStruct* FeederDecoder::getMaterial(const QString& id)const
{
    if (id.isEmpty())
        return NULL;

    for (auto itr : m_allMaterials)
    {
        if (itr->nfcid == id)
            return itr;
    }
    return NULL;
}

const FeederBottle * FeederDecoder::getfeedParams(int numb) const
{
    for (auto &itr : m_feedParams)
    {
        if (!itr.canConvert<FeederBottle>())
            continue;

        auto ret = (FeederBottle*)itr.data();
        if (ret->getBottleNumb() == numb)
            return ret;
    }
    return nullptr;
}

const FeederBottle * FeederDecoder::getfeedParamsByTube(int numb) const
{
    for (auto &itr : m_feedParams)
    {
        if (!itr.canConvert<FeederBottle>())
            continue;

        auto ret = (FeederBottle*)itr.data();
        if (ret->getTubeNumb() == numb)
            return ret;
    }
    return nullptr;
}

StoreStruct* FeederDecoder::getPropContainer(float weight, const QString& name, const QMap<int, float> &preDistrs)const
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

BottleStruct * FeederDecoder::getBottle(int numb) const
{
    for (auto itr : m_allBottle)
    {
        if (itr->m_numb == numb)
            return itr;
    }
    return nullptr;
}

void FeederDecoder::onWait()
{
    auto itr = m_actions.begin();
    for (; itr != m_actions.end(); ++itr)
    {
        if (Dev_NextWait == itr->type)
        {
            m_actions.erase(itr);
            break;
        }
        if (itr->bWaitFinish)
            return;
    }
    doAction();
}

void FeederDecoder::addFeederAct(uint16_t cmd, bool bWait/*=false*/, int32_t ack)
{
    DeviceAct act(Dev_Feeder, bWait);
    act.SetFeedCmd(cmd, ack);
    m_actions << act;
}

void FeederDecoder::addServoMotorAct(RobotPostion pos, bool bWait /*= true*/)
{
    if (pos < 0)
        return;

    DeviceAct actServo(Dev_Servo, bWait);
    actServo.servoPos = pos;
    m_actions << actServo;
}

void FeederDecoder::addStepMotorAct(uint8_t type, uint8_t ch, bool bCont, bool bWait /*= true*/)
{
    DeviceAct actStep(Dev_StepMotor, bWait);
    actStep.stepCh = ch;
    actStep.stepType = type;
    actStep.stepDirCont = bCont;
    m_actions << actStep;
}


void FeederDecoder::adddRobotAct(uint16_t type, uint8_t index /*= 0*/, bool bWait /*= true*/)
{
    DeviceAct actRobot(Dev_Robot, bWait);
    actRobot.robotStep = type;
    actRobot.robotIndex = index;
    m_actions << actRobot;
}

TubeStruct *FeederDecoder::getTube(int idx)const
{
    for (auto itr : m_allTube)
    {
        if (itr->numb == idx)
            return itr;
    }
    return nullptr;
}

void FeederDecoder::decode(const QByteArray& msg)
{
    if (m_sends.isEmpty())
        return;

    auto arr = m_sends.first();
    auto addr = PichModbusU16(arr.data()+2);
    if (arr.at(1) != msg.at(1))
        return;
    m_sends.removeFirst();
    switch (arr.at(1))
    {
    case 3:
        prcsRead(addr, msg);
        break;
    case 4:
        prcsStat(addr, msg);
        break;
    case 0x10:
        if (200 == addr)
            readFeedStat();
        break;
    default:
        break;
    }
}

int FeederDecoder::send(const QByteArray& arr, bool bWaitWAck)
{
    auto ret = m_port->write(arr.data(), arr.size());
    if (!bWaitWAck && !m_sends.isEmpty() && 404== PichModbusU16(arr.data() + 2) && arr==m_sends.first())
        m_sends.removeFirst();
    else
        m_lastTmSnd = QDateTime::currentMSecsSinceEpoch();
    return ret;
}

void FeederDecoder::genActions(FeederBottle &bt)
{
	if (!m_actions.isEmpty() || m_feedParams.isEmpty())
		return;

    DeviceAct act(Dev_Feeder, true);

    addFeederAct(100);

    int iGen = 0;
	for (auto &itr : bt.feedMaterial())
    {
        iGen++;

        addServoMotorAct(Pos_BlanceDoor);
        adddRobotAct(RobotMgr::OpenDoor);
        //addServoMotorAct(Pos_InOutBlance);
        adddRobotAct(RobotMgr::BottleInBlance/*, bt.getBottleNumb()*/);
        //addServoMotorAct(Pos_BlanceDoor);
        adddRobotAct(RobotMgr::CloseDoor);

        addFeederAct(102);

        adddRobotAct(RobotMgr::MoveStore/*, itr.first*/);

        addFeederAct(104);

        adddRobotAct(RobotMgr::StoreBack/*, itr.first*/);

        if (iGen < bt.feedMaterial().count())
            addFeederAct(106, true, 103);
    }
    adddRobotAct(RobotMgr::OpenDoor);
    adddRobotAct(RobotMgr::BottleOutBlance/*, bt.getBottleNumb()*/);
    adddRobotAct(RobotMgr::CloseDoor);
    addFeederAct(108);
    addFeederAct(110, false, 0);
    adddRobotAct(RobotMgr::Bottle2Tube/*, bt.getBottleNumb()*/);
    adddRobotAct(RobotMgr::MoveTube/*, bt.getBottleNumb()*/);
    addStepMotorAct(CtrlType::Motor_Stove, bt.getChannel(), true, false);
    m_actions << DeviceAct(1.5);//等1.5S
    addStepMotorAct(CtrlType::Motor_Tube, bt.getChannel(), false, false);

    auto pos = getStovePos(bt.getChannel());
    if (pos > 0)
        addServoMotorAct(pos);

    adddRobotAct(RobotMgr::Tube2Stove/*, bt.getChannel()*/);
    addStepMotorAct(CtrlType::Motor_Tube, bt.getChannel(), true);
    adddRobotAct(RobotMgr::OutStove/*, bt.getChannel()*/);
    addStepMotorAct(CtrlType::Motor_Stove, bt.getChannel(), false);

    if (auto bottle = getBottle(bt.getBottleNumb()))
    {
        if (C_Using != bottle->m_flag)
        {
            bottle->m_flag = C_Using;
            emit bottleChanged(bottle);
        }
    }
    doAction();
}

void FeederDecoder::genBackActions(const FeederBottle *fb)
{
    if (!m_actions.isEmpty() || m_feedParams.isEmpty() || fb)
        return;

    addStepMotorAct(CtrlType::Motor_Stove, fb->getChannel(), true, false);
    addServoMotorAct(fb->getChannel()==0? Pos_Stove1 : Pos_Stove2);
    adddRobotAct(RobotMgr::ClampStoveTube, fb->getChannel());
    addStepMotorAct(CtrlType::Motor_Tube, fb->getChannel(), false);
    adddRobotAct(RobotMgr::StoveTubeOut, fb->getChannel());
    addServoMotorAct(Pos_Home);
    adddRobotAct(RobotMgr::TubeBack/*, fb->getTubeNumb()*/);

    doAction();
}

void FeederDecoder::checkActions()
{
    if (m_actions.isEmpty() && !m_feedParams.isEmpty())
    {
        bool bSet = false;
        for (auto &itr : m_feedParams)
        {
            if (itr.canConvert<FeederBottle>())
            {
                auto fp = itr.value<FeederBottle>();
                auto tb = fp.getTube();
                if (!tb || tb->bFeeded)
                    continue;

                if (!bSet)
                {
                    bSet = true;
                    fp.feedFinish();
                    if (auto bt = getBottle(fp.getBottleNumb()))
                    {
                        bt->m_flag = C_Used;
                        emit bottleChanged(bt);
                    }
					emit feedTubeChanged(fp.getChannel(), true);
                    continue;
                }
                genActions(fp);
                break;
            }
            else if (itr.canConvert<int>())
            {
                if (auto tb = getTube(itr.toInt()))
                {
                    if (tb->flag >= C_Used)
                        continue;
					
					auto fb = getfeedParamsByTube(tb->numb);
                    if (!bSet)
                    {
                        bSet = true;
                        tb->flag = C_Back;
                    }
                    genBackActions(fb);
                    break;
                }
            }
        }
    }
}

QByteArray FeederDecoder::pickMsg()
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
        if (remian < 4)
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

void FeederDecoder::timerEvent(QTimerEvent* e)
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
        if (m_port->isOpen() && !m_sends.isEmpty() && QDateTime::currentMSecsSinceEpoch()-m_lastTmSnd>500)
            send(m_sends.first(), false);
    }
}

void FeederDecoder::readMaterials(int idx)
{
    if (idx < 0 || idx >= MaterialMaxNum)
    {
        m_flag &= ~Flag_DoReadMaterial;
        return;
    }

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

void FeederDecoder::readStore(int idx)
{
    if (idx < 0 || idx >= m_nStoreNum)
    {
        m_flag &= ~Flag_DoReadStore;
        return;
    }

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

void FeederDecoder::append(uint8_t* buff, uint16_t len)
{
    QByteArray arr(len+2, 0);
    memcpy(arr.data(), buff, len);
    AddModbusData((uint8_t*)arr.data()+len, Modbus_crc16(buff, len));
    if (m_sends.isEmpty())
        send(arr);
    m_sends << arr;
}

int FeederDecoder::getAckLen(uint8_t *buff, uint32_t)const
{
    int ret = -1;
    switch (buff[1])
    {
    case 6:
    case 0x10:
        ret = 8; break;
    case 3:
    case 4:
        ret = buff[2] + 5; break;
    }

    return ret;
}

const QStringList &FeederDecoder::AllAvalidMaterials() const
{
    return m_canFeedMatesNames;
}

float FeederDecoder::MaterialsWeight(const QString& name) const
{
    float ret = 0;
    if (name.isEmpty())
        return ret;

    for (auto& itr : m_allMaterials)
    {
        if (GetContainer(itr->nfcid) && itr->name == name)
            ret += itr->weight-itr->weight;
    }
    return ret;
}

void FeederDecoder::AddMaterial(const QString& nfcid, const QString& name, float weight)
{
	MaterialStruct m(nfcid, name, weight);

    auto date = QDate::currentDate();
    m.data.year = date.year()-2000;
    m.data.month = date.month();
    m.data.day = date.day();
    addMaterial(m);
}

void FeederDecoder::ChangeMaterial(const QString& nfcid, float weight, const QString& name, const QString& idNew)
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
                if (auto c = getContainer(nfcid))
                    c->nfcid = nfcid;
            }
            if (bChange)
            {
                auto date = QDate::currentDate();
                itr->data.year = date.year() - 2000;
                itr->data.month = date.month();
                itr->data.day = date.day();
                writeMaterial(itr, i);
                if (auto c = getContainer(nfcid))
                    emit containerChanged(c);
                emit materialChanged(nfcid, *itr);
                checkMatesCanFeed();
            }
            return;
        }
        ++i;
    }
}

const MaterialStruct* FeederDecoder::GetMaterial(const QString& id)const
{
    return getMaterial(id);
}

const StoreStruct* FeederDecoder::GetContainer(const QString& id)const
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

const StoreStruct* FeederDecoder::GetContainer(uint32_t idx)const
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

QStringList FeederDecoder::GetStoreNfcId(bool bContainUse, const QString& cur)
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

const QList<BottleStruct*>& FeederDecoder::AllBottles()const
{
    return m_allBottle;
}

const QList<TubeStruct*>& FeederDecoder::AllTubes()const
{
    return m_allTube;
}

TubeStruct *FeederDecoder::ValidTube(int index)const
{
    for (auto itr : m_allTube)
    {
        if (C_CanUse != itr->flag)
            continue;
        if (index <0)
            return itr;
        if (itr->numb == index)
            return itr;
    }
    return nullptr;
}

QList<TubeStruct*> FeederDecoder::ValidTubes()const
{
    QList<TubeStruct*> ret;
    for (auto itr : m_allTube)
    {
        if (C_CanUse == itr->flag)
            ret << itr;
    }
    return ret;
}

bool FeederDecoder::FeedSolidMaterial(const QMap<QString, float> &feeds, int numb, int nTube, int ch)
{
    if (feeds.isEmpty())
        return false;

    if (numb < 0)
    {
        for (auto itr : m_allBottle)
        {
            if (C_CanUse == itr->m_flag)
            {
                numb = itr->m_numb;
                break;
            }
        }
    }
        
    if (getfeedParams(numb))
        return false;

    if (nTube < 0)
    {
        for (auto itr : m_allTube)
        {
            if (C_CanUse == itr->flag)
            {
                numb = itr->numb;
                break;
            }
        }
    }

    QMap<int, float> preDistrs;
    sumFeederWeight(&preDistrs);
    QList<QPair<int, float> > preNumDistrs;
    for (auto itr = feeds.begin(); itr != feeds.end(); ++itr)
    {
        auto store = getPropContainer(itr.value(), itr.key(), preDistrs);
        if (!store)
            return false;
        preNumDistrs << QPair<int, float>(store->numb, itr.value());
    }
    if (auto bt = getBottle(numb))
    {
        if (auto tb = getTube(nTube))
        {
            if (tb->flag != C_WaitStart)
            {
                tb->flag = C_WaitStart;
                emit tubeChanged(tb);
            }
        }
        if (C_WaitStart!=bt->m_flag)
        {
            bt->m_flag = C_WaitStart;
            emit bottleChanged(bt);
        }
        FeederBottle fb(preNumDistrs, (uint16_t)numb, nTube);
        fb.setChannel(ch);
        m_feedParams << QVariant::fromValue(fb);
        genActions(fb);
        readFeedStat();
        return true;
    }
    return false;
}

bool FeederDecoder::StoveTubeBack(int num)
{
    for (auto &itr : m_feedParams)
    {
        if (itr.type() != QVariant::Int)
            continue;

        if (itr.toInt() == num)
            return false;
    }

    if (auto tb = getTube(num))
    {
        if (tb->bFeeded && tb->flag==C_Using)
        {
            m_feedParams << num;
            genBackActions(getfeedParamsByTube(num));
            tb->flag = C_Back;
            return true;
        }
    }
    return false;
}

void FeederDecoder::CancleFeed(BottleStruct*)
{
}

uint32_t FeederDecoder::GetStoreNum()const
{
    return m_nStoreNum;
}

QString FeederDecoder::GetCurPortName()const
{
    return m_port ? m_port->portName() : QString();
}

int FeederDecoder::GetCurPortBaut()const
{
    return m_port ? m_port->baudRate() : 9600;
}

QSerialPort * FeederDecoder::serialPort() const
{
    return m_port;
}

FeederDecoder::PortStat FeederDecoder::serialPortStat() const
{
    return m_comStat;
}

void FeederDecoder::writeMaterial(const MaterialStruct *m, uint16_t index)
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

void FeederDecoder::writeCmd(uint16_t cmd, const QString& id, float w)
{
    uint8_t buff[23] = { 1, 0x10, 0, 0, 0, 1, 2, 0};
    int len = 9;
    AddModbusData(buff + 2, 2000);
    AddModbusData(buff + 7, cmd);
    if (!id.isEmpty())
    {
        auto arr = QByteArray::fromHex(id.toUtf8());
        uint16_t sz = arr.size();
        memcpy(buff + 9, arr.data(), sz > 8 ? 8 : sz);
        AddModbusFloat(buff + 17, w);
        len = 23;
        buff[5] = 8;
        buff[6] = 16;
    }
    append(buff, len);
}

void FeederDecoder::writeFunc(uint16_t cmd, uint16_t val)
{
    uint8_t buff2[6] = { 1, 6, 0, 0, 0, 1 };
    AddModbusData(buff2 + 2, cmd);
    AddModbusData(buff2 + 4, val);
    append(buff2, 6);
}

void FeederDecoder::readFeedStat()
{
    uint8_t buff[6] = { ModBussAddr, 4, 0, 105, 0, 1 };
    append(buff, 6);
}

int FeederDecoder::indexOfMaterial(const QString& id)
{
    int i = 0;
    for (auto itr = m_allMaterials.begin(); itr != m_allMaterials.end(); ++itr, ++i)
    {
        if ((*itr)->nfcid == id)
            return i;
    }
    return i;
}

void FeederDecoder::addMaterial(const MaterialStruct& m, bool bAdd)
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
                if (auto c = GetContainer(itr->nfcid))
                {
                    emit containerChanged(c);
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
        if (auto c = getContainer(m.nfcid))
        { 
            c->pMate = tmp;
            emit containerChanged(c);
        }
        if(bAdd)
            writeMaterial(&m, i);

        emit materialAdded(m);
        checkMatesCanFeed();
    }
}

void FeederDecoder::updateStore(const QString& nfcid, int idx)
{
    auto m = nfcid.isEmpty() ? nullptr : GetMaterial(nfcid);

    for (auto& itr : m_allStore)
    {
        if (itr->numb == idx && itr->pMate!=m)
        {
            itr->numb = idx;
            itr->pMate = m;
            emit containerChanged(itr);
            checkMatesCanFeed();
            return;
        }
    }
	auto tmp = new StoreStruct(idx, m, nfcid);
    m_allStore << tmp;
    emit containerChanged(tmp);
    checkMatesCanFeed();
}