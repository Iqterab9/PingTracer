#include "mainwindow.h"
#include "exportmanager.h"
#include <QApplication>
#include <QMessageBox>
#include <QFileDialog>
#include <QClipboard>
#include <QHeaderView>
#include <QFont>
#include <QFontMetrics>
#include <QSplitter>
#include <QDateTime>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_pingTracer(nullptr)
    , m_updateTimer(new QTimer(this))
    , m_isRunning(false)
    , m_totalPacketsSent(0)
    , m_totalPacketsReceived(0)
{
    setupUI();
    setupMenus();
    setupToolbar();
    setupStatusBar();
    setupConnections();
    
    // Initialize ping tracer
    m_pingTracer = new PingTracer(this);
    
    // Connect ping tracer signals
    connect(m_pingTracer, &PingTracer::hopDataUpdated, 
            this, &MainWindow::onTracerouteUpdate);
    connect(m_pingTracer, &PingTracer::errorOccurred, 
            this, &MainWindow::onTracerouteError);
    
    // Initial state
    updateButtonStates();
    applyCurrentTheme();
    
    // Set window properties
    setWindowTitle("PingTracer by Harvey - www.iqterabharvey.me");
    setMinimumSize(800, 600);
    resize(1200, 800);
}

MainWindow::~MainWindow()
{
    if (m_pingTracer && m_pingTracer->isRunning()) {
        m_pingTracer->stop();
    }
}

void MainWindow::setupUI()
{
    // Create central widget
    m_centralWidget = new QWidget(this);
    setCentralWidget(m_centralWidget);
    
    // Main layout
    m_mainLayout = new QVBoxLayout(m_centralWidget);
    m_mainLayout->setSpacing(10);
    m_mainLayout->setContentsMargins(10, 10, 10, 10);
    
    // Main splitter
    m_mainSplitter = new QSplitter(Qt::Horizontal, this);
    m_mainLayout->addWidget(m_mainSplitter);
    
    // Left panel (input and results)
    QWidget* leftPanel = new QWidget();
    QVBoxLayout* leftLayout = new QVBoxLayout(leftPanel);
    
    // Input group
    m_inputGroup = new QGroupBox("Target Configuration", this);
    m_inputLayout = new QGridLayout(m_inputGroup);
    
    // Host input
    m_inputLayout->addWidget(new QLabel("Host/IP:"), 0, 0);
    m_hostLineEdit = new QLineEdit(this);
    m_hostLineEdit->setPlaceholder("Enter hostname or IP address (e.g., google.com, 8.8.8.8)");
    m_hostLineEdit->setText("google.com");
    m_inputLayout->addWidget(m_hostLineEdit, 0, 1, 1, 2);
    
    // Interval setting
    m_inputLayout->addWidget(new QLabel("Interval (ms):"), 1, 0);
    m_intervalSpinBox = new QSpinBox(this);
    m_intervalSpinBox->setRange(100, 10000);
    m_intervalSpinBox->setValue(1000);
    m_intervalSpinBox->setSuffix(" ms");
    m_inputLayout->addWidget(m_intervalSpinBox, 1, 1);
    
    // Timeout setting
    m_inputLayout->addWidget(new QLabel("Timeout (ms):"), 1, 2);
    m_timeoutSpinBox = new QSpinBox(this);
    m_timeoutSpinBox->setRange(500, 30000);
    m_timeoutSpinBox->setValue(5000);
    m_timeoutSpinBox->setSuffix(" ms");
    m_inputLayout->addWidget(m_timeoutSpinBox, 1, 3);
    
    // Control buttons
    m_startButton = new QPushButton("Start", this);
    m_stopButton = new QPushButton("Stop", this);
    m_resetButton = new QPushButton("Reset", this);
    
    m_startButton->setIcon(QIcon(":/icons/start.png"));
    m_stopButton->setIcon(QIcon(":/icons/stop.png"));
    m_resetButton->setIcon(QIcon(":/icons/reset.png"));
    
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(m_startButton);
    buttonLayout->addWidget(m_stopButton);
    buttonLayout->addWidget(m_resetButton);
    buttonLayout->addStretch();
    
    m_inputLayout->addLayout(buttonLayout, 2, 0, 1, 4);
    
    // Status label
    m_statusLabel = new QLabel("Ready to start tracing...", this);
    m_statusLabel->setStyleSheet("QLabel { color: #666; font-style: italic; }");
    m_inputLayout->addWidget(m_statusLabel, 3, 0, 1, 4);
    
    leftLayout->addWidget(m_inputGroup);
    
    // Results group
    m_resultsGroup = new QGroupBox("Traceroute Results", this);
    m_resultsLayout = new QVBoxLayout(m_resultsGroup);
    
    // Results table
    m_resultsTable = new QTableWidget(this);
    m_resultsTable->setColumnCount(7);
    QStringList headers = {"Hop", "Hostname", "IP Address", "Loss %", "Sent", "Best", "Avg", "Worst"};
    m_resultsTable->setHorizontalHeaderLabels(headers);
    
    // Table properties
    m_resultsTable->setAlternatingRowColors(true);
    m_resultsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_resultsTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_resultsTable->setSortingEnabled(false);
    m_resultsTable->verticalHeader()->setVisible(false);
    
    // Header properties
    QHeaderView* header = m_resultsTable->horizontalHeader();
    header->setStretchLastSection(true);
    header->setSectionResizeMode(QHeaderView::Interactive);
    
    m_resultsLayout->addWidget(m_resultsTable);
    leftLayout->addWidget(m_resultsGroup);
    
    // Control group
    m_controlGroup = new QGroupBox("Options", this);
    m_controlLayout = new QHBoxLayout(m_controlGroup);
    
    m_exportButton = new QPushButton("Export", this);
    m_copyButton = new QPushButton("Copy to Clipboard", this);
    m_darkModeCheckBox = new QCheckBox("Dark Mode", this);
    
    m_exportButton->setIcon(QIcon(":/icons/export.png"));
    m_copyButton->setIcon(QIcon(":/icons/copy.png"));
    
    m_controlLayout->addWidget(m_exportButton);
    m_controlLayout->addWidget(m_copyButton);
    m_controlLayout->addStretch();
    m_controlLayout->addWidget(m_darkModeCheckBox);
    
    leftLayout->addWidget(m_controlGroup);
    
    // Right panel (statistics)
    m_statsGroup = new QGroupBox("Statistics & Log", this);
    m_statsLayout = new QVBoxLayout(m_statsGroup);
    
    m_statsTextEdit = new QTextEdit(this);
    m_statsTextEdit->setReadOnly(true);
    m_statsTextEdit->setFont(QFont("Courier New", 9));
    m_statsLayout->addWidget(m_statsTextEdit);
    
    // Add panels to splitter
    m_mainSplitter->addWidget(leftPanel);
    m_mainSplitter->addWidget(m_statsGroup);
    m_mainSplitter->setSizes({800, 400});
}

