#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QTableWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QProgressBar>
#include <QComboBox>
#include <QSpinBox>
#include <QStatusBar>
#include <QMenuBar>
#include <QToolBar>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QSplitter>
#include <QTextEdit>
#include <QCheckBox>
#include "pingtracer.h"
#include "thememanager.h"

QT_BEGIN_NAMESPACE
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void startTracing();
    void stopTracing();
    void resetResults();
    void exportResults();
    void copyToClipboard();
    void onHostChanged();
    void onTracerouteUpdate(const QList<HopData>& hops);
    void onTracerouteError(const QString& error);
    void onThemeChanged();
    void showAbout();
    void showHelp();
    void toggleDarkMode();
    void onIntervalChanged();
    void onTimeoutChanged();

private:
    void setupUI();
    void setupMenus();
    void setupToolbar();
    void setupStatusBar();
    void setupConnections();
    void updateButtonStates();
    void updateStatusBar();
    void resizeColumnsToContent();
    void applyCurrentTheme();
    
    // Core components
    PingTracer* m_pingTracer;
    QTimer* m_updateTimer;
    
    // Central widget and layouts
    QWidget* m_centralWidget;
    QSplitter* m_mainSplitter;
    QVBoxLayout* m_mainLayout;
    
    // Input section
    QGroupBox* m_inputGroup;
    QGridLayout* m_inputLayout;
    QLineEdit* m_hostLineEdit;
    QPushButton* m_startButton;
    QPushButton* m_stopButton;
    QPushButton* m_resetButton;
    QSpinBox* m_intervalSpinBox;
    QSpinBox* m_timeoutSpinBox;
    QLabel* m_statusLabel;
    
    // Results table
    QGroupBox* m_resultsGroup;
    QVBoxLayout* m_resultsLayout;
    QTableWidget* m_resultsTable;
    
    // Statistics panel
    QGroupBox* m_statsGroup;
    QVBoxLayout* m_statsLayout;
    QTextEdit* m_statsTextEdit;
    
    // Control panel
    QGroupBox* m_controlGroup;
    QHBoxLayout* m_controlLayout;
    QPushButton* m_exportButton;
    QPushButton* m_copyButton;
    QCheckBox* m_darkModeCheckBox;
    QComboBox* m_themeComboBox;
    
    // Menu and toolbar
    QMenuBar* m_menuBar;
    QMenu* m_fileMenu;
    QMenu* m_viewMenu;
    QMenu* m_helpMenu;
    QToolBar* m_toolBar;
    QStatusBar* m_statusBar;
    QProgressBar* m_progressBar;
    QLabel* m_statusInfo;
    
    // Actions
    QAction* m_startAction;
    QAction* m_stopAction;
    QAction* m_resetAction;
    QAction* m_exportAction;
    QAction* m_exitAction;
    QAction* m_darkModeAction;
    QAction* m_aboutAction;
    QAction* m_helpAction;
    
    // State variables
    bool m_isRunning;
    QString m_currentHost;
    int m_totalPacketsSent;
    int m_totalPacketsReceived;
};

#endif // MAINWINDOW_H
