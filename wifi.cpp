#include "wifihandler.h"
#include <QProcess>
#include <QDebug>

WifiHandler::WifiHandler(QObject *parent) : QObject(parent) {}

bool WifiHandler::wifiOn() const { return m_wifiOn; }

void WifiHandler::setWifiOn(bool on)
{
    m_wifiOn = on;

    if (on)
        QProcess::execute("rfkill unblock wifi");
    else
        QProcess::execute("rfkill block wifi");

    emit wifiOnChanged(on);
    emit connectionStatusChanged(isConnected());
}

QStringList WifiHandler::scanWifi()
{
    QStringList result;
    QProcess process;
    process.start("bash", QStringList() << "-c" << "nmcli -t -f SSID dev wifi");
    process.waitForFinished();
    QString output = process.readAllStandardOutput();

    for (const QString &line : output.split("\n"))
        if (!line.trimmed().isEmpty())
            result << line.trimmed();

    emit wifiScanCompleted(result);
    return result;
}

bool WifiHandler::connectToWifi(const QString &ssid, const QString &password)
{
    QString cmd = QString("nmcli dev wifi connect \"%1\" password \"%2\"")
                      .arg(ssid, password);

    int ret = QProcess::execute(cmd);
    emit connectionStatusChanged(isConnected());
    return (ret == 0);
}

QString WifiHandler::getIpAddress()
{
    QProcess p;
    // Run ip -4 addr show wlan0 and extract the IPv4 address
    p.start("bash", QStringList() << "-c" 
             << "ip -4 addr show wlan0 | awk '/inet / {print $2}'");
    p.waitForFinished();
    QString ip = p.readAllStandardOutput().trimmed();
    return ip.isEmpty() ? "N/A" : ip;
}


QString WifiHandler::getConnectedSSID()
{
    QProcess p;
    p.start("bash", QStringList() << "-c" << "nmcli -t -f ACTIVE,SSID dev wifi | grep '^yes' | cut -d: -f2");
    p.waitForFinished();
    return p.readAllStandardOutput().trimmed();
}

bool WifiHandler::isConnected()
{
    return !getConnectedSSID().isEmpty();
}
