#include "mainwindow.h"
#include "dashboardwidget.h"
#include "sallewidget.h"
#include "formateurwidget.h"
#include "courswidget.h"
#include "stagiairewidget.h"
#include "authentification.h"
#include "db.h"
#include "roleworkspace.h"
#include "incidentresolutiondialog.h"
#include "userprofiledialog.h"
#include "moduletools.h"


#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGridLayout>
#include <QStackedWidget>
#include <QButtonGroup>
#include <QFrame>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QProgressBar>
#include <QMessageBox>
#include <QHeaderView>
#include <QTableWidget>
#include <QGraphicsDropShadowEffect>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUi();
    resize(1200, 800);
    setWindowTitle("Centre de Formation - Interface de Gestion");
}

void MainWindow::setupUi() {
    m_stackPages = new QStackedWidget(this);

    
    m_stackPages->addWidget(creerPageConnexion());

    
    m_stackPages->addWidget(creerDashboardAdmin(true));
    m_stackPages->addWidget(creerDashboardAdmin(false));

    
    m_stackPages->addWidget(creerDashboardFormateur());

    
    m_stackPages->addWidget(creerDashboardStagiaire());

    setCentralWidget(m_stackPages);
    m_stackPages->setCurrentIndex(0);
}

QWidget* MainWindow::creerPageConnexion() {
    QWidget* page = new QWidget();
    QVBoxLayout* mainLayout = new QVBoxLayout(page);
    mainLayout->setAlignment(Qt::AlignCenter);

    QFrame* loginCard = new QFrame();
    loginCard->setObjectName("glassCard");
    loginCard->setFixedWidth(380);

    QVBoxLayout* cardLayout = new QVBoxLayout(loginCard);
    cardLayout->setContentsMargins(30, 30, 30, 30);

    QLabel* lblTitre = new QLabel("<h2>Connexion</h2>");
    lblTitre->setAlignment(Qt::AlignCenter);

    QFormLayout* formLayout = new QFormLayout();
    m_editLoginEmail = new QLineEdit();
    m_editLoginEmail->setPlaceholderText("exemple@centre.tn");

    m_editLoginPassword = new QLineEdit();
    m_editLoginPassword->setEchoMode(QLineEdit::Password);
    m_editLoginPassword->setPlaceholderText("Mot de passe");

    formLayout->addRow("Email:", m_editLoginEmail);
    formLayout->addRow("Mot de passe:", m_editLoginPassword);

    m_lblErreurAuth = new QLabel("");
    m_lblErreurAuth->setStyleSheet("color: #e11d48; font-weight: bold;");
    m_lblErreurAuth->setAlignment(Qt::AlignCenter);

    QPushButton* btnConnexion = new QPushButton("Se Connecter");
    btnConnexion->setObjectName("primaryBtn");
    btnConnexion->setDefault(true);

    connect(btnConnexion, &QPushButton::clicked, this, &MainWindow::tenterConnexion);
    connect(m_editLoginEmail, &QLineEdit::returnPressed, this, &MainWindow::tenterConnexion);
    connect(m_editLoginPassword, &QLineEdit::returnPressed, this, &MainWindow::tenterConnexion);

    cardLayout->addWidget(lblTitre);
    cardLayout->addLayout(formLayout);
    cardLayout->addWidget(m_lblErreurAuth);
    cardLayout->addSpacing(10);
    cardLayout->addWidget(btnConnexion);

    mainLayout->addWidget(loginCard);
    return page;
}

void MainWindow::tenterConnexion() {
    m_lblErreurAuth->clear();

    QString email = m_editLoginEmail->text().trimmed();
    QString password = m_editLoginPassword->text();

    if (email.isEmpty() || password.isEmpty()) {
        m_lblErreurAuth->setText("Veuillez remplir tous les champs.");
        return;
    }

    UserSession session = Authentification::authentifier(email, password);
    if (!session.isValid) {
        m_lblErreurAuth->setText("Email ou mot de passe incorrect (ou compte inactif).");
        return;
    }

    m_userIdConnecte = session.userId;
    m_userRoleConnecte = session.roleStr;
    m_userEmailConnecte = session.email;
    m_userPrenomConnecte = session.prenom;
    m_userNomConnecte = session.nom;

    m_editLoginEmail->clear();
    m_editLoginPassword->clear();

    mettreAJourContextesUtilisateur();

    if (session.role == UserRole::SUPER_ADMIN) {
        m_stackPages->setCurrentIndex(1);
    } else if (session.role == UserRole::ADMIN) {
        m_stackPages->setCurrentIndex(2);
    } else if (session.role == UserRole::FORMATEUR) {
        m_formateurWorkspace->setUser(session.userId, session.prenom, session.nom, session.email);
        m_stackPages->setCurrentIndex(3);
    } else if (session.role == UserRole::STAGIAIRE) {
        m_stagiaireWorkspace->setUser(session.userId, session.prenom, session.nom, session.email);
        m_stackPages->setCurrentIndex(4);
    } else {
        m_stackPages->setCurrentIndex(1);
    }
}

