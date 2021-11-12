//#include "mainwindow.h"
#include "modbustcpclient.h"
#include "modbustcpclient_p.h"

QT_USE_NAMESPACE

ModbusTcpClient::ModbusTcpClient(QObject *parent)
    : QModbusTcpClient(*new ModbusTcpClientPrivate, parent)
{
}

ModbusTcpClient::ModbusTcpClient(ModbusTcpClientPrivate &dd, QObject *parent)
    : QModbusTcpClient(dd, parent)
{
}

void ModbusTcpClient::valueChanged(int value)
{
    //    Q_UNUSED(value)
    Q_D(ModbusTcpClient);

//        if (auto instance = MainWindow::instance()) {
//            d->m_tId = quint16(instance->ti1Spin->value() << 8) | quint8(instance->ti2Spin->value());
//            d->m_pId = quint16(instance->pi1Spin->value() << 8) | quint8(instance->pi2Spin->value());
//            d->m_length = quint16(instance->l1Spin->value() << 8) | quint8(instance->l2Spin->value());
//            d->m_uId = quint8(instance->ui1Spin->value());
//        }

    if(d->m_tId < 65535)
    {
        d->m_tId++;
    }
    else
    {
        d->m_tId = 0;
    }
    d->m_pId = 0;
    d->m_length = value;
    d->m_uId = 1;
}
