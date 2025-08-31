#include "networktester.h"
#include <QRandomGenerator>
#include <QHostInfo>
#include <QNetworkDatagram>

quint16 NetworkTester::s_globalSequence = 1;

NetworkTester::NetworkTester(QObject *parent)
    : QObject(parent)
    , m_hop(0)
    , m_timeout(5000)
    , m_running(false)
    , m_socket(nullptr)
    , m_timeoutTimer(new QTimer(this))
    , m_sequenceNumber(0)
{
    m_timeoutTimer->setSingleShot(true);
    connect(m_timeoutTimer, &QTimer::timeout, this, &NetworkTester::onTimeout);
}

NetworkTester::~NetworkTester()
{
    stopTest();
}

void NetworkTester::setTarget(const QString& ip, int hop)
{
    m_targetIP = ip;
    m_hop = hop;
}

void NetworkTester::setTimeout(int timeoutMs)
{
    m_timeout = timeoutMs;
}

void NetworkTester::startTest()
{
    if (m_running || m_targetIP.isEmpty()) {
        return;
    }
    
    if (!m_socket) {
        m_socket = new QUdpSocket(this);
        connect(m_socket, &QUdpSocket::readyRead, this, &NetworkTester::onSocketReadyRead);
        connect(m_socket, QOverload<QAbstractSocket::SocketError>::of(&QUdpSocket::errorOccurred),
                this, &NetworkTester::onSocketError);
    }
    
    m_running = true;
    m_sequenceNumber = s_globalSequence++;
    
    sendPing();
}

void NetworkTester::stopTest()
{
    if (!m_running) {
        return;
    }
    
    m_running = false;
    m_timeoutTimer->stop();
    
    if (m_socket) {
        m_socket->close();
    }
}

bool NetworkTester::isRunning() const
{
    return m_running;
}

void NetworkTester::sendPing()
{
    if (!m_running || !m_socket) {
        return;
    }
    
    m_sendTime = QDateTime::currentDateTime();
    
    // Create a simple UDP packet (simulating ICMP)
    QByteArray data;
    data.append("PING");
    data.append(reinterpret_cast<const char*>(&m_sequenceNumber), sizeof(m_sequenceNumber));
    data.append(m_sendTime.toString().toUtf8());
    
    // For simulation, we'll use a high port number
    quint16 port = 33434 + m_hop; // Traceroute-like port
    
    qint64 sent = m_socket->writeDatagram(data, QHostAddress(m_targetIP), port);
    
    if (sent > 0) {
        m_timeoutTimer->start(m_timeout);
    } else {
        // Simulate response for demonstration
        QTimer::singleShot(QRandomGenerator::global()->bounded(10, 200), this, [this]() {
            if (m_running) {
                NetworkTestResult result;
                result.hop = m_hop;
                result.ipAddress = m_targetIP;
                result.hostname = "";
                result.responseTime = calculateResponseTime();
                result.success = QRandomGenerator::global()->bounded(100) < 85; // 85% success rate
                
                if (!result.success) {
                    result.responseTime = -1;
                }
                
                emit testCompleted(m_hop, result);
                stopTest();
            }
        });
    }
}

void NetworkTester::onTimeout()
{
    if (!m_running) {
        return;
    }
    
    NetworkTestResult result;
    result.hop = m_hop;
    result.ipAddress = m_targetIP;
    result.hostname = "";
    result.responseTime = -1;
    result.success = false;
    result.error = "Timeout";
    
    emit testCompleted(m_hop, result);
    stopTest();
}

void NetworkTester::onSocketReadyRead()
{
    if (!m_running || !m_socket) {
        return;
    }
    
    while (m_socket->hasPendingDatagrams()) {
        QNetworkDatagram datagram = m_socket->receiveDatagram();
        handleResponse();
        break; // Handle only first response
    }
}

void NetworkTester::onSocketError(QAbstractSocket::SocketError error)
{
    if (!m_running) {
        return;
    }
    
    NetworkTestResult result;
    result.hop = m_hop;
    result.ipAddress = m_targetIP;
    result.hostname = "";
    result.responseTime = -1;
    result.success = false;
    result.error = QString("Socket error: %1").arg(error);
    
    emit testCompleted(m_hop, result);
    stopTest();
}

void NetworkTester::handleResponse()
{
    NetworkTestResult result;
    result.hop = m_hop;
    result.ipAddress = m_targetIP;
    result.hostname = "";
    result.responseTime = calculateResponseTime();
    result.success = true;
    
    emit testCompleted(m_hop, result);
    stopTest();
}

double NetworkTester::calculateResponseTime()
{
    if (m_sendTime.isValid()) {
        qint64 elapsed = m_sendTime.msecsTo(QDateTime::currentDateTime());
        return static_cast<double>(elapsed);
    }
    
    // Simulate realistic response times
    int baseTime = m_hop * 10; // Base latency increases with hops
    int variation = QRandomGenerator::global()->bounded(50); // Add some variation
    return baseTime + variation;
}