void MainWindow::deconnecter() {
    m_userIdConnecte = -1;
    m_userRoleConnecte.clear();
    m_userEmailConnecte.clear();
    m_userPrenomConnecte.clear();
    m_userNomConnecte.clear();
    m_lblErreurAuth->clear();
    m_stackPages->setCurrentIndex(0);
}

void MainWindow::mettreAJourContextesUtilisateur() {
    QString displayName = !m_userPrenomConnecte.isEmpty() ? m_userPrenomConnecte : m_userEmailConnecte;

    if (m_lblWelcomeAdmin) {
        m_lblWelcomeAdmin->setText(QString("Bienvenue, <b>%1</b>").arg(displayName));
    }
    if (m_lblBadgeAdmin) {
        m_lblBadgeAdmin->setText(QString("<b>%1</b>").arg(m_userRoleConnecte));
    }
    if (m_lblWelcomeFormateur) {
        m_lblWelcomeFormateur->setText(QString("Bienvenue, <b>%1</b> (Formateur)").arg(displayName));
    }
    if (m_lblWelcomeStagiaire) {
        m_lblWelcomeStagiaire->setText(QString("Bienvenue, <b>%1</b> (Stagiaire)").arg(displayName));
    }
}

QWidget* MainWindow::creerDashboardAdmin(bool superAdmin) {
    QWidget* mainDashboard = new QWidget();
    QHBoxLayout* rootLayout = new QHBoxLayout(mainDashboard);
    rootLayout->setContentsMargins(15, 15, 15, 15);
    rootLayout->setSpacing(15);

    
    QFrame* sidebar = new QFrame();
    sidebar->setObjectName("sidebar");
    sidebar->setFixedWidth(220);
    sidebar->setAttribute(Qt::WA_StyledBackground, true);

    QGraphicsDropShadowEffect* sidebarShadow = new QGraphicsDropShadowEffect(sidebar);
    sidebarShadow->setBlurRadius(25);
    sidebarShadow->setColor(QColor(15, 23, 42, 30));
    sidebarShadow->setOffset(0, 6);
    sidebar->setGraphicsEffect(sidebarShadow);

    QVBoxLayout* sidebarLayout = new QVBoxLayout(sidebar);
    sidebarLayout->setContentsMargins(10, 15, 10, 15);

    QLabel* lblLogo = new QLabel(superAdmin ? "<b>CentrePro<br><small>Gouvernance</small></b>" : "<b>CentrePro<br><small>Opérations</small></b>");
    lblLogo->setObjectName("sidebarTitle");
    sidebarLayout->addWidget(lblLogo);
    sidebarLayout->addSpacing(20);

    QPushButton* btnTableauDeBord = new QPushButton("Tableau de Bord");
    QPushButton* btnSalles        = new QPushButton("Salles");
    QPushButton* btnFormateurs    = new QPushButton("Formateurs");
    QPushButton* btnCours         = new QPushButton("Cours");
    QPushButton* btnStagiaires    = new QPushButton("Stagiaires");
    QPushButton* btnLogout        = new QPushButton("Déconnexion");
    btnTableauDeBord->setIcon(ModuleTools::standardIcon(QStyle::SP_ComputerIcon));
    btnSalles->setIcon(ModuleTools::standardIcon(QStyle::SP_DirHomeIcon));
    btnFormateurs->setIcon(ModuleTools::standardIcon(QStyle::SP_FileDialogDetailedView));
    btnCours->setIcon(ModuleTools::standardIcon(QStyle::SP_FileDialogContentsView));
    btnStagiaires->setIcon(ModuleTools::standardIcon(QStyle::SP_FileDialogInfoView));
    btnLogout->setIcon(ModuleTools::standardIcon(QStyle::SP_DialogCloseButton));

    btnTableauDeBord->setCheckable(true);
    btnSalles->setCheckable(true);
    btnFormateurs->setCheckable(true);
    btnCours->setCheckable(true);
    btnStagiaires->setCheckable(true);
    btnTableauDeBord->setChecked(true);

    QButtonGroup* navGroup = new QButtonGroup(mainDashboard);
    navGroup->setExclusive(true);
    navGroup->addButton(btnTableauDeBord, 0);
    navGroup->addButton(btnSalles, 1);
    navGroup->addButton(btnFormateurs, 2);
    navGroup->addButton(btnCours, 3);
    navGroup->addButton(btnStagiaires, 4);

    sidebarLayout->addWidget(btnTableauDeBord);
    sidebarLayout->addWidget(btnSalles);
    sidebarLayout->addWidget(btnFormateurs);
    sidebarLayout->addWidget(btnCours);
    sidebarLayout->addWidget(btnStagiaires);
    sidebarLayout->addStretch();
    sidebarLayout->addWidget(btnLogout);

    connect(btnLogout, &QPushButton::clicked, this, &MainWindow::deconnecter);

    
    QVBoxLayout* rightContentLayout = new QVBoxLayout();
    rightContentLayout->setSpacing(15);

    
    QFrame* topHeader = new QFrame();
    topHeader->setObjectName("glassCard");
    topHeader->setAttribute(Qt::WA_StyledBackground, true);

    QGraphicsDropShadowEffect* headerShadow = new QGraphicsDropShadowEffect(topHeader);
    headerShadow->setBlurRadius(20);
    headerShadow->setColor(QColor(15, 23, 42, 25));
    headerShadow->setOffset(0, 4);
    topHeader->setGraphicsEffect(headerShadow);

    QHBoxLayout* headerLayout = new QHBoxLayout(topHeader);
    headerLayout->setContentsMargins(20, 10, 20, 10);

    m_lblWelcomeAdmin = new QLabel("Bienvenue, <b>Admin</b>");
    m_lblWelcomeAdmin->setStyleSheet("font-size: 13pt; color: #1a2b27;");

    m_lblBadgeAdmin = new QLabel("<b>ADMIN</b>");
    m_lblBadgeAdmin->setStyleSheet("background: rgba(20, 184, 166, 0.15); color: #0f766e; padding: 5px 12px; border-radius: 12px;");

    headerLayout->addWidget(m_lblWelcomeAdmin);
    headerLayout->addStretch();

    QPushButton* btnAdminProfil = new QPushButton("Mon Profil", topHeader);
    btnAdminProfil->setIcon(ModuleTools::standardIcon(QStyle::SP_FileDialogInfoView));
    btnAdminProfil->setObjectName("btnVider");
    btnAdminProfil->setStyleSheet("padding: 6px 14px; font-weight: bold; font-size: 10pt;");
    connect(btnAdminProfil, &QPushButton::clicked, this, &MainWindow::ouvrirProfil);
    headerLayout->addWidget(btnAdminProfil);

    headerLayout->addWidget(m_lblBadgeAdmin);

    
    QStackedWidget* moduleStack = new QStackedWidget();

    
    DashboardWidget* dashWidget = new DashboardWidget(this);
    moduleStack->addWidget(dashWidget);

    
    SalleWidget* salleWidget = new SalleWidget(this);
    moduleStack->addWidget(salleWidget);

    
    FormateurWidget* formateurWidget = new FormateurWidget(superAdmin, this);
    moduleStack->addWidget(formateurWidget);

    
    CoursWidget* coursWidget = new CoursWidget(this);
    moduleStack->addWidget(coursWidget);

    
    StagiaireWidget* stagiaireWidget = new StagiaireWidget(this);
    moduleStack->addWidget(stagiaireWidget);

    
    connect(dashWidget, &DashboardWidget::addRoomRequested, this, [btnSalles, salleWidget]() {
        btnSalles->click();
        salleWidget->afficherFormulaireAjout();
    });

    connect(dashWidget, &DashboardWidget::addCourseRequested, this, [btnCours, coursWidget]() {
        btnCours->click();
        coursWidget->afficherFormulaireAjout();
    });

    connect(dashWidget, &DashboardWidget::viewPlanningRequested, this, [btnSalles]() {
        btnSalles->animateClick();
    });

    
    connect(dashWidget, &DashboardWidget::navigateToSignalsRequested, this, [this, dashWidget]() {
        IncidentResolutionDialog dlg(this);
        dlg.exec();
        dashWidget->refreshDashboard();
    });

    
    connect(navGroup, &QButtonGroup::idClicked, moduleStack, [moduleStack, dashWidget, salleWidget, formateurWidget, coursWidget, stagiaireWidget](int id) {
        if (id == 0) dashWidget->refreshDashboard();
        else if (id == 1) salleWidget->afficherListe();
        else if (id == 2) formateurWidget->afficherListe();
        else if (id == 3) coursWidget->afficherListe();
        else if (id == 4) stagiaireWidget->afficherListe();
        moduleStack->setCurrentIndex(id);
    });

    rightContentLayout->addWidget(topHeader);
    rightContentLayout->addWidget(moduleStack);

    rootLayout->addWidget(sidebar);
    rootLayout->addLayout(rightContentLayout, 1);

    return mainDashboard;
}

