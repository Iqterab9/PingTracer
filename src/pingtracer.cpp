#include "pingtracer.h"
#include <QHostInfo>
#include <QNetworkInterface>
#include <QProcess>
#include <QRegularExpression>
#include <QDebug>

PingTracer::PingTracer(QObject *parent)
    : QObject(parent)
    , m_interval(1000)
    , m_timeout(5000)
    , m_maxHops(30)
    , m_running(false)
    , m_resolving(false)
    , m_currentHop(1)
    , m_lookupId(-1)
{
    m_traceTimer = new QTimer(this);
    m_traceTimer->setSingleShot(false);
    connect(m_traceTimer, &QTimer::timeout, this, &PingTracer::performTrace);
    
    m_networkThread = new QThread(this);
    m_networkThread->start();
}

PingTracer::~PingTracer()
{
    stop();
    if (m_networkThread) {
        m_networkThread->quit();
        m_networkThread->wait();
    }
}

void PingTracer::setTarget(const QString& host)
{
    if (!m_running) {
        m_targetHost = host;
        m_targetIP.clear();
    }
}

void PingTracer::setInterval(int intervalMs)
{
    m_interval = qMax(100, intervalMs);
    if (m_running) {
        m_traceTimer->setInterval(m_interval);
    }
}

void PingTracer::setTimeout(int timeoutMs)
{
    m_timeout = qMax(500, timeoutMs);
}

void PingTracer::setMaxHops(int maxHops)
{
    m_maxHops = qMax(1, qMin(64, maxHops));
}

bool PingTracer::start()
{
    if (m_running || m_targetHost.isEmpty()) {
        return false;
    }
    
    resetData();
    
    // First resolve the target hostname
    resolveTarget();
    return true;
}

void PingTracer::stop()
{
    if (!m_running) {
        return;
    }
    
    m_running = false;
    m_traceTimer->stop();
    
    // Clean up network testers
    for (NetworkTester* tester : m_networkTesters) {
        tester->stopTest();
        tester->deleteLater();
    }
    m_networkTesters.clear();
    
    emit finished();
}

bool PingTracer::isRunning() const
{
    return m_running;
}

QList<HopData> PingTracer::getHopData() const
{
    QMutexLocker locker(&m_dataMutex);
    return m_hopData;
}

QString PingTracer::getTarget() const
{
    return m_targetHost;
}

void PingTracer::resolveTarget()
{
    m_resolving = true;
    
    // Check if target is already an IP address
    QHostAddress addr(m_targetHost);
    if (!addr.isNull()) {
        m_targetIP = m_targetHost;
        startTraceroute();
        return;
    }
    
    // Resolve hostname
    m_lookupId = QHostInfo::lookupHost(m_targetHost, this, 
                                       SLOT(onHostLookupFinished(QHostInfo)));
}

void PingTracer::onHostLookupFinished(const QHostInfo& hostInfo)
{
    m_resolving = false;
    
    if (hostInfo.lookupId() != m_lookupId) {
        return; // Not our lookup
    }
    
    if (hostInfo.error() != QHostInfo::NoError) {
        emit errorOccurred(QString("Failed to resolve hostname: %1").arg(hostInfo.errorString()));
        return;
    }
    
    QList<QHostAddress> addresses = hostInfo.addresses();
    if (addresses.isEmpty()) {
        emit errorOccurred("No IP addresses found for hostname");
        return;
    }
    
    // Prefer IPv4 addresses
    for (const QHostAddress& addr : addresses) {
        if (addr.protocol() == QAbstractSocket::IPv4Protocol) {
            m_targetIP = addr.toString();
            break;
        }
    }
    
    if (m_targetIP.isEmpty()) {
        m_targetIP = addresses.first().toString();
    }
    
    startTraceroute();
}

