#ifndef ROLEWORKSPACE_H
#define ROLEWORKSPACE_H

#include <QWidget>
class QLabel;
class QTableWidget;
class RoleWorkspace : public QWidget {
    Q_OBJECT
public:
    enum class Mode { Formateur, Stagiaire };
    explicit RoleWorkspace(Mode mode, QWidget *parent = nullptr);
    void setUser(int userId, const QString &firstName, const QString &lastName);
private:
    void setupUi(); void refresh(); QWidget *createMetricCard(const QString &label, QLabel **value, const QString &accent);
    Mode m_mode; int m_userId = -1; QLabel *m_welcome = nullptr, *m_primaryMetric = nullptr, *m_secondaryMetric = nullptr, *m_tertiaryMetric = nullptr; QTableWidget *m_table = nullptr;
};
#endif
