#ifndef NETWORKTESTER_H
#define NETWORKTESTER_H

#include <QObject>
#include <QTimer>
#include <QUdpSocket>
#include <QHostAddress>
#include <QDateTime>
#include <QThread>

struct NetworkTestResult {
    int hop;
    QString ipAddress;
    QString hostname;
    double responseTime; // -1 if timeout
    bool success;
    QString error;
    
    NetworkTestResult() : hop(0), responseTime(-1), success(false) {}
};

class NetworkTester : public QObject
{
    Q_OBJECT

public:
    explicit NetworkTester(QObject *parent = nullptr);
    ~NetworkTester();
    
    void setTarget(const QString& ip, int hop);
    void setTimeout(int timeoutMs);
    void startTest();
    void stopTest();
    
    bool isRunning() const;

signals:
    void testCompleted(int hop, const NetworkTestResult& result);
    void errorOccurred(const QString& error);

private slots:
    void onTimeout();
    void onSocketReadyRead();
    void onSocketError(QAbstractSocket::SocketError error);

private:
    void sendPing();
    void handleResponse();
    double calculateResponseTime();
    
    QString m_targetIP;
    int m_hop;
    int m_timeout;
    bool m_running;
    
    QUdpSocket* m_socket;
    QTimer* m_timeoutTimer;
    QDateTime m_sendTime;
    quint16 m_sequenceNumber;
    
    static quint16 s_globalSequence;
};

#endif // NETWORKTESTER_H
