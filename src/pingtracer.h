#ifndef PINGTRACER_H
#define PINGTRACER_H

#include <QObject>
#include <QTimer>
#include <QThread>
#include <QMutex>
#include <QHostInfo>
#include <QString>
#include <QList>
#include "networktester.h"

struct HopData {
    int hopNumber;
    QString hostname;
    QString ipAddress;
    int sent;
    int received;
    double bestTime;
    double avgTime;
    double worstTime;
    QList<double> responseTimes;
    
    HopData() : hopNumber(0), sent(0), received(0), bestTime(-1), avgTime(-1), worstTime(-1) {}
};

class PingTracer : public QObject
{
    Q_OBJECT

public:
    explicit PingTracer(QObject *parent = nullptr);
    ~PingTracer();
    
    // Configuration
    void setTarget(const QString& host);
    void setInterval(int intervalMs);
    void setTimeout(int timeoutMs);
    void setMaxHops(int maxHops);
    
    // Control
    bool start();
    void stop();
    bool isRunning() const;
    
    // Data access
    QList<HopData> getHopData() const;
    QString getTarget() const;

signals:
    void hopDataUpdated(const QList<HopData>& hops);
    void errorOccurred(const QString& error);
    void finished();

private slots:
    void performTrace();
    void onHostLookupFinished(const QHostInfo& hostInfo);
    void onNetworkTestResult(int hop, const NetworkTestResult& result);

private:
    void resolveTarget();
    void resetData();
    void updateStatistics();
    void startTraceroute();
    
    // Configuration
    QString m_targetHost;
    QString m_targetIP;
    int m_interval;
    int m_timeout;
    int m_maxHops;
    
    // State
    bool m_running;
    bool m_resolving;
    QTimer* m_traceTimer;
    QMutex m_dataMutex;
    
    // Data
    QList<HopData> m_hopData;
    int m_currentHop;
    int m_lookupId;
    
    // Network testing
    QList<NetworkTester*> m_networkTesters;
    QThread* m_networkThread;
};

#endif // PINGTRACER_H
