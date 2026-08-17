#include "stovetable.h"
#pragma execution_character_set("utf-8")

StoveTable::StoveTable(QWidget *parent) : QWidget(parent)
{
    p_col = 3;
    p_row = PRO_CNT;
    tableWidget = new QTableWidget(p_row,p_col);   //  3列 PRO_CNT行
    tableWidget->setParent(this);
    tableWidget->resize(364, 450); //设置表格大小
    tableWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);   //  水平滑动条不可见
    tableWidget->horizontalHeader()->setSectionsClickable(false);//水平表头不可点击
}

//void StoveTable::setTableHeader(QStringList header)
//{
//    tableWidget->setHorizontalHeaderLabels(header);
//}

void StoveTable::setTableFormat(int col, int row)
{
    p_col = col;
    p_row = row;
    tableWidget->setColumnCount(col);
    tableWidget->setRowCount(row);

}
//  数据发送出去需要扩大10倍
void StoveTable::getTableTemp(QList<int> *data)
{
      int i=0;
      data->clear();
      for(i=0;i<p_row;i++)
      {
         data->append(edit[0][i]->text().toFloat()*10);   //温度
         data->append(edit[1][i]->text().toInt()*10);   //时间
      }
}
//  保存时不需要扩大10倍
void StoveTable::getTableTemp2(QList<int> *data)
{
    int i=0;
    data->clear();
    for(i=0;i<p_row;i++)
    {
       data->append(edit[0][i]->text().toInt());   //温度
       data->append(edit[1][i]->text().toInt());   //时间
    }
}

void StoveTable::setTabelTemp(int *temp, int *time)
{
    int i;
    for(i=0;i<p_row;i++)
    {
          edit[0][i]->setText(QString::number(temp[i]));// 温度
          edit[1][i]->setText(QString::number(time[i]));// 时间
    }
}

void StoveTable::saveTableTemp(void)
{
    QList<int> userdata;
    getTableTemp2(&userdata);
    for(int i=0;i<PRO_CNT;i++)
    {
        table.temp[i] = userdata.at(2*i);
        table.time[i] = userdata.at(2*i+1);
    }
    file.open(QFile::ReadWrite);
    //qDebug() << "文件路径:" << file.fileName();
    file.write((const char *)&table,sizeof(table));
    file.close();
}

void StoveTable::Table_init()
{
    int i;
    tableWidget->verticalHeader()->hide(); //设置垂直头不可见
    QStringList header;
    header<<"程序段"<<"温度(℃)"<<"时间(min)";
    tableWidget->setHorizontalHeaderLabels(header);
    tableWidget->horizontalHeader()->setStyleSheet("QHeaderView::section{background:rgb(128,128,128);"
                                                   "font-size:24px;"
                                                   "min-height:40px;"
                                                   "font-weight:bold;"
                                                   "color:white;}");  //设置表头样式
    tableWidget->setColumnWidth(0, 100);  //  设置列宽度
    tableWidget->setColumnWidth(1, 130);
    tableWidget->setColumnWidth(2, 130);
    tableWidget->horizontalHeader()->setSectionResizeMode(0,QHeaderView::Fixed);
    tableWidget->horizontalHeader()->setSectionResizeMode(1,QHeaderView::Fixed);
    tableWidget->horizontalHeader()->setSectionResizeMode(2,QHeaderView::Fixed);

    //启用交替行颜色
    tableWidget->setAlternatingRowColors(true);
    //隐藏网格线
    tableWidget->setShowGrid(false);

    //隔行设置背景色
    tableWidget->setStyleSheet(
        "QTableWidget {"
        "    alternate-background-color: #c0c0c0;"
        "    background-color: white;"
        "    font-size:18px;"
        "    gridline-color: #d0d0d0;"  // 建议添加网格线颜色
        "}"
        "QTableWidget::item {"
        "    border: none;"
        "    padding: 2px;"
        "}"
    );

    for(i=0;i<p_row;i++)
    {

      tableWidget->setItem(i,0,new QTableWidgetItem(QString::number(i+1)));
      tableWidget->item(i,0)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
      tableWidget->item(i,0)->setFlags(Qt::ItemIsEditable);   //  设置不可编辑

      edit[0][i] = new tabelEdit(this,i,1);
      //edit[0][i]->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
      //edit[0][i]->setStyleSheet("border: none; background: transparent;");
      connect(edit[0][i],SIGNAL(edit_finished(int,int,float)),this,SLOT(edit_finished(int,int,float)));
      tableWidget->setCellWidget(i,1, edit[0][i]);

      edit[1][i] = new tabelEdit(this,i,2);
      //edit[1][i]->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
      //edit[1][i]->setStyleSheet("border: none; background: transparent;");
      connect(edit[1][i],SIGNAL(edit_finished(int,int,float)),this,SLOT(edit_finished(int,int,float)));
      tableWidget->setCellWidget(i,2, edit[1][i]);

      tableWidget->setRowHeight(i,40);   //设置行高
    }

    QString path = QCoreApplication::applicationDirPath()+"/user";
    QDir dir(path);
    if(!dir.exists())   //  检查目录是否存在
    {
       dir.mkdir(path);
    }
    path = path + "/" + this->objectName();
    file.setFileName(path);    //  读取文件中的数据
    if(file.exists())   //  如果文件存在
    {
        file.open(QFile::ReadWrite);
        file.read((char *)&table,sizeof(table));
        file.close();
    }
    else
    {
        file.open(QFile::ReadWrite);
        file.write((char *)&table,sizeof(table));
        file.close();
    }
    setTabelTemp(&table.temp[0],&table.time[0]);   //  将数据显示出来
}

int StoveTable::get_col()
{
    return p_col;
}

int StoveTable::get_row()
{
    return p_row;
}

void StoveTable::edit_finished(int row, int col, float val)
{
   stove_refresh(row,col,val);
}
