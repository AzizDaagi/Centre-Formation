#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>

class RoleWorkspace;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override = default;

private slots:
    void tenterConnexion();
    void deconnecter();
    void ouvrirProfil();

private:
    void setupUi();

    
    QWidget* creerPageConnexion();
    QWidget* creerDashboardAdmin(bool superAdmin = false);
    QWidget* creerDashboardFormateur();
    QWidget* creerDashboardStagiaire();

    
    void mettreAJourContextesUtilisateur();

    
    QStackedWidget* m_stackPages;

    
    QLineEdit* m_editLoginEmail;
    QLineEdit* m_editLoginPassword;
    QLabel* m_lblErreurAuth;

    
    int m_userIdConnecte = -1;
    QString m_userRoleConnecte;
    QString m_userEmailConnecte;
    QString m_userPrenomConnecte;
    QString m_userNomConnecte;

    
    QLabel* m_lblWelcomeAdmin;
    QLabel* m_lblBadgeAdmin;
    QLabel* m_lblWelcomeFormateur = nullptr;
    QLabel* m_lblWelcomeStagiaire = nullptr;
    RoleWorkspace* m_formateurWorkspace = nullptr;
    RoleWorkspace* m_stagiaireWorkspace = nullptr;
};

#endif 
