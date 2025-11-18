#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QSocketNotifier>
#include <QTimer>
#include <QDebug>
#include "canhandler.h"
#include "wifihandler.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <net/if.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <cstring>
#include <cstdlib>

class CanManager : public QObject {
    Q_OBJECT
public:
    explicit CanManager(CanHandler *handler, QObject *parent = nullptr)
        : QObject(parent), m_handler(handler), m_socket(-1), m_notifier(nullptr)
    {
        QTimer *timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, &CanManager::checkCanInterface);
        timer->start(2000);
    }

private slots:
    void checkCanInterface()
    {
        const char *ifname = "can0";

        bool canExists = (system("ip link show can0 > /dev/null 2>&1") == 0);
        if (!canExists) {
            // PCAN physically removed — cleanup
            if (m_socket >= 0) {
                qDebug() << "PCAN removed. Closing socket...";
                cleanupSocket();
                m_handler->clearData();
            }
            return;
        }

        if (m_socket < 0) {
            qDebug() << "Trying to bring CAN up...";
            system("ip link set can0 up type can bitrate 250000 2>/dev/null");

            m_socket = socket(PF_CAN, SOCK_RAW, CAN_RAW);
            if (m_socket < 0) {
                perror("Socket open failed");
                return;
            }

            struct ifreq ifr {};
            strncpy(ifr.ifr_name, ifname, IFNAMSIZ - 1);
            if (ioctl(m_socket, SIOCGIFINDEX, &ifr) < 0) {
                perror("Interface not found yet");
                cleanupSocket();
                return;
            }

            struct sockaddr_can addr {};
            addr.can_family = AF_CAN;
            addr.can_ifindex = ifr.ifr_ifindex;

            if (bind(m_socket, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
                perror("Bind failed");
                cleanupSocket();
                return;
            }

            fcntl(m_socket, F_SETFL, O_NONBLOCK);
            qDebug() << "✅ CAN interface ready.";

            m_notifier = new QSocketNotifier(m_socket, QSocketNotifier::Read, this);
            connect(m_notifier, &QSocketNotifier::activated, this, &CanManager::readCanData);
        }
    }

    void readCanData()
    {
        if (m_socket < 0)
            return;

        struct can_frame frame;
        ssize_t nbytes = read(m_socket, &frame, sizeof(struct can_frame));
        if (nbytes > 0) {
            QString idStr = QString("0x%1").arg(frame.can_id & CAN_SFF_MASK, 3, 16, QLatin1Char('0')).toUpper();
            QString dataStr;
            for (int i = 0; i < frame.can_dlc; ++i)
                dataStr += QString("%1 ").arg(frame.data[i], 2, 16, QLatin1Char('0')).toUpper();
            m_handler->updateCanData(idStr, dataStr.trimmed());
        }
    }

private:
    void cleanupSocket()
    {
        if (m_notifier) {
            m_notifier->deleteLater();
            m_notifier = nullptr;
        }
        if (m_socket >= 0) {
            close(m_socket);
            m_socket = -1;
        }
    }

    CanHandler *m_handler;
    int m_socket;
    QSocketNotifier *m_notifier;
};

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    CanHandler handler;
    WifiHandler wifi;   // <-- your WiFi handler class

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("canHandler", &handler);
    engine.rootContext()->setContextProperty("wifi", &wifi);  // <-- expose to QML
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

   CanManager manager(&handler);
    return app.exec();
    
}

#include "main.moc"
