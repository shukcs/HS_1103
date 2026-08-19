#include "MaterialStore.h"
#include <QSerialPortInfo>
#include <QSerialPort>
#include <QItemDelegate>
#include <QPainter>
#include "FeederDecoder.h"
#include "subMateUi/DlgMaterialModify.h"
#include "subMateUi/DlgFeedMaterial.h"
#include "subMateUi/DlgTubeBack.h"
#include "common/DlgSerialSettings.h"
#include "common/DlgSocketSettings.h"
#include "RobotMgr.h"

#include "ui_MaterialStore.h"
#pragma execution_character_set("utf-8")

class ContainerDelegate : public QItemDelegate
{
public:
    explicit ContainerDelegate(QObject* p = nullptr) : QItemDelegate(p) {}

    QSize sizeHint(const QStyleOptionViewItem&, const QModelIndex&)const override
    {
        return QSize(107, 175);
    }
    void paint(QPainter* p, const QStyleOptionViewItem& opt, const QModelIndex& index) const override
    {
		static QIcon ic{ ":/image/store.png" };
		ic.paint(p, opt.rect);
        p->setRenderHint(QPainter::Antialiasing);
		QColor col(Qt::lightGray);
		if (auto c = index.data(Qt::UserRole + 1).value<const StoreStruct*>())
		{
			switch (c->getStat())
			{
			case  S_NoMate:
				col = Qt::white; break;
			case S_CanFeed:
			case S_WaitFeed:
			case S_Feeded:
				col = Qt::darkGreen; break;
			case S_Feeding:
				col = QColor("#00c0f0"); break;
			default:
				break;
			}
            auto ft = QFont(tr("宋体"));
            ft.setPointSize(c->nfcid.isEmpty() ? 30 : 12);
            p->setFont(ft);
		}
		auto rc = opt.rect.adjusted(17, 69, -15, -51);
        p->setBrush(col);
		p->setPen(Qt::transparent);
		p->drawRect(rc);
		p->setPen(Qt::black);
		p->drawText(rc, Qt::AlignCenter, index.data(Qt::DisplayRole).toString());
    }
};

class BottleDelegate : public QItemDelegate
{
public:
    explicit BottleDelegate(QObject* p = nullptr) : QItemDelegate(p) {}

    QSize sizeHint(const QStyleOptionViewItem&, const QModelIndex&)const override
    {
        return QSize(98, 170);
    }
    void paint(QPainter* p, const QStyleOptionViewItem& opt, const QModelIndex& index) const override
    {
        p->setRenderHint(QPainter::Antialiasing);
		static QIcon ic[] = {QIcon(":/image/bottle_gray.png"),QIcon(":/image/bottle_white.png"), QIcon(":/image/bottle_orange.png")
			, QIcon(":/image/bottle_sky.png"), QIcon(":/image/bottle_green.png"), QIcon(":/image/bottle_red.png"), };

		int idx=0;
		if (auto bt = index.data(Qt::UserRole + 1).value<BottleStruct*>())
		{
            switch (bt->getFlag())
            {
            case B_CanUse:
                idx = 1; break;
            case B_WaitStart:
                idx = 2; break;
            case B_Using:
                idx = 3; break;
            case B_Used:
                idx = 4; break;
            case B_Error:
                idx = 5; break;
            }
		}
		ic[idx].paint(p, opt.rect);
		p->drawText(opt.rect, Qt::AlignCenter, index.data(Qt::DisplayRole).toString());
	}
};

class TubeDelegate : public QItemDelegate
{
public:
    explicit TubeDelegate(QObject* p = nullptr) : QItemDelegate(p) {}

