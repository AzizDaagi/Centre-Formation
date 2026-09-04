#ifndef SALLEWIDGET_H
#define SALLEWIDGET_H

#include <QWidget>
#include <QTableWidget>
#include <QLineEdit>
#include <QSpinBox>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QStackedWidget>

class SalleWidget : public QWidget {
    Q_OBJECT

public:
    explicit SalleWidget(QWidget *parent = nullptr);
    void rafraichirTable();
    void afficherListe();
    void afficherFormulaireAjout();

private slots:
    void ouvrirFormulaireAjout();
    void ouvrirFormulaireModification();
    void enregistrer();
    void supprimerSalle();
    void onSelectionChanged();
    void retourListe();

private:
    void setupUi();
    QWidget* creerPageListe();
    QWidget* creerPageFormulaire();
    void remplirFormulaire(int id);
    void mettreAJourBoutonsListe();

    QStackedWidget* m_stack;
    QTableWidget*  m_tableSalles;
    QPushButton*   m_btnAjouter;
    QPushButton*   m_btnModifier;
    QPushButton*   m_btnSupprimer;
    QLabel*        m_lblCount;
    class QChartView* m_chart = nullptr;

    QLabel*        m_lblFormTitre;
    QLineEdit*     m_editNomSalle;
    QSpinBox*      m_spinCapacite;
    QComboBox*     m_comboType;
    QComboBox*     m_comboStatut;
    QPushButton*   m_btnEnregistrer;
    QPushButton*   m_btnAnnuler;

    int  m_idSalleSelectionnee = -1;
    bool m_modeAjout = true;
};

#endif // SALLEWIDGET_H
