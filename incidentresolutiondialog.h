#ifndef INCIDENTRESOLUTIONDIALOG_H
#define INCIDENTRESOLUTIONDIALOG_H

#include <QDialog>

class QTableWidget;
class QComboBox;
class QPushButton;
class QTextEdit;

class IncidentResolutionDialog : public QDialog {
    Q_OBJECT
public:
    explicit IncidentResolutionDialog(QWidget *parent = nullptr);

private slots:
    void rafraichirIncidents();
    void resoudreSelection();
    void marquerEnCoursSelection();
    void supprimerSignalementSelection();

private:
    void setupUi();
    void executerMiseAJour(const QString &nouveauStatut, bool effacerComplet);

    QTableWidget *m_table;
    QPushButton *m_btnResolu;
    QPushButton *m_btnEnCours;
    QPushButton *m_btnSupprimer;
    QComboBox *m_filterType;
    QTextEdit *m_detailDescription;
};

#endif // INCIDENTRESOLUTIONDIALOG_H
