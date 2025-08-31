#include "thememanager.h"
#include <QFile>
#include <QTextStream>

ThemeManager& ThemeManager::instance()
{
    static ThemeManager instance;
    return instance;
}

void ThemeManager::initialize()
{
    // Load all theme stylesheets
    m_styleSheets[Theme::Professional] = getProfessionalTheme();
    m_styleSheets[Theme::Dark] = getDarkTheme();
    m_styleSheets[Theme::Light] = getLightTheme();
}

void ThemeManager::applyTheme(QApplication* app, Theme theme)
{
    if (app && m_styleSheets.contains(theme)) {
        app->setStyleSheet(m_styleSheets[theme]);
        m_currentTheme = theme;
        emit themeChanged(theme);
    }
}

QString ThemeManager::getThemeStyleSheet(Theme theme)
{
    return m_styleSheets.value(theme, "");
}

ThemeManager::Theme ThemeManager::getCurrentTheme() const
{
    return m_currentTheme;
}

void ThemeManager::setCurrentTheme(Theme theme)
{
    m_currentTheme = theme;
}

QString ThemeManager::getProfessionalTheme()
{
    return QString(R"(
        /* Professional Theme - PingTracer by Harvey */
        QMainWindow {
            background-color: #f5f5f5;
            color: #333333;
        }
        
        QGroupBox {
            font-weight: bold;
            font-size: 12px;
            border: 2px solid #cccccc;
            border-radius: 8px;
            margin-top: 1ex;
            padding-top: 10px;
            background-color: #ffffff;
        }
        
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 10px;
            padding: 0 5px 0 5px;
            color: #2c5282;
        }
        
        QPushButton {
            background-color: #4299e1;
            border: none;
            color: white;
            padding: 8px 16px;
            border-radius: 6px;
            font-weight: bold;
            min-width: 80px;
        }
        
        QPushButton:hover {
            background-color: #3182ce;
        }
        
        QPushButton:pressed {
            background-color: #2c5282;
        }
        
        QPushButton:disabled {
            background-color: #a0aec0;
            color: #718096;
        }
        
        QLineEdit {
            border: 2px solid #e2e8f0;
            border-radius: 6px;
            padding: 8px;
            background-color: #ffffff;
            font-size: 11px;
        }
        
        QLineEdit:focus {
            border-color: #4299e1;
        }
        
        QSpinBox {
            border: 2px solid #e2e8f0;
            border-radius: 6px;
            padding: 6px;
            background-color: #ffffff;
            min-width: 80px;
        }
        
        QSpinBox:focus {
            border-color: #4299e1;
        }
        
        QTableWidget {
            alternate-background-color: #f7fafc;
            background-color: #ffffff;
            border: 1px solid #e2e8f0;
            gridline-color: #e2e8f0;
            selection-background-color: #bee3f8;
            border-radius: 6px;
        }
        
        QTableWidget::item {
            padding: 6px;
            border-bottom: 1px solid #e2e8f0;
        }
        
        QTableWidget::item:selected {
            background-color: #bee3f8;
            color: #1a202c;
        }
        
        QHeaderView::section {
            background-color: #4299e1;
            color: white;
            padding: 8px;
            border: none;
            font-weight: bold;
        }
        
        QTextEdit {
            border: 1px solid #e2e8f0;
            border-radius: 6px;
            background-color: #ffffff;
            padding: 8px;
            font-family: 'Courier New', monospace;
        }
        
        QCheckBox {
            spacing: 8px;
            color: #2d3748;
        }
        
        QCheckBox::indicator {
            width: 18px;
            height: 18px;
            border-radius: 4px;
            border: 2px solid #cbd5e0;
        }
        
        QCheckBox::indicator:checked {
            background-color: #4299e1;
            border-color: #4299e1;
        }
        
        QMenuBar {
            background-color: #ffffff;
            border-bottom: 1px solid #e2e8f0;
        }
        
        QMenuBar::item {
            padding: 6px 12px;
            margin: 2px;
            border-radius: 4px;
        }
        
        QMenuBar::item:selected {
            background-color: #edf2f7;
        }
        
        QMenu {
            background-color: #ffffff;
            border: 1px solid #e2e8f0;
            border-radius: 6px;
            padding: 4px;
        }
        
        QMenu::item {
            padding: 6px 20px;
            margin: 1px;
            border-radius: 4px;
        }
        
        QMenu::item:selected {
            background-color: #edf2f7;
        }
        
        QToolBar {
            background-color: #ffffff;
            border: 1px solid #e2e8f0;
            padding: 4px;
            spacing: 2px;
        }
        
        QStatusBar {
            background-color: #f7fafc;
            border-top: 1px solid #e2e8f0;
            color: #4a5568;
        }
        
        QProgressBar {
            border: 1px solid #e2e8f0;
            border-radius: 4px;
            text-align: center;
            background-color: #f7fafc;
        }
        
        QProgressBar::chunk {
            background-color: #4299e1;
            border-radius: 3px;
        }
        
        QLabel {
            color: #2d3748;
        }
        
        QSplitter::handle {
            background-color: #e2e8f0;
            width: 2px;
        }
        
        QSplitter::handle:horizontal {
            width: 4px;
        }
        
        QSplitter::handle:vertical {
            height: 4px;
        }
    )");
}

QString ThemeManager::getDarkTheme()
{
    return QString(R"(
        /* Dark Theme - PingTracer by Harvey */
        QMainWindow {
            background-color: #1a202c;
            color: #e2e8f0;
        }
        
        QGroupBox {
            font-weight: bold;
            font-size: 12px;
            border: 2px solid #4a5568;
            border-radius: 8px;
            margin-top: 1ex;
            padding-top: 10px;
            background-color: #2d3748;
        }
        
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 10px;
            padding: 0 5px 0 5px;
            color: #63b3ed;
        }
        
        QPushButton {
            background-color: #3182ce;
            border: none;
            color: white;
            padding: 8px 16px;
            border-radius: 6px;
            font-weight: bold;
            min-width: 80px;
        }
        
        QPushButton:hover {
            background-color: #2c5282;
        }
        
        QPushButton:pressed {
            background-color: #2a4365;
        }
        
        QPushButton:disabled {
            background-color: #4a5568;
            color: #718096;
        }
        
        QLineEdit {
            border: 2px solid #4a5568;
            border-radius: 6px;
            padding: 8px;
            background-color: #2d3748;
            color: #e2e8f0;
            font-size: 11px;
        }
        
        QLineEdit:focus {
            border-color: #63b3ed;
        }
        
        QSpinBox {
            border: 2px solid #4a5568;
            border-radius: 6px;
            padding: 6px;
            background-color: #2d3748;
            color: #e2e8f0;
            min-width: 80px;
        }
        
        QSpinBox:focus {
            border-color: #63b3ed;
        }
        
        QTableWidget {
            alternate-background-color: #2d3748;
            background-color: #1a202c;
            border: 1px solid #4a5568;
            gridline-color: #4a5568;
            selection-background-color: #3182ce;
            color: #e2e8f0;
            border-radius: 6px;
        }
        
        QTableWidget::item {
            padding: 6px;
            border-bottom: 1px solid #4a5568;
        }
        
        QTableWidget::item:selected {
            background-color: #3182ce;
            color: #ffffff;
        }
        
        QHeaderView::section {
            background-color: #2c5282;
            color: white;
            padding: 8px;
            border: none;
            font-weight: bold;
        }
        
        QTextEdit {
            border: 1px solid #4a5568;
            border-radius: 6px;
            background-color: #2d3748;
            color: #e2e8f0;
            padding: 8px;
            font-family: 'Courier New', monospace;
        }
        
        QCheckBox {
            spacing: 8px;
            color: #e2e8f0;
        }
        
        QCheckBox::indicator {
            width: 18px;
            height: 18px;
            border-radius: 4px;
            border: 2px solid #4a5568;
            background-color: #2d3748;
        }
        
        QCheckBox::indicator:checked {
            background-color: #3182ce;
            border-color: #3182ce;
        }
        
        QMenuBar {
            background-color: #2d3748;
            color: #e2e8f0;
            border-bottom: 1px solid #4a5568;
        }
        
        QMenuBar::item {
            padding: 6px 12px;
            margin: 2px;
            border-radius: 4px;
        }
        
        QMenuBar::item:selected {
            background-color: #4a5568;
        }
        
        QMenu {
            background-color: #2d3748;
            color: #e2e8f0;
            border: 1px solid #4a5568;
            border-radius: 6px;
            padding: 4px;
        }
        
        QMenu::item {
            padding: 6px 20px;
            margin: 1px;
            border-radius: 4px;
        }
        
        QMenu::item:selected {
            background-color: #4a5568;
        }
        
        QToolBar {
            background-color: #2d3748;
            border: 1px solid #4a5568;
            padding: 4px;
            spacing: 2px;
        }
        
        QStatusBar {
            background-color: #2d3748;
            border-top: 1px solid #4a5568;
            color: #a0aec0;
        }
        
        QProgressBar {
            border: 1px solid #4a5568;
            border-radius: 4px;
            text-align: center;
            background-color: #2d3748;
            color: #e2e8f0;
        }
        
        QProgressBar::chunk {
            background-color: #3182ce;
            border-radius: 3px;
        }
        
        QLabel {
            color: #e2e8f0;
        }
        
        QSplitter::handle {
            background-color: #4a5568;
            width: 2px;
        }
        
        QSplitter::handle:horizontal {
            width: 4px;
        }
        
        QSplitter::handle:vertical {
            height: 4px;
        }
    )");
}

QString ThemeManager::getLightTheme()
{
    return QString(R"(
        /* Light Theme - PingTracer by Harvey */
        QMainWindow {
            background-color: #ffffff;
            color: #000000;
        }
        
        QGroupBox {
            font-weight: bold;
            font-size: 12px;
            border: 1px solid #d0d0d0;
            border-radius: 6px;
            margin-top: 1ex;
            padding-top: 8px;
            background-color: #fafafa;
        }
        
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 8px;
            padding: 0 4px 0 4px;
            color: #0066cc;
        }
        
        QPushButton {
            background-color: #0066cc;
            border: 1px solid #0052a3;
            color: white;
            padding: 6px 12px;
            border-radius: 4px;
            font-weight: normal;
            min-width: 70px;
        }
        
        QPushButton:hover {
            background-color: #0052a3;
        }
        
        QPushButton:pressed {
            background-color: #004080;
        }
        
        QPushButton:disabled {
            background-color: #cccccc;
            border-color: #cccccc;
            color: #888888;
        }
        
        QLineEdit {
            border: 1px solid #cccccc;
            border-radius: 4px;
            padding: 6px;
            background-color: #ffffff;
            font-size: 11px;
        }
        
        QLineEdit:focus {
            border-color: #0066cc;
        }
        
        QSpinBox {
            border: 1px solid #cccccc;
            border-radius: 4px;
            padding: 4px;
            background-color: #ffffff;
            min-width: 70px;
        }
        
        QSpinBox:focus {
            border-color: #0066cc;
        }
        
        QTableWidget {
            alternate-background-color: #f8f8f8;
            background-color: #ffffff;
            border: 1px solid #cccccc;
            gridline-color: #dddddd;
            selection-background-color: #cce7ff;
        }
        
        QTableWidget::item {
            padding: 4px;
            border-bottom: 1px solid #eeeeee;
        }
        
        QTableWidget::item:selected {
            background-color: #cce7ff;
            color: #000000;
        }
        
        QHeaderView::section {
            background-color: #f0f0f0;
            color: #333333;
            padding: 6px;
            border: 1px solid #cccccc;
            font-weight: bold;
        }
        
        QTextEdit {
            border: 1px solid #cccccc;
            border-radius: 4px;
            background-color: #ffffff;
            padding: 6px;
            font-family: 'Courier New', monospace;
        }
        
        QCheckBox {
            spacing: 6px;
            color: #000000;
        }
        
        QCheckBox::indicator {
            width: 16px;
            height: 16px;
            border-radius: 3px;
            border: 1px solid #cccccc;
            background-color: #ffffff;
        }
        
        QCheckBox::indicator:checked {
            background-color: #0066cc;
            border-color: #0066cc;
        }
        
        QMenuBar {
            background-color: #f8f8f8;
            border-bottom: 1px solid #cccccc;
        }
        
        QMenuBar::item {
            padding: 4px 8px;
            margin: 1px;
            border-radius: 3px;
        }
        
        QMenuBar::item:selected {
            background-color: #e0e0e0;
        }
        
        QMenu {
            background-color: #ffffff;
            border: 1px solid #cccccc;
            border-radius: 4px;
            padding: 2px;
        }
        
        QMenu::item {
            padding: 4px 16px;
            margin: 1px;
            border-radius: 3px;
        }
        
        QMenu::item:selected {
            background-color: #e0e0e0;
        }
        
        QToolBar {
            background-color: #f8f8f8;
            border: 1px solid #cccccc;
            padding: 2px;
            spacing: 1px;
        }
        
        QStatusBar {
            background-color: #f0f0f0;
            border-top: 1px solid #cccccc;
            color: #666666;
        }
        
        QProgressBar {
            border: 1px solid #cccccc;
            border-radius: 3px;
