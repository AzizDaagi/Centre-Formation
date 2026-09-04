#ifndef ROLEWORKSPACE_H
#define ROLEWORKSPACE_H

#include <QWidget>

class QLabel;
class QTableWidget;
class QTabWidget;
class QComboBox;
class QDoubleSpinBox;
class QLineEdit;
class QTextEdit;
class QProgressBar;

class RoleWorkspace : public QWidget {
    Q_OBJECT

public:
    enum class Mode { Formateur, Stagiaire };
    explicit RoleWorkspace(Mode mode, QWidget *parent = nullptr);
    void setUser(int userId, const QString &firstName, const QString &lastName);

private slots:
    // Formateur actions
    void onFormateurCoursChanged(int index);
    void validerSeanceEtPresences();
    void mettreAJourStatutStagiaire();
    void envoyerSignalementFormateur();

    // Stagiaire actions
    void soumettreJustificationStagiaire();
    void envoyerSignalementStagiaire();

private:
    void setupUi();
    void refresh();
    QWidget* createMetricCard(const QString &label, QLabel **value, const QString &accent);

    // Specific workspace creators
    QWidget* createFormateurWorkspace();
    QWidget* createStagiaireWorkspace();

    // Refreshers
    void refreshFormateur();
    void refreshStagiaire();

    Mode m_mode;
    int m_userId = -1;
    QString m_userFirstName;
    QString m_userLastName;

    // Common UI
    QLabel *m_welcome = nullptr;
    QLabel *m_primaryMetric = nullptr;
    QLabel *m_secondaryMetric = nullptr;
    QLabel *m_tertiaryMetric = nullptr;

    // Formateur UI components
    QComboBox *m_comboFormateurCours = nullptr;
    QLabel *m_lblSalleSession = nullptr;
    QDoubleSpinBox *m_spinHeuresSession = nullptr;
    QLineEdit *m_editSujetSession = nullptr;
    QTableWidget *m_tablePresences = nullptr;

    QTableWidget *m_tableStagiairesSuivi = nullptr;
    QComboBox *m_comboStatutUpdate = nullptr;

    QComboBox *m_comboSalleIncidentForm = nullptr;
    QComboBox *m_comboTypeIncidentForm = nullptr;
    QTextEdit *m_editDescIncidentForm = nullptr;

    // Stagiaire UI components
    QLabel *m_lblStagiaireCours = nullptr;
    QLabel *m_lblStagiaireFormateur = nullptr;
    QLabel *m_lblStagiaireSalle = nullptr;
    QLabel *m_lblStagiairePeriode = nullptr;
    QProgressBar *m_barProgression = nullptr;
    QLabel *m_lblProgressionDetail = nullptr;

    QTextEdit *m_editJustification = nullptr;
    QComboBox *m_comboTypeIncidentStag = nullptr;
    QTextEdit *m_editDescIncidentStag = nullptr;
};

#endif // ROLEWORKSPACE_H