QWidget* MainWindow::creerDashboardFormateur() {
    QWidget* page = new QWidget();
    QVBoxLayout* mainLayout = new QVBoxLayout(page);

    QHBoxLayout* header = new QHBoxLayout();
    QLabel* roleTitle = new QLabel("<h2>CentrePro / Espace Formateur</h2>", page);
    header->addWidget(roleTitle);
    header->addStretch();

    QPushButton* btnLogout = new QPushButton("Déconnexion");
    connect(btnLogout, &QPushButton::clicked, this, &MainWindow::deconnecter);
    header->addWidget(btnLogout);

    mainLayout->addLayout(header);
    m_formateurWorkspace = new RoleWorkspace(RoleWorkspace::Mode::Formateur, page);
    connect(m_formateurWorkspace, &RoleWorkspace::openProfileRequested, this, &MainWindow::ouvrirProfil);
    mainLayout->addWidget(m_formateurWorkspace);
    return page;
}

QWidget* MainWindow::creerDashboardStagiaire() {
    QWidget* page = new QWidget();
    QVBoxLayout* mainLayout = new QVBoxLayout(page);

    QHBoxLayout* header = new QHBoxLayout();
    QLabel* roleTitle = new QLabel("<h2>CentrePro / Mon apprentissage</h2>", page);
    header->addWidget(roleTitle);
    header->addStretch();

    QPushButton* btnLogout = new QPushButton("Déconnexion");
    connect(btnLogout, &QPushButton::clicked, this, &MainWindow::deconnecter);
    header->addWidget(btnLogout);

    mainLayout->addLayout(header);
    m_stagiaireWorkspace = new RoleWorkspace(RoleWorkspace::Mode::Stagiaire, page);
    connect(m_stagiaireWorkspace, &RoleWorkspace::openProfileRequested, this, &MainWindow::ouvrirProfil);
    mainLayout->addWidget(m_stagiaireWorkspace);
    return page;
}

void MainWindow::ouvrirProfil() {
    if (m_userIdConnecte < 0) return;

    UserProfileDialog::UserType uType = UserProfileDialog::UserType::Admin;
    if (m_userRoleConnecte == "FORMATEUR") {
        uType = UserProfileDialog::UserType::Formateur;
    } else if (m_userRoleConnecte == "STAGIAIRE") {
        uType = UserProfileDialog::UserType::Stagiaire;
    }

    UserProfileDialog dlg(m_userIdConnecte, uType, this);
    connect(&dlg, &UserProfileDialog::profileUpdated, this, [this](const QString &newPrenom, const QString &newNom) {
        m_userPrenomConnecte = newPrenom;
        m_userNomConnecte = newNom;
        mettreAJourContextesUtilisateur();
        if (m_formateurWorkspace) m_formateurWorkspace->setUser(m_userIdConnecte, newPrenom, newNom, m_userEmailConnecte);
        if (m_stagiaireWorkspace) m_stagiaireWorkspace->setUser(m_userIdConnecte, newPrenom, newNom, m_userEmailConnecte);
    });

    dlg.exec();
}
