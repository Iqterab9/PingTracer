#ifndef EXPORTMANAGER_H
#define EXPORTMANAGER_H

#include <QString>
#include <QTableWidget>

class ExportManager
{
public:
    static bool exportResults(QTableWidget* table, const QString& targetHost, const QString& fileName);
    static QString formatResultsForClipboard(QTableWidget* table, const QString& targetHost);
    
private:
    static QString formatAsText(QTableWidget* table, const QString& targetHost);
    static QString formatAsCSV(QTableWidget* table, const QString& targetHost);
    static QString getCurrentTimestamp();
};

#endif // EXPORTMANAGER_H