void MainWindow::setupMenus()
{
    m_menuBar = menuBar();
    
    // File menu
    m_fileMenu = m_menuBar->addMenu("&File");
    
    m_startAction = new QAction(QIcon(":/icons/start.png"), "&Start", this);
    m_startAction->setShortcut(QKeySequence::New);
    m_startAction->setStatusTip("Start network tracing");
    
    m_stopAction = new QAction(QIcon(":/icons/stop.png"), "S&top", this);
    m_stopAction->setShortcut(QKeySequence("Ctrl+T"));
    m_stopAction->setStatusTip("Stop network tracing");
    
    m_resetAction = new QAction(QIcon(":/icons/reset.png"), "&Reset", this);
    m_resetAction->setShortcut(QKeySequence("Ctrl+R"));
    m_resetAction->setStatusTip("Reset all results");
    
    m_exportAction = new QAction(QIcon(":/icons/export.png"), "&Export...", this);
    m_exportAction->setShortcut(QKeySequence::SaveAs);
    m_exportAction->setStatusTip("Export results to file");
    
    m_exitAction = new QAction("E&xit", this);
    m_exitAction->setShortcut(QKeySequence::Quit);
    m_exitAction->setStatusTip("Exit PingTracer");
    
    m_fileMenu->addAction(m_startAction);
    m_fileMenu->addAction(m_stopAction);
    m_fileMenu->addAction(m_resetAction);
    m_fileMenu->addSeparator();
    m_fileMenu->addAction(m_exportAction);
    m_fileMenu->addSeparator();
    m_fileMenu->addAction(m_exitAction);
    
    // View menu
    m_viewMenu = m_menuBar->addMenu("&View");
    
    m_darkModeAction = new QAction("&Dark Mode", this);
    m_darkModeAction->setCheckable(true);
    m_darkModeAction->setStatusTip("Toggle dark mode");
    
    m_viewMenu->addAction(m_darkModeAction);
    
    // Help menu
    m_helpMenu = m_menuBar->addMenu("&Help");
    
    m_helpAction = new QAction("&Help", this);
    m_helpAction->setShortcut(QKeySequence::HelpContents);
    m_helpAction->setStatusTip("Show help information");
    
    m_aboutAction = new QAction("&About PingTracer", this);
    m_aboutAction->setStatusTip("About PingTracer");
    
    m_helpMenu->addAction(m_helpAction);
    m_helpMenu->addSeparator();
    m_helpMenu->addAction(m_aboutAction);
}