    QSize sizeHint(const QStyleOptionViewItem&, const QModelIndex&)const override
    {
        return QSize(80, 80);
    }
    void paint(QPainter* p, const QStyleOptionViewItem& opt, const QModelIndex& index) const override
    {
        p->setRenderHint(QPainter::Antialiasing);
        QColor col(Qt::lightGray);
        if (auto tube = index.data(Qt::UserRole + 1).value<TubeStruct*>())
        {
			switch (tube->getFlag())
			{
			case T_WaitPrepare:
				col = Qt::white; break;
			case T_Prepared:
			case T_WaitFix:
			case T_Fixed:
			case T_WaitRecycle:
				col = QColor("#ffbf00"); break;
			case T_Preparing:
			case T_Fixing:
			case T_Recycling:
				col = QColor("#00c0f0"); break;
			case T_Recyced:
				col = Qt::darkGreen; break;
			default:
				break;
			}
        }
        p->setBrush(col);
        p->drawEllipse(opt.rect.adjusted(2, 2, -2, -2));
        p->drawText(opt.rect, Qt::AlignCenter, index.data(Qt::DisplayRole).toString());
    }
};
/*
* MaterialStore
*/
MaterialStore::MaterialStore(QWidget *parent) : QWidget(parent)
, m_ui(new Ui::MaterialStore), m_robot(new RobotMgr(this))
{
    m_ui->setupUi(this);
    m_ui->table_material->setColumnWidth(0, 55);
    m_ui->table_material->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
    m_ui->table_material->setColumnWidth(1, 150);
    m_ui->table_material->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Fixed);
    m_ui->table_material->setColumnWidth(2, 105);
    m_ui->table_material->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Fixed);
    m_ui->table_material->setColumnWidth(3, 70);
    m_ui->table_material->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Fixed);
    m_ui->table_material->setColumnWidth(4, 90);
    m_ui->table_material->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Fixed);

    initUi();
    initFeederDecode();
}

MaterialStore::~MaterialStore()
{
    delete m_ui;
}

RobotMgr* MaterialStore::GetRobotMgr()const
{
    return m_robot;
}

void MaterialStore::updateStore(const StoreStruct* c)
{
    auto str = c->nfcid.isEmpty() ? QString::number(c->numb + 1) : 
        QString("%1\n%2\n%3 克").arg(c->numb + 1)
        .arg(c->pMate ? c->pMate->name : "nil")
        .arg(c->pMate ? QString::number(c->pMate->weight) : "nil");
    for (int i = 0; i < m_ui->list_container->count(); ++i)
    {
        if (auto item = m_ui->list_container->item(i))
        {
            if (item->data(Qt::UserRole + 1).value<const StoreStruct*>() == c)
            {
                item->setText(str);
                m_ui->list_container->update();
                return;
            }
        }
    }
    auto ite = new QListWidgetItem(str);
    ite->setData(Qt::UserRole + 1, QVariant::fromValue(c));
    m_ui->list_container->addItem(ite);
}

void MaterialStore::initFeederDecode()
{
    m_ui->list_container->setItemDelegate(new ContainerDelegate(m_ui->list_container));
    m_ui->listWidget->setItemDelegate(new BottleDelegate(m_ui->listWidget));
    for (auto itr : FeederMgr::Instance().AllBottles())
    {
        auto ite = new QListWidgetItem(QString::number(itr->m_numb+1));
        ite->setData(Qt::UserRole + 1, QVariant::fromValue(itr));
        m_ui->listWidget->addItem(ite);
    }
    m_ui->listWidget_2->setItemDelegate(new TubeDelegate(m_ui->listWidget_2));
    for (auto itr : FeederMgr::Instance().AllTubes())
    {
        auto ite = new QListWidgetItem(QString::number(itr->getNumber()+1));
        ite->setData(Qt::UserRole + 1, QVariant::fromValue(itr));
        m_ui->listWidget_2->addItem(ite);
    }
    connect(&FeederMgr::Instance(), &FeederMgr::materialAdded, m_ui->table_material, &MaterialTableWidget::AddMaterial);
    connect(&FeederMgr::Instance(), &FeederMgr::materialChanged, m_ui->table_material, &MaterialTableWidget::ChangeMaterial);
    connect(&FeederMgr::Instance(), &FeederMgr::storeChanged, this, &MaterialStore::updateStore);
    connect(&FeederMgr::Instance(), &FeederMgr::bottleChanged, this, [=] {m_ui->listWidget->update(); });
    connect(&FeederMgr::Instance(), &FeederMgr::tubeChanged, this, [=] {m_ui->listWidget_2->update(); });

    for (int i = 0; ; i++)
    {
        if (auto c = FeederMgr::Instance().GetStore(i))
            updateStore(c);
        else
            break;
    }
}

