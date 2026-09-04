#ifndef COURSWIDGET_H
#define COURSWIDGET_H

#include <QWidget>
#include <QTableWidget>
#include <QLineEdit>
#include <QSpinBox>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QStackedWidget>

class CoursWidget : public QWidget {
    Q_OBJECT
public:
    explicit CoursWidget(QWidget *parent = nullptr);
    void rafraichirTable();
    void afficherListe();
    void afficherFormulaireAjout();
private slots:
    void ouvrirFormulaireAjout();
    void ouvrirFormulaireModification();
    void enregistrer();
    void supprimerCours();
    void onSelectionChanged();
    void retourListe();
private:
    void setupUi();
    QWidget* creerPageListe();
    QWidget* creerPageFormulaire();
    void remplirFormulaire(int id);
    void rafraichirComboFormateurs();
    void mettreAJourBoutonsListe();

    QStackedWidget* m_stack;
    QTableWidget*   m_tableCours;
    QPushButton*    m_btnAjouter, *m_btnModifier, *m_btnSupprimer;
    QLabel*         m_lblCount, *m_lblFormTitre;
    class QChartView* m_chart = nullptr;
    QLineEdit*      m_editTitre, *m_editDescription;
    QSpinBox*       m_spinHeures;
    QComboBox*      m_comboFormateur;
    QPushButton*    m_btnEnregistrer, *m_btnAnnuler;
    int  m_idCoursSelectionne = -1;
    bool m_modeAjout = true;
};
#endif