void MainWindow::setupToolbar()
{
    m_toolBar = addToolBar("Main");
    m_toolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    
    m_toolBar->addAction(m_startAction);
    m_toolBar->addAction(m_stopAction);
    m_toolBar->addAction(m_resetAction);
    m_toolBar->addSeparator();
    m_toolBar->addAction(m_exportAction);
    m_toolBar->addSeparator();
    m_toolBar->addAction(m_darkModeAction);
}

void MainWindow::setupStatusBar()
{
    m_statusBar = statusBar();
    
    m_statusInfo = new QLabel("Ready");
    m_progressBar = new QProgressBar();
    m_progressBar->setVisible(false);
    m_progressBar->setMaximumWidth(200);
    
    m_statusBar->addWidget(m_statusInfo);
    m_statusBar->addPermanentWidget(m_progressBar);
}

void MainWindow::setupConnections()
{
    // Button connections
    connect(m_startButton, &QPushButton::clicked, this, &MainWindow::startTracing);
    connect(m_stopButton, &QPushButton::clicked, this, &MainWindow::stopTracing);
    connect(m_resetButton, &QPushButton::clicked, this, &MainWindow::resetResults);
    connect(m_exportButton, &QPushButton::clicked, this, &MainWindow::exportResults);
    connect(m_copyButton, &QPushButton::clicked, this, &MainWindow::copyToClipboard);
    
    // Action connections
    connect(m_startAction, &QAction::triggered, this, &MainWindow::startTracing);
    connect(m_stopAction, &QAction::triggered, this, &MainWindow::stopTracing);
    connect(m_resetAction, &QAction::triggered, this, &MainWindow::resetResults);
    connect(m_exportAction, &QAction::triggered, this, &MainWindow::exportResults);
    connect(m_exitAction, &QAction::triggered, this, &QWidget::close);
    connect(m_darkModeAction, &QAction::triggered, this, &MainWindow::toggleDarkMode);
    connect(m_aboutAction, &QAction::triggered, this, &MainWindow::showAbout);
    connect(m_helpAction, &QAction::triggered, this, &MainWindow::showHelp);
    
    // Input connections
    connect(m_hostLineEdit, &QLineEdit::textChanged, this, &MainWindow::onHostChanged);
    connect(m_intervalSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), 
            this, &MainWindow::onIntervalChanged);
    connect(m_timeoutSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), 
            this, &MainWindow::onTimeoutChanged);
    
    // Theme connections
    connect(m_darkModeCheckBox, &QCheckBox::toggled, this, &MainWindow::toggleDarkMode);
    
    // Update timer
    connect(m_updateTimer, &QTimer::timeout, this, &MainWindow::updateStatusBar);
}

void MainWindow::startTracing()
{
    QString host = m_hostLineEdit->text().trimmed();
    if (host.isEmpty()) {
        QMessageBox::warning(this, "PingTracer", "Please enter a hostname or IP address.");
        m_hostLineEdit->setFocus();
        return;
    }
    
    if (m_pingTracer && m_pingTracer->isRunning()) {
        m_pingTracer->stop();
    }
    
    // Clear previous results
    m_resultsTable->setRowCount(0);
    m_statsTextEdit->clear();
    
    // Configure and start tracer
    m_pingTracer->setTarget(host);
    m_pingTracer->setInterval(m_intervalSpinBox->value());
    m_pingTracer->setTimeout(m_timeoutSpinBox->value());
    
    if (m_pingTracer->start()) {
        m_isRunning = true;
        m_currentHost = host;
        m_totalPacketsSent = 0;
        m_totalPacketsReceived = 0;
        
        m_statusLabel->setText(QString("Tracing route to %1...").arg(host));
        m_statusInfo->setText("Running");
        m_progressBar->setVisible(true);
        m_progressBar->setRange(0, 0); // Indeterminate progress
        
        m_updateTimer->start(500); // Update every 500ms
        
        // Log start
        QString startMsg = QString("[%1] Started tracing to %2")
                          .arg(QDateTime::currentDateTime().toString("hh:mm:ss"))
                          .arg(host);
        m_statsTextEdit->append(startMsg);
        
        updateButtonStates();
    } else {
        QMessageBox::critical(this, "PingTracer", 
                             "Failed to start network tracing. Please check your network permissions.");
    }
}

