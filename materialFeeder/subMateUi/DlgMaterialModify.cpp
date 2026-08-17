#include "DlgMaterialModify.h"
#include <QCheckBox>
#include "../FeederDecoder.h"

#include "ui_DlgMaterialModify.h"
#pragma execution_character_set("utf-8")

DlgMaterialModify::DlgMaterialModify(QWidget *parent) : QDialog(parent)
, m_ui(new Ui::DlgMaterialModify)
{
    m_ui->setupUi(this);
    initUi();
}

DlgMaterialModify::~DlgMaterialModify()
{
    delete m_ui;
}

void DlgMaterialModify::Modify(const MaterialStruct* m, const QString &id)
{
    if (m || !id.isEmpty())
    {
        m_ui->stackedWidget->setCurrentWidget(m_ui->page_2);
        m_ui->cmb_id->setCurrentText(m ? m->nfcid : id);
        m_ui->lb_id->setText(m ? m->nfcid : id);
        if (m)
        { 
            m_ui->edit_name->setText(m->name);
            m_ui->spinBox->setValue(m->weight);
        }

        m_ui->cmb_id->clear();
        m_ui->cmb_id->addItems(FeederDecoder::Instance().GetStoreNfcId(false, m ? m->nfcid : id));
    }
}

QString DlgMaterialModify::GetId()const
{
    return m_ui->cmb_id->currentText();
}

QString DlgMaterialModify::GetName()const
{
    return m_ui->edit_name->text();
}

double DlgMaterialModify::GetWeight()const
{
    return m_ui->spinBox->value();
}

void DlgMaterialModify::GetMaterial(MaterialStruct* m)const
{
    if (!m)
        return;

    m->nfcid = m_ui->cmb_id->currentText();
    m->name = m_ui->edit_name->text();
    m->weight = m_ui->spinBox->value();
}

void DlgMaterialModify::initUi()
{
    m_ui->cmb_id->addItems(FeederDecoder::Instance().GetStoreNfcId(false));
    connect(m_ui->btn_cancle, &QPushButton::clicked, this, &QDialog::reject);
    connect(m_ui->btn_ok, &QPushButton::clicked, this, &QDialog::accept);
}