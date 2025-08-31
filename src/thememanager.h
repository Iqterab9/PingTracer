#ifndef THEMEMANAGER_H
#define THEMEMANAGER_H

#include <QObject>
#include <QApplication>
#include <QString>
#include <QMap>

class ThemeManager : public QObject
{
    Q_OBJECT

public:
    enum class Theme {
        Professional,
        Dark,
        Light
    };

    static ThemeManager& instance();
    
    void initialize();
    void applyTheme(QApplication* app, Theme theme);
    QString getThemeStyleSheet(Theme theme);
    
    Theme getCurrentTheme() const;
    void setCurrentTheme(Theme theme);

signals:
    void themeChanged(Theme theme);

private:
    ThemeManager() = default;
    
    QString getProfessionalTheme();
    QString getDarkTheme();
    QString getLightTheme();
    
    Theme m_currentTheme = Theme::Professional;
    QMap<Theme, QString> m_styleSheets;
};

#endif // THEMEMANAGER_H