void MainWindow::stopTracing()
{
    if (m_pingTracer && m_pingTracer->isRunning()) {
        m_pingTracer->stop();
    }
    
    m_isRunning = false;
    m_statusLabel->setText("Tracing stopped.");
    m_statusInfo->setText("Stopped");
    m_progressBar->setVisible(false);
    m_updateTimer->stop();
    
    // Log stop
    QString stopMsg = QString("[%1] Tracing stopped")
                     .arg(QDateTime::currentDateTime().toString("hh:mm:ss"));
    m_statsTextEdit->append(stopMsg);
    
    updateButtonStates();
}

void MainWindow::resetResults()
{
    if (m_pingTracer && m_pingTracer->isRunning()) {
        m_pingTracer->stop();
    }
    
    m_resultsTable->setRowCount(0);
    m_statsTextEdit->clear();
    m_isRunning = false;
    m_totalPacketsSent = 0;
    m_totalPacketsReceived = 0;
    
    m_statusLabel->setText("Ready to start tracing...");
    m_statusInfo->setText("Ready");
    m_progressBar->setVisible(false);
    m_updateTimer->stop();
    
    updateButtonStates();
}

void MainWindow::exportResults()
{
    if (m_resultsTable->rowCount() == 0) {
        QMessageBox::information(this, "PingTracer", "No results to export.");
        return;
    }
    
    QString fileName = QFileDialog::getSaveFileName(
        this,
        "Export Results",
        QString("PingTracer_Results_%1.txt").arg(QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss")),
        "Text Files (*.txt);;CSV Files (*.csv);;All Files (*)"
    );
    
    if (!fileName.isEmpty()) {
        ExportManager::exportResults(m_resultsTable, m_currentHost, fileName);
        m_statusInfo->setText("Results exported successfully");
    }
}

void MainWindow::copyToClipboard()
{
    if (m_resultsTable->rowCount() == 0) {
        QMessageBox::information(this, "PingTracer", "No results to copy.");
        return;
    }
    
    QString clipboardText = ExportManager::formatResultsForClipboard(m_resultsTable, m_currentHost);
    QApplication::clipboard()->setText(clipboardText);
    
    m_statusInfo->setText("Results copied to clipboard");
}

void MainWindow::onHostChanged()
{
    // Enable/disable start button based on host input
    updateButtonStates();
}

void MainWindow::onTracerouteUpdate(const QList<HopData>& hops)
{
    // Update results table
    m_resultsTable->setRowCount(hops.size());
    
    for (int i = 0; i < hops.size(); ++i) {
        const HopData& hop = hops[i];
        
        // Hop number
        QTableWidgetItem* hopItem = new QTableWidgetItem(QString::number(hop.hopNumber));
        hopItem->setTextAlignment(Qt::AlignCenter);
        m_resultsTable->setItem(i, 0, hopItem);
        
        // Hostname
        QTableWidgetItem* hostnameItem = new QTableWidgetItem(hop.hostname);
        m_resultsTable->setItem(i, 1, hostnameItem);
        
        // IP Address
        QTableWidgetItem* ipItem = new QTableWidgetItem(hop.ipAddress);
        m_resultsTable->setItem(i, 2, ipItem);
        
        // Loss percentage
        double lossPercent = hop.sent > 0 ? ((double)(hop.sent - hop.received) / hop.sent) * 100.0 : 0.0;
        QTableWidgetItem* lossItem = new QTableWidgetItem(QString::number(lossPercent, 'f', 1));
        lossItem->setTextAlignment(Qt::AlignCenter);
        
        // Color code based on packet loss
        if (lossPercent > 50) {
            lossItem->setBackground(QColor(255, 200, 200)); // Light red
        } else if (lossPercent > 10) {
            lossItem->setBackground(QColor(255, 255, 200)); // Light yellow
        } else {
            lossItem->setBackground(QColor(200, 255, 200)); // Light green
        }
        
        m_resultsTable->setItem(i, 3, lossItem);
        
        // Sent packets
        QTableWidgetItem* sentItem = new QTableWidgetItem(QString::number(hop.sent));
        sentItem->setTextAlignment(Qt::AlignCenter);
        m_resultsTable->setItem(i, 4, sentItem);
        
        // Best time
        QTableWidgetItem* bestItem = new QTableWidgetItem(
            hop.bestTime >= 0 ? QString::number(hop.bestTime, 'f', 1) + " ms" : "---");
        bestItem->setTextAlignment(Qt::AlignCenter);
        m_resultsTable->setItem(i, 5, bestItem);
        
        // Average time
        QTableWidgetItem* avgItem = new QTableWidgetItem(
            hop.avgTime >= 0 ? QString::number(hop.avgTime, 'f', 1) + " ms" : "---");
        avgItem->setTextAlignment(Qt::AlignCenter);
        m_resultsTable->setItem(i, 6, avgItem);
        
        // Worst time
        QTableWidgetItem* worstItem = new QTableWidgetItem(
            hop.worstTime >= 0 ? QString::number(hop.worstTime, 'f', 1) + " ms" : "---");
        worstItem->setTextAlignment(Qt::AlignCenter);
        m_resultsTable->setItem(i, 7, worstItem);
        
        // Update statistics
        m_totalPacketsSent += hop.sent;
        m_totalPacketsReceived += hop.received;
    }
    
    resizeColumnsToContent();
    
    // Update statistics text
    QString statsText = QString(
        "=== PingTracer Statistics ===\n"
        "Target: %1\n"
        "Total Hops: %2\n"
        "Total Packets Sent: %3\n"
        "Total Packets Received: %4\n"
        "Overall Loss: %5%\n"
        "Last Update: %6\n\n"
    ).arg(m_currentHost)
     .arg(hops.size())
     .arg(m_totalPacketsSent)
     .arg(m_totalPacketsReceived)
     .arg(m_totalPacketsSent > 0 ? QString::number(((double)(m_totalPacketsSent - m_totalPacketsReceived) / m_totalPacketsSent) * 100.0, 'f', 2) : "0.00")
     .arg(QDateTime::currentDateTime().toString("hh:mm:ss"));
    
    // Add detailed hop information
    statsText += "=== Hop Details ===\n";
    for (const HopData& hop : hops) {
        statsText += QString("Hop %1: %2 (%3) - Loss: %4% - Avg: %5ms\n")
                    .arg(hop.hopNumber)
                    .arg(hop.hostname)
                    .arg(hop.ipAddress)
                    .arg(hop.sent > 0 ? QString::number(((double)(hop.sent - hop.received) / hop.sent) * 100.0, 'f', 1) : "0.0")
                    .arg(hop.avgTime >= 0 ? QString::number(hop.avgTime, 'f', 1) : "---");
    }
    
    m_statsTextEdit->setPlainText(statsText);
}

void MainWindow::onTracerouteError(const QString& error)
{
    m_statusLabel->setText(QString("Error: %1").arg(error));
    m_statusInfo->setText("Error");
    m_progressBar->setVisible(false);
    
    QString errorMsg = QString("[%1] ERROR: %2")
                      .arg(QDateTime::currentDateTime().toString("hh:mm:ss"))
                      .arg(error);
    m_statsTextEdit->append(errorMsg);
    
    QMessageBox::warning(this, "PingTracer", QString("Network error occurred:\n%1").arg(error));
    
    stopTracing();
}

void MainWindow::onThemeChanged()
{
    applyCurrentTheme();
}

void MainWindow::showAbout()
{
    QString aboutText = QString(
        "<h2>PingTracer v1.0.0</h2>"
        "<p><strong>Professional Network Diagnostic Tool</strong></p>"
        "<p>A modern, cross-platform alternative to WinMTR with enhanced features.</p>"
        "<p><strong>Developer:</strong> Harvey<br>"
        "<strong>Website:</strong> <a href='https://www.iqterabharvey.me'>www.iqterabharvey.me</a></p>"
        "<p><strong>Features:</strong></p>"
        "<ul>"
        "<li>Real-time network path tracing</li>"
        "<li>Per-hop latency statistics</li>"
        "<li>Packet loss monitoring</li>"
        "<li>Professional dark/light themes</li>"
        "<li>Export to multiple formats</li>"
        "<li>Cross-platform support (Windows/Linux)</li>"
        "</ul>"
        "<p><strong>Built with:</strong> Qt6 & C++17</p>"
        "<p>Copyright © 2024 Harvey Tech. All rights reserved.</p>"
    );
    
    QMessageBox::about(this, "About PingTracer", aboutText);
}

void MainWindow::showHelp()
{
    QString helpText = QString(
        "<h2>PingTracer Help</h2>"
        "<h3>Getting Started:</h3>"
        "<ol>"
        "<li>Enter a hostname or IP address in the <strong>Host/IP</strong> field</li>"
        "<li>Adjust the <strong>Interval</strong> and <strong>Timeout</strong> settings if needed</li>"
        "<li>Click <strong>Start</strong> to begin tracing</li>"
        "<li>View real-time results in the table below</li>"
        "<li>Use <strong>Export</strong> or <strong>Copy</strong> to save results</li>"
        "</ol>"
        "<h3>Understanding Results:</h3>"
        "<ul>"
        "<li><strong>Hop:</strong> Router number in the path to destination</li>"
        "<li><strong>Hostname:</strong> DNS name of the router (if available)</li>"
        "<li><strong>IP Address:</strong> Router's IP address</li>"
        "<li><strong>Loss %:</strong> Percentage of packets lost at this hop</li>"
        "<li><strong>Sent:</strong> Number of packets sent to this hop</li>"
        "<li><strong>Best/Avg/Worst:</strong> Latency statistics in milliseconds</li>"
        "</ul>"
        "<h3>Color Coding:</h3>"
        "<ul>"
        "<li><span style='background-color: #c8ffc8;'>Green:</span> Low packet loss (≤10%)</li>"
        "<li><span style='background-color: #ffffc8;'>Yellow:</span> Medium packet loss (10-50%)</li>"
        "<li><span style='background-color: #ffc8c8;'>Red:</span> High packet loss (>50%)</li>"
        "</ul>"
        "<h3>Tips:</h3>"
        "<ul>"
        "<li>Higher packet loss often indicates network congestion or problems</li>"
        "<li>Consistent high latency may indicate slow network segments</li>"
        "<li>Timeouts (shown as ---) indicate unreachable or filtered hops</li>"
        "</ul>"
    );
    
    QMessageBox::information(this, "PingTracer Help", helpText);
}

void MainWindow::toggleDarkMode()
{
    bool isDark = m_darkModeCheckBox->isChecked() || m_darkModeAction->isChecked();
    
    // Sync checkbox and action
    m_darkModeCheckBox->setChecked(isDark);
    m_darkModeAction->setChecked(isDark);
    
    // Apply theme
    ThemeManager::Theme theme = isDark ? ThemeManager::Theme::Dark : ThemeManager::Theme::Professional;
    ThemeManager::instance().applyTheme(qApp, theme);
}

void MainWindow::onIntervalChanged()
{
    if (m_pingTracer && m_pingTracer->isRunning()) {
        m_pingTracer->setInterval(m_intervalSpinBox->value());
    }
}

void MainWindow::onTimeoutChanged()
{
    if (m_pingTracer && m_pingTracer->isRunning()) {
        m_pingTracer->setTimeout(m_timeoutSpinBox->value());
    }
}

void MainWindow::updateButtonStates()
{
    bool hasHost = !m_hostLineEdit->text().trimmed().isEmpty();
    bool isRunning = m_isRunning;
    
    m_startButton->setEnabled(hasHost && !isRunning);
    m_stopButton->setEnabled(isRunning);
    m_resetButton->setEnabled(!isRunning);
    m_exportButton->setEnabled(m_resultsTable->rowCount() > 0);
    m_copyButton->setEnabled(m_resultsTable->rowCount() > 0);
    
    // Actions
    m_startAction->setEnabled(hasHost && !isRunning);
    m_stopAction->setEnabled(isRunning);
    m_resetAction->setEnabled(!isRunning);
    m_exportAction->setEnabled(m_resultsTable->rowCount() > 0);
    
    // Input fields
    m_hostLineEdit->setEnabled(!isRunning);
    m_intervalSpinBox->setEnabled(true); // Can be changed during operation
    m_timeoutSpinBox->setEnabled(true);  // Can be changed during operation
}

void MainWindow::updateStatusBar()
{
    if (m_isRunning && m_pingTracer) {
        QString status = QString("Running - Packets sent: %1, received: %2")
                        .arg(m_totalPacketsSent)
                        .arg(m_totalPacketsReceived);
        m_statusInfo->setText(status);
    }
}

void MainWindow::resizeColumnsToContent()
{
    for (int i = 0; i < m_resultsTable->columnCount(); ++i) {
        m_resultsTable->resizeColumnToContents(i);
    }
}

void MainWindow::applyCurrentTheme()
{
    // Additional custom styling can be applied here
    // The main theme is handled by ThemeManager
}