void MaterialStore::initUi()
{
    connect(m_ui->list_container, &QListWidget::itemClicked, this, [=](QListWidgetItem* item) {
        auto c = item->data(Qt::UserRole + 1).value<const StoreStruct*>();
        if (!c || c->nfcid.isEmpty())
            return;
        DlgMaterialModify dlg(this);
        auto title = tr("料斗%1配料").arg(c->numb + 1);
        dlg.setWindowTitle(title);
        dlg.Modify(c->pMate, c->nfcid);
        if (dlg.exec() == QDialog::Accepted)
            FeederMgr::Instance().AddMaterial(c->nfcid, dlg.GetName(), dlg.GetWeight());
    });

    connect(m_ui->listWidget, &QListWidget::itemClicked, this, [=](QListWidgetItem* item) {
        auto bt = item->data(Qt::UserRole + 1).value<BottleStruct*>();
        if (bt->getFlag() == B_Used)
            return;
        DlgFeedMaterial dlg(this);
        dlg.Init(FeederMgr::Instance().GetfeedParamsByBottleNum(bt->m_numb));
        dlg.SetBottleAvlible(bt->getFlag() == T_WaitPrepare);
        dlg.setWindowTitle(tr("料瓶%1投料").arg(bt->m_numb + 1));
        connect(&dlg, &DlgFeedMaterial::bottleAvlibleChanged, this, [=](bool b) {
            bt->setFlag(b ? B_CanUse : B_None);
            m_ui->listWidget->update();
        });
        if (QDialog::Accepted == dlg.exec())
        {
            QMap<QString, float> mates;
            if (dlg.FeedBottle(&mates) && !mates.isEmpty())
            {
				auto idx = dlg.GetChannel();
				FeederMgr::Instance().FeedSolidMaterial(mates, (int)bt->m_numb, dlg.GetTubeNumb(), idx < 0 ? false : true, idx);
                m_ui->listWidget->update();
            }
        }
    });
    connect(m_ui->listWidget_2, &QListWidget::itemClicked, this, [=](QListWidgetItem* item) {
        auto tb = item->data(Qt::UserRole+1).value<TubeStruct*>();
        if (tb->getFlag()==T_Fixed)
        {
            DlgTubeBack dlg(this);
            dlg.Init(tb);
            dlg.exec();
            return;
        }

        if (T_None!=tb->getFlag() && T_WaitPrepare!=tb->getFlag())
            return;

        tb->setFlag(tb->getFlag() == T_None ? T_WaitPrepare : T_None);
    });
    connect(&FeederMgr::Instance(), &FeederMgr::connectStatChanged, this, [=](FeederMgr::PortStat st) {
        QString strIcon = ":/stateBar/image/closed.png";
        switch (st)
        {
        case FeederMgr::NoData:
            strIcon = ":/stateBar/image/disconnected.png";
            break;
        case FeederMgr::Communicate:
            strIcon = ":/stateBar/image/connected.png";
            break;
        default:
            break;
        }
        m_ui->btn_com->setIcon(QIcon(strIcon));
    });
    connect(m_ui->btn_com, &QPushButton::clicked, this, [=] {
        DlgSerialSettings dlg(this);
        dlg.Inital(FeederMgr::Instance().serialPort());
        if (dlg.exec() == QDialog::Accepted)
            FeederMgr::Instance().ConnectPort();
    });

    connect(m_robot, &RobotMgr::connectStatChanged, this, [=](RobotMgr::RobotStat st) {
        QString strIcon = ":/stateBar/image/closed.png";
        switch (st)
        {
        case RobotMgr::NoData:
            strIcon = ":/stateBar/image/disconnected.png";
            break;
        case RobotMgr::Communicate:
        case RobotMgr::PowerOff:
        case RobotMgr::PowerOn:
        case RobotMgr::RobotStart:
        case RobotMgr::ProgmaStart:
            strIcon = ":/stateBar/image/connected.png";
            break;
        default:
            break;
        }
        m_ui->btn_robot->setIcon(QIcon(strIcon));
    });
    connect(m_ui->btn_robot, &QPushButton::clicked, this, [=] {
        DlgSocketSettings dlg(this);
        dlg.Inital(m_robot->GetHost(), m_robot->GetPort());
        if (dlg.exec() == QDialog::Accepted)
            m_robot->ConnectSocket(dlg.GetHost(), dlg.GetPort());
    });
    connect(m_ui->btn_balance, &QPushButton::clicked, this, [=] {m_ui->stackedWidget->setCurrentWidget(m_ui->demo); });
    connect(m_ui->btn_list, &QPushButton::clicked, this, [=] {m_ui->stackedWidget->setCurrentWidget(m_ui->page); });
}
