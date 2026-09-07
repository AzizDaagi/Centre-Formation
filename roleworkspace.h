#ifndef ROLEWORKSPACE_H
#define ROLEWORKSPACE_H

#include <QWidget>
#include <QString>

class QLabel;
class QTableWidget;
class QTabWidget;
class QComboBox;
class QDoubleSpinBox;
class QLineEdit;
class QTextEdit;
class QProgressBar;
class QPushButton;
class QDateEdit;
class QTimeEdit;

class RoleWorkspace : public QWidget {
    Q_OBJECT

public:
    enum class Mode { Formateur, Stagiaire };
    explicit RoleWorkspace(Mode mode, QWidget *parent = nullptr);
    void setUser(int userId, const QString &firstName, const QString &lastName,
                 const QString &email = QString());

signals:
    void openProfileRequested();

private slots:
    
    void onFormateurCoursChanged(int index);
    void validerSeanceEtPresences();
    void mettreAJourStatutStagiaire();
    void envoyerSignalementFormateur();
    void exporterFeuilleEmargementPdf();
    void analyserProgressionAvecIA();

    
    void soumettreJustificationStagiaire();
    void envoyerSignalementStagiaire();
    void exporterAttestationFormationPdf();
    void reserverSalleEtude();
    void annulerReservationEtude();

private:
    void setupUi();
    void refresh();
    QWidget* createMetricCard(const QString &label, QLabel **value, const QString &accent);

    
    QWidget* createFormateurWorkspace();
    QWidget* createStagiaireWorkspace();

    
    void refreshFormateur();
    void refreshStagiaire();
    void refreshFormateurTrainees();

    Mode m_mode;
    int m_userId = -1;
    QString m_userFirstName;
    QString m_userLastName;
    QString m_userEmail;

    
    QLabel *m_welcome = nullptr;
    QLabel *m_notificationBanner = nullptr;
    QLabel *m_primaryMetric = nullptr;
    QLabel *m_secondaryMetric = nullptr;
    QLabel *m_tertiaryMetric = nullptr;

    
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
    QTextEdit *m_aiProgressionResult = nullptr;

    
    QLabel *m_lblStagiaireCours = nullptr;
    QLabel *m_lblStagiaireFormateur = nullptr;
    QLabel *m_lblStagiaireSalle = nullptr;
    QLabel *m_lblStagiairePeriode = nullptr;
    QProgressBar *m_barProgression = nullptr;
    QLabel *m_lblProgressionDetail = nullptr;
    QPushButton *m_btnAttestationPdf = nullptr;

    QTextEdit *m_editJustification = nullptr;
    QComboBox *m_comboTypeIncidentStag = nullptr;
    QTextEdit *m_editDescIncidentStag = nullptr;

    
    QLabel *m_lblSalleHabituelle = nullptr;
    QLabel *m_lblSalleStatutChangement = nullptr;
    QTableWidget *m_tableSallesDispos = nullptr;
    QComboBox *m_comboReservationSalle = nullptr;
    QDateEdit *m_dateReservation = nullptr;
    QTimeEdit *m_heureDebutReservation = nullptr;
    QTimeEdit *m_heureFinReservation = nullptr;
    QTableWidget *m_tableMesReservations = nullptr;
    QPushButton *m_btnAnnulerReservation = nullptr;
};

#endif 
