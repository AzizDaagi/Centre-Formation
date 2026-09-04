#ifndef STAGIAIREWIDGET_H
#define STAGIAIREWIDGET_H

#include <QWidget>
#include <QTableWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QDateEdit>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QLabel>
#include <QStackedWidget>

class StagiaireWidget : public QWidget {
    Q_OBJECT
public:
    explicit StagiaireWidget(QWidget *parent = nullptr);
    void rafraichirTable();
    void afficherListe();
    void afficherFormulaireAjout();
private slots:
    void ouvrirFormulaireAjout();
    void ouvrirFormulaireModification();
    void enregistrer();
    void supprimerStagiaire();
    void onSelectionChanged();
    void retourListe();
private:
    void setupUi();
    QWidget* creerPageListe();
    QWidget* creerPageFormulaire();
    void remplirFormulaire(int id);
    void rafraichirCombos();
    void mettreAJourBoutonsListe();

    QStackedWidget*  m_stack;
    QTableWidget*    m_tableStagiaires;
    QPushButton*     m_btnAjouter, *m_btnModifier, *m_btnSupprimer;
    QLabel*          m_lblCount, *m_lblFormTitre;
    QLineEdit*       m_editNom, *m_editPrenom, *m_editEmail, *m_editPassword;
    QComboBox*       m_comboFormateur, *m_comboCours, *m_comboSalle, *m_comboStatut;
    QDateEdit*       m_dateDebut, *m_dateFin;
    QDoubleSpinBox*  m_spinHeures;
    QPushButton*     m_btnEnregistrer, *m_btnAnnuler;
    int  m_idStagiaireSelectionne = -1;
    bool m_modeAjout = true;
};
#endif