void PingTracer::startTraceroute()
{
    if (m_targetIP.isEmpty()) {
        emit errorOccurred("No target IP address available");
        return;
    }
    
    m_running = true;
    m_currentHop = 1;
    
    // Initialize hop data
    QMutexLocker locker(&m_dataMutex);
    m_hopData.clear();
    for (int i = 0; i < m_maxHops; ++i) {
        HopData hop;
        hop.hopNumber = i + 1;
        hop.hostname = "---";
        hop.ipAddress = "---";
        m_hopData.append(hop);
    }
    locker.unlock();
    
    // Start the tracing timer
    m_traceTimer->start(m_interval);
    
    // Perform initial trace
    performTrace();
}

void PingTracer::performTrace()
{
    if (!m_running) {
        return;
    }
    
    // Create network testers for each hop (simulate traceroute behavior)
    // In a real implementation, this would use raw sockets with TTL manipulation
    // For this demo, we'll simulate the behavior
    
    for (int hop = 1; hop <= qMin(m_currentHop + 3, m_maxHops); ++hop) {
        if (hop > m_networkTesters.size()) {
            NetworkTester* tester = new NetworkTester();
            tester->moveToThread(m_networkThread);
            tester->setTimeout(m_timeout);
            
            connect(tester, &NetworkTester::testCompleted,
                    this, &PingTracer::onNetworkTestResult);
            
            m_networkTesters.append(tester);
        }
        
        if (hop <= m_networkTesters.size()) {
            NetworkTester* tester = m_networkTesters[hop - 1];
            
            // Simulate different IP addresses for different hops
            QString hopIP = simulateHopIP(hop);
            tester->setTarget(hopIP, hop);
            
            QMetaObject::invokeMethod(tester, "startTest", Qt::QueuedConnection);
        }
    }
    
    if (m_currentHop < m_maxHops) {
        m_currentHop++;
    }
}

QString PingTracer::simulateHopIP(int hop)
{
    // This is a simulation - in real implementation, this would come from actual traceroute
    QStringList targetParts = m_targetIP.split('.');
    if (targetParts.size() == 4) {
        // Simulate intermediate hops
        if (hop < m_maxHops) {
            return QString("192.168.%1.%2").arg(hop).arg((hop * 7) % 255 + 1);
        } else {
            return m_targetIP; // Final destination
        }
    }
    return m_targetIP;
}

void PingTracer::onNetworkTestResult(int hop, const NetworkTestResult& result)
{
    if (!m_running || hop < 1 || hop > m_hopData.size()) {
        return;
    }
    
    QMutexLocker locker(&m_dataMutex);
    
    HopData& hopData = m_hopData[hop - 1];
    hopData.hopNumber = hop;
    hopData.ipAddress = result.ipAddress;
    hopData.sent++;
    
    if (result.success && result.responseTime >= 0) {
        hopData.received++;
        hopData.responseTimes.append(result.responseTime);
        
        // Update statistics
        if (hopData.bestTime < 0 || result.responseTime < hopData.bestTime) {
            hopData.bestTime = result.responseTime;
        }
        
        if (hopData.worstTime < 0 || result.responseTime > hopData.worstTime) {
            hopData.worstTime = result.responseTime;
        }
        
        // Calculate average
        double sum = 0;
        for (double time : hopData.responseTimes) {
            sum += time;
        }
        hopData.avgTime = sum / hopData.responseTimes.size();
        
        // Resolve hostname if not already done
        if (hopData.hostname == "---" && !result.hostname.isEmpty()) {
            hopData.hostname = result.hostname;
        } else if (hopData.hostname == "---") {
            // Try to resolve hostname
            QHostInfo::lookupHost(result.ipAddress, [this, hop](const QHostInfo& info) {
                if (info.error() == QHostInfo::NoError && !info.hostName().isEmpty()) {
                    QMutexLocker locker(&m_dataMutex);
                    if (hop <= m_hopData.size()) {
                        m_hopData[hop - 1].hostname = info.hostName();
                    }
                }
            });
        }
    }
    
    locker.unlock();
    
    // Emit updated data
    emit hopDataUpdated(getHopData());
}

void PingTracer::resetData()
{
    QMutexLocker locker(&m_dataMutex);
    m_hopData.clear();
    m_currentHop = 1;
}

void PingTracer::updateStatistics()
{
    // Statistics are updated in onNetworkTestResult
}
