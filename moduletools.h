#ifndef MODULETOOLS_H
#define MODULETOOLS_H

#include <QStringList>
#include <QVector>
#include <QIcon>
#include <QStyle>

class QWidget;
class QTableWidget;
class QChartView;

class ModuleTools
{
public:
    static QWidget *createMultiCriteriaTools(QTableWidget *table,
                                             const QVector<int> &criteriaColumns,
                                             const QStringList &criteriaLabels);
    static void updateCategoryChart(QChartView *chartView, QTableWidget *table,
                                    int categoryColumn, const QString &title);
    static bool exportTableToPdf(QTableWidget *table, const QString &title,
                                 QString *outputPath = nullptr);
    static bool isValidEmail(const QString &email);
    static QIcon standardIcon(QStyle::StandardPixmap icon);
    static QWidget *createPaginationControls(QTableWidget *table, int pageSize = 8);
    static void refreshPagination(QTableWidget *table);
};

#endif // MODULETOOLS_H
