#ifndef MODULETOOLS_H
#define MODULETOOLS_H

#include <QStringList>
#include <QVector>

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
};

#endif // MODULETOOLS_H
