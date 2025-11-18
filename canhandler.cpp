#include "canhandler.h"

CanHandler::CanHandler(QObject *parent) : QObject(parent)
{
    m_cleanupTimer = new QTimer(this);
    connect(m_cleanupTimer, &QTimer::timeout, this, &CanHandler::cleanupOldData);
    m_cleanupTimer->start(1000);
}

void CanHandler::updateCanData(const QString &id, const QString &data)
{
    qint64 now = QDateTime::currentMSecsSinceEpoch();

    CanInfo &info = m_canData[id];
    if (info.lastTimestamp > 0)
        info.cycleTime = now - info.lastTimestamp;

    info.data = data;
    info.lastTimestamp = now;

    // Prepare display string: show "data (X ms)"
    QString displayStr = info.data;
    if (info.cycleTime > 0)
        displayStr += QString("  (%1 ms)").arg(info.cycleTime);

    m_displayMap[id] = displayStr;

    emit canDataChanged();
}

void CanHandler::cleanupOldData()
{
    const qint64 now = QDateTime::currentMSecsSinceEpoch();
    const qint64 timeoutMs = 3000;

    QList<QString> toRemove;
    for (auto it = m_canData.constBegin(); it != m_canData.constEnd(); ++it) {
        if (now - it.value().lastTimestamp > timeoutMs)
            toRemove.append(it.key());
    }

    bool removed = false;
    for (const QString &id : toRemove) {
        m_canData.remove(id);
        m_displayMap.remove(id);
        removed = true;
    }

    if (removed)
        emit canDataChanged();
}

void CanHandler::clearData()
{
    m_canData.clear();
    m_displayMap.clear();
    emit canDataChanged();
}
