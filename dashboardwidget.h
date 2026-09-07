#ifndef DASHBOARDWIDGET_H
#define DASHBOARDWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QProgressBar>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFrame>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QStyle>

class DashboardWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DashboardWidget(QWidget *parent = nullptr);
    ~DashboardWidget();

    
    void refreshDashboard();

signals:
    
    void navigateToSignalsRequested();
    
    void addRoomRequested();
    void addCourseRequested();
    void viewPlanningRequested();

private:
    
    QLabel *m_lblTotalSalles;
    QLabel *m_lblTotalFormateurs;
    QLabel *m_lblTotalCours;
    QLabel *m_lblTotalStagiaires;

    
    QVBoxLayout *m_scheduleListLayout;
    QLabel *m_lblSignalsCount;
    QLabel *m_lblSignalsPreviewText;
    QLabel *m_lblSignalsPreviewMeta;

    
    QProgressBar *m_occupancyBar;
    QLabel *m_lblOccupancyPercent;

    
    void setupUi();
    QFrame* createKpiCard(const QString &title, QStyle::StandardPixmap icon,
                          QLabel **valueLabel, const QString &accentColor);

    
    void loadKpiMetrics();
    void loadUpcomingCourses();
    void loadSignalsSummary();
    void loadOccupancyData();
};

#endif 