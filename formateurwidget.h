#ifndef FORMATEURWIDGET_H
#define FORMATEURWIDGET_H

#include <QWidget>
#include <QTableWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QStackedWidget>

class FormateurWidget : public QWidget {
    Q_OBJECT
public:
    explicit FormateurWidget(QWidget *parent = nullptr);
    void rafraichirTable();
    void afficherListe();
    void afficherFormulaireAjout();
private slots:
    void ouvrirFormulaireAjout();
    void ouvrirFormulaireModification();
    void enregistrer();
    void supprimerFormateur();
    void onSelectionChanged();
    void retourListe();
private:
    void setupUi();
    QWidget* creerPageListe();
    QWidget* creerPageFormulaire();
    void remplirFormulaire(int id);
    void mettreAJourBoutonsListe();

    QStackedWidget* m_stack;
    QTableWidget*   m_tableFormateurs;
    QPushButton*    m_btnAjouter, *m_btnModifier, *m_btnSupprimer;
    QLabel*         m_lblCount, *m_lblFormTitre;
    QLineEdit*      m_editNom, *m_editPrenom, *m_editEmail, *m_editPassword;
    QComboBox*      m_comboRole, *m_comboStatut;
    QPushButton*    m_btnEnregistrer, *m_btnAnnuler;
    int  m_idFormateurSelectionne = -1;
    bool m_modeAjout = true;
};
#endif
