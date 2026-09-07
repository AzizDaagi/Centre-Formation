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

    // Call this method whenever switching to the dashboard tab to refresh numbers
    void refreshDashboard();

signals:
    // Signal emitted when user clicks "Voir Tout" on the Signalements card
    void navigateToSignalsRequested();
    // Signals emitted for quick shortcut buttons
    void addRoomRequested();
    void addCourseRequested();
    void viewPlanningRequested();

private:
    // UI Elements - Metric Cards
    QLabel *m_lblTotalSalles;
    QLabel *m_lblTotalFormateurs;
    QLabel *m_lblTotalCours;
    QLabel *m_lblTotalStagiaires;

    // UI Elements - Upcoming Schedule & Signals Preview
    QVBoxLayout *m_scheduleListLayout;
    QLabel *m_lblSignalsCount;
    QLabel *m_lblSignalsPreviewText;
    QLabel *m_lblSignalsPreviewMeta;

    // UI Elements - Room Utilization
    QProgressBar *m_occupancyBar;
    QLabel *m_lblOccupancyPercent;

    // Setup Helpers
    void setupUi();
    QFrame* createKpiCard(const QString &title, QStyle::StandardPixmap icon,
                          QLabel **valueLabel, const QString &accentColor);

    // Database Fetching Helpers
    void loadKpiMetrics();
    void loadUpcomingCourses();
    void loadSignalsSummary();
    void loadOccupancyData();
};

#endif // DASHBOARDWIDGET_H