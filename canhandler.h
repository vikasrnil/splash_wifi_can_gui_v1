#pragma once
#include <QObject>
#include <QVariant>
#include <QMap>
#include <QTimer>
#include <QDateTime>

struct CanInfo {
    QString data;
    qint64 lastTimestamp = 0;
    qint64 cycleTime = 0;  // in milliseconds
};

class CanHandler : public QObject
{
    Q_OBJECT
public:
    explicit CanHandler(QObject *parent = nullptr);

    Q_INVOKABLE void updateCanData(const QString &id, const QString &data);
    Q_INVOKABLE void clearData();

    Q_PROPERTY(QVariantMap canMap READ canMap NOTIFY canDataChanged)
    QVariantMap canMap() const { return m_displayMap; }

signals:
    void canDataChanged();

private slots:
    void cleanupOldData();

private:
    QMap<QString, CanInfo> m_canData;     // full internal data
    QVariantMap m_displayMap;             // for QML display
    QTimer *m_cleanupTimer;
};
