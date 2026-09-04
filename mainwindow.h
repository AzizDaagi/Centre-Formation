#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override = default;

private slots:
    void tenterConnexion();
    void deconnecter();

private:
    void setupUi();

    // Page factory methods
    QWidget* creerPageConnexion();
    QWidget* creerDashboardAdmin();
    QWidget* creerDashboardFormateur();
    QWidget* creerDashboardStagiaire();

    // Context UI Refresher
    void mettreAJourContextesUtilisateur();

    // UI Navigation Container
    QStackedWidget* m_stackPages;

    // Login Form Widgets
    QLineEdit* m_editLoginEmail;
    QLineEdit* m_editLoginPassword;
    QLabel* m_lblErreurAuth;

    // Active User Context
    int m_userIdConnecte = -1;
    QString m_userRoleConnecte;
    QString m_userEmailConnecte;
    QString m_userPrenomConnecte;
    QString m_userNomConnecte;

    // Dynamic Header Labels for Dashboards
    QLabel* m_lblWelcomeAdmin;
    QLabel* m_lblBadgeAdmin;
    QLabel* m_lblWelcomeFormateur;
    QLabel* m_lblWelcomeStagiaire;
};

#endif // MAINWINDOW_H