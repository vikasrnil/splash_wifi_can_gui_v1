#pragma once
#include <QObject>
#include <QStringList>

class WifiHandler : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool connected READ isConnected NOTIFY connectionStatusChanged)
    Q_PROPERTY(bool wifiOn READ wifiOn WRITE setWifiOn NOTIFY wifiOnChanged)

public:
    explicit WifiHandler(QObject *parent = nullptr);

    Q_INVOKABLE QStringList scanWifi();
    Q_INVOKABLE bool connectToWifi(const QString &ssid, const QString &password);
    Q_INVOKABLE QString getIpAddress();
    Q_INVOKABLE QString getConnectedSSID();

    bool isConnected();
    bool wifiOn() const;
    void setWifiOn(bool on);

signals:
    void wifiOnChanged(bool);
    void connectionStatusChanged(bool);
    void wifiScanCompleted(QStringList);

private:
    bool m_wifiOn = true;
};
