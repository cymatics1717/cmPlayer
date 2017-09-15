#include "deviceconfig.h"
#include "ui_deviceconfig.h"

deviceConfig::deviceConfig(QWidget *parent) : QDialog(parent),ui(new Ui::deviceConfig)
{
    ui->setupUi(this);
    devlist<<"wayne"<<"sdf";
    ui->listWidget->addItems(devlist);
}

deviceConfig::~deviceConfig()
{
    delete ui;
}

QString deviceConfig::aliasName() const
{
    return ui->aliasTitleLineEdit->text();
}

QString deviceConfig::ip() const
{
    return ui->iPLineEdit->text();
}

quint64 deviceConfig::port() const
{
    return ui->portLineEdit->text().toUInt();
}

QString deviceConfig::username() const
{
    return ui->userNameLineEdit->text();
}

QString deviceConfig::password() const
{
    return ui->passwordLineEdit->text();
}
