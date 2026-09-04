#include "roleworkspace.h"
#include "db.h"
#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QMessageBox>
#include <QProgressBar>
#include <QPushButton>
#include <QSqlQuery>
#include <QSqlError>
#include <QTabWidget>
#include <QTableWidget>
#include <QTextEdit>
#include <QLineEdit>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QDate>
#include <QVBoxLayout>

RoleWorkspace::RoleWorkspace(Mode mode, QWidget *parent)
    : QWidget(parent), m_mode(mode)
{
    setupUi();
}

QWidget* RoleWorkspace::createMetricCard(const QString &label, QLabel **value, const QString &accent) {
    auto *card = new QFrame(this);
    card->setStyleSheet(QString("background:#fff;border:1px solid #e2e8f0;border-left:5px solid %1;border-radius:16px;").arg(accent));
    auto *layout = new QVBoxLayout(card);
    layout->setContentsMargins(18, 14, 18, 14);
    auto *caption = new QLabel(label, card);
    caption->setStyleSheet("color:#64748b;font-size:10pt;font-weight:600;");
    *value = new QLabel("—", card);
    (*value)->setStyleSheet("color:#0f172a;font-size:22pt;font-weight:800;");
    layout->addWidget(caption);
    layout->addWidget(*value);
    return card;
}

void RoleWorkspace::setupUi() {
    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(20, 20, 20, 20);
    root->setSpacing(16);

    // Hero banner
    auto *hero = new QFrame(this);
    hero->setStyleSheet("background:qlineargradient(x1:0,y1:0,x2:1,y2:1,stop:0 #0f172a,stop:1 #1e3a8a);border-radius:20px;");
    auto *heroLayout = new QVBoxLayout(hero);
    heroLayout->setContentsMargins(24, 20, 24, 20);

    m_welcome = new QLabel(hero);
    m_welcome->setStyleSheet("color:white;font-size:20pt;font-weight:800;");
    
    auto *description = new QLabel(m_mode == Mode::Formateur 
        ? "Poste de commandement pédagogique : conduite de séance, présences en direct, compétences et escalade d'incidents."
        : "Hub personnel d'apprentissage : avancement en direct, repères de salle, justifications et signalements.", hero);
    description->setStyleSheet("color:#cbd5e1;font-size:10.5pt;");

    heroLayout->addWidget(m_welcome);
    heroLayout->addWidget(description);

    // KPI row
    auto *metrics = new QGridLayout();
    if (m_mode == Mode::Formateur) {
        metrics->addWidget(createMetricCard("MODULES ASSIGNÉS", &m_primaryMetric, "#38bdf8"), 0, 0);
        metrics->addWidget(createMetricCard("STAGIAIRES SUIVIS", &m_secondaryMetric, "#a78bfa"), 0, 1);
        metrics->addWidget(createMetricCard("CHARGE HORAIRE", &m_tertiaryMetric, "#2dd4bf"), 0, 2);
    } else {
        metrics->addWidget(createMetricCard("HEURES VALIDÉES", &m_primaryMetric, "#38bdf8"), 0, 0);
        metrics->addWidget(createMetricCard("PROGRESSION GLOBALE", &m_secondaryMetric, "#a78bfa"), 0, 1);
        metrics->addWidget(createMetricCard("STATUT DU PARCOURS", &m_tertiaryMetric, "#2dd4bf"), 0, 2);
    }

    root->addWidget(hero);
    root->addLayout(metrics);

    // Dynamic Body based on Role
    if (m_mode == Mode::Formateur) {
        root->addWidget(createFormateurWorkspace(), 1);
    } else {
        root->addWidget(createStagiaireWorkspace(), 1);
    }
}

// ============================================================================
// FORMATEUR WORKSPACE ("The Instructor Command Center")
// ============================================================================
QWidget* RoleWorkspace::createFormateurWorkspace() {
    auto *tabWidget = new QTabWidget(this);
    tabWidget->setStyleSheet(
        "QTabWidget::pane { border: 1px solid #e2e8f0; border-radius: 14px; background: #ffffff; padding: 10px; }"
        "QTabBar::tab { background: #f1f5f9; color: #475569; font-weight: 600; padding: 10px 20px; border-top-left-radius: 8px; border-top-right-radius: 8px; margin-right: 4px; }"
        "QTabBar::tab:selected { background: #ffffff; color: #0284c7; border: 1px solid #e2e8f0; border-bottom: none; }"
    );

    // --- TAB 1: Journal de Séance & Présences ---
    auto *tabSession = new QWidget();
    auto *layoutSession = new QVBoxLayout(tabSession);
    layoutSession->setContentsMargins(16, 16, 16, 16);
    layoutSession->setSpacing(14);

    auto *sessionBar = new QFrame(tabSession);
    sessionBar->setStyleSheet("background: #f8fafc; border: 1px solid #e2e8f0; border-radius: 12px; padding: 10px;");
    auto *sessionBarLayout = new QHBoxLayout(sessionBar);

    sessionBarLayout->addWidget(new QLabel("<b>Module actif :</b>"));
    m_comboFormateurCours = new QComboBox(sessionBar);
    m_comboFormateurCours->setMinimumWidth(220);
    connect(m_comboFormateurCours, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &RoleWorkspace::onFormateurCoursChanged);
    sessionBarLayout->addWidget(m_comboFormateurCours);

    sessionBarLayout->addSpacing(15);
    sessionBarLayout->addWidget(new QLabel("<b>Salle :</b>"));
    m_lblSalleSession = new QLabel("—", sessionBar);
    m_lblSalleSession->setStyleSheet("font-weight: 600; color: #0284c7;");
    sessionBarLayout->addWidget(m_lblSalleSession);

    sessionBarLayout->addSpacing(15);
    sessionBarLayout->addWidget(new QLabel("<b>Volume séance :</b>"));
    m_spinHeuresSession = new QDoubleSpinBox(sessionBar);
    m_spinHeuresSession->setRange(0.5, 8.0);
    m_spinHeuresSession->setSingleStep(0.5);
    m_spinHeuresSession->setValue(2.0);
    m_spinHeuresSession->setSuffix(" h");
    sessionBarLayout->addWidget(m_spinHeuresSession);

    sessionBarLayout->addSpacing(15);
    sessionBarLayout->addWidget(new QLabel("<b>Thème du jour :</b>"));
    m_editSujetSession = new QLineEdit(sessionBar);
    m_editSujetSession->setPlaceholderText("ex: Travaux pratiques administration Linux...");
    sessionBarLayout->addWidget(m_editSujetSession, 1);

    layoutSession->addWidget(sessionBar);

    m_tablePresences = new QTableWidget(tabSession);
    m_tablePresences->setColumnCount(5);
    m_tablePresences->setHorizontalHeaderLabels({"ID", "Stagiaire", "Heures Actuelles", "Émargement du Jour", "Niveau Validé"});
    m_tablePresences->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_tablePresences->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_tablePresences->verticalHeader()->setVisible(false);
    m_tablePresences->setShowGrid(false);
    m_tablePresences->setAlternatingRowColors(true);
    layoutSession->addWidget(m_tablePresences, 1);

    auto *btnValidation = new QPushButton("✅  Valider la Séance & Créditer les Heures aux Présents", tabSession);
    btnValidation->setStyleSheet("background: #0284c7; color: white; font-weight: bold; font-size: 11pt; padding: 12px 24px; border-radius: 10px;");
    connect(btnValidation, &QPushButton::clicked, this, &RoleWorkspace::validerSeanceEtPresences);
    layoutSession->addWidget(btnValidation, 0, Qt::AlignRight);

    tabWidget->addTab(tabSession, "📋 Émargement & Conduite de Séance");

    // --- TAB 2: Suivi & Validation des Compétences ---
    auto *tabSuivi = new QWidget();
    auto *layoutSuivi = new QVBoxLayout(tabSuivi);
    layoutSuivi->setContentsMargins(16, 16, 16, 16);
    layoutSuivi->setSpacing(14);

    auto *infoSuivi = new QLabel("Sélectionnez un stagiaire dans la liste pour mettre à jour son statut ou valider son parcours de formation.", tabSuivi);
    infoSuivi->setStyleSheet("color: #64748b; font-size: 10pt;");
    layoutSuivi->addWidget(infoSuivi);

    m_tableStagiairesSuivi = new QTableWidget(tabSuivi);
    m_tableStagiairesSuivi->setColumnCount(6);
    m_tableStagiairesSuivi->setHorizontalHeaderLabels({"ID", "Stagiaire", "Email", "Heures Validées", "Progression (%)", "Statut Actuel"});
    m_tableStagiairesSuivi->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_tableStagiairesSuivi->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_tableStagiairesSuivi->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tableStagiairesSuivi->setSelectionMode(QAbstractItemView::SingleSelection);
    m_tableStagiairesSuivi->verticalHeader()->setVisible(false);
    m_tableStagiairesSuivi->setAlternatingRowColors(true);
    layoutSuivi->addWidget(m_tableStagiairesSuivi, 1);

    auto *actionSuivi = new QHBoxLayout();
    actionSuivi->addWidget(new QLabel("<b>Changer statut stagiaire sélectionné :</b>"));
    m_comboStatutUpdate = new QComboBox(tabSuivi);
    m_comboStatutUpdate->addItems({"ACTIF", "DIPLOME", "SUSPENDU", "ABANDON"});
    actionSuivi->addWidget(m_comboStatutUpdate);

    auto *btnUpdateStatut = new QPushButton("Enregistrer le statut", tabSuivi);
    btnUpdateStatut->setStyleSheet("background: #0f766e; color: white; font-weight: bold; padding: 8px 18px; border-radius: 8px;");
    connect(btnUpdateStatut, &QPushButton::clicked, this, &RoleWorkspace::mettreAJourStatutStagiaire);
    actionSuivi->addWidget(btnUpdateStatut);
    actionSuivi->addStretch();
    layoutSuivi->addLayout(actionSuivi);

    tabWidget->addTab(tabSuivi, "🎯 Suivi & Progression Stagiaires");

    // --- TAB 3: Signalement Rapide ---
    auto *tabIncident = new QWidget();
    auto *layoutIncident = new QVBoxLayout(tabIncident);
    layoutIncident->setContentsMargins(20, 20, 20, 20);
    layoutIncident->setSpacing(14);

    auto *lblIncidentTitle = new QLabel("<b>Escalade Rapide d'Anomalie (Salle ou Matériel)</b>", tabIncident);
    lblIncidentTitle->setStyleSheet("font-size: 12pt; color: #0f172a;");
    layoutIncident->addWidget(lblIncidentTitle);

    auto *gridIncident = new QGridLayout();
    gridIncident->addWidget(new QLabel("Salle concernée :"), 0, 0);
    m_comboSalleIncidentForm = new QComboBox(tabIncident);
    gridIncident->addWidget(m_comboSalleIncidentForm, 0, 1);

    gridIncident->addWidget(new QLabel("Nature de l'anomalie :"), 1, 0);
    m_comboTypeIncidentForm = new QComboBox(tabIncident);
    m_comboTypeIncidentForm->addItems({"Panne équipement vidéo / projecteur", "Poste stagiaire défectueux", "Problème réseau / connexion", "Matériel ou consommable manquant", "Autre dysfonctionnement"});
    gridIncident->addWidget(m_comboTypeIncidentForm, 1, 1);

    gridIncident->addWidget(new QLabel("Description détaillée :"), 2, 0, Qt::AlignTop);
    m_editDescIncidentForm = new QTextEdit(tabIncident);
    m_editDescIncidentForm->setPlaceholderText("Précisez l'incident pour intervention technique rapide...");
    gridIncident->addWidget(m_editDescIncidentForm, 2, 1);

    layoutIncident->addLayout(gridIncident);

    auto *btnSendIncident = new QPushButton("⚠️  Transmettre le Signalement à l'Administration", tabIncident);
    btnSendIncident->setStyleSheet("background: #e11d48; color: white; font-weight: bold; padding: 10px 20px; border-radius: 8px;");
    connect(btnSendIncident, &QPushButton::clicked, this, &RoleWorkspace::envoyerSignalementFormateur);
    layoutIncident->addWidget(btnSendIncident, 0, Qt::AlignRight);
    layoutIncident->addStretch();

    tabWidget->addTab(tabIncident, "⚠️ Signalement & Escalade");

    return tabWidget;
}

// ============================================================================
// STAGIAIRE PORTAL ("The Learner Hub")
// ============================================================================
QWidget* RoleWorkspace::createStagiaireWorkspace() {
    auto *tabWidget = new QTabWidget(this);
    tabWidget->setStyleSheet(
        "QTabWidget::pane { border: 1px solid #e2e8f0; border-radius: 14px; background: #ffffff; padding: 10px; }"
        "QTabBar::tab { background: #f1f5f9; color: #475569; font-weight: 600; padding: 10px 20px; border-top-left-radius: 8px; border-top-right-radius: 8px; margin-right: 4px; }"
        "QTabBar::tab:selected { background: #ffffff; color: #0284c7; border: 1px solid #e2e8f0; border-bottom: none; }"
    );

    // --- TAB 1: Ma Progression ---
    auto *tabProg = new QWidget();
    auto *layoutProg = new QVBoxLayout(tabProg);
    layoutProg->setContentsMargins(24, 24, 24, 24);
    layoutProg->setSpacing(20);

    auto *lblTitleProg = new QLabel("<b>Feuille de Route d'Apprentissage</b>", tabProg);
    lblTitleProg->setStyleSheet("font-size: 14pt; color: #0f172a;");
    layoutProg->addWidget(lblTitleProg);

    m_barProgression = new QProgressBar(tabProg);
    m_barProgression->setRange(0, 100);
    m_barProgression->setValue(0);
    m_barProgression->setFixedHeight(28);
    m_barProgression->setTextVisible(true);
    m_barProgression->setStyleSheet(
        "QProgressBar { border: 1px solid #cbd5e1; border-radius: 12px; text-align: center; background: #f1f5f9; font-weight: bold; font-size: 10pt; color: #0f172a; }"
        "QProgressBar::chunk { background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #0284c7, stop:1 #38bdf8); border-radius: 12px; }"
    );
    layoutProg->addWidget(m_barProgression);

    m_lblProgressionDetail = new QLabel("Validation en cours...", tabProg);
    m_lblProgressionDetail->setStyleSheet("color: #64748b; font-size: 11pt;");
    layoutProg->addWidget(m_lblProgressionDetail);

    auto *cardSummary = new QFrame(tabProg);
    cardSummary->setStyleSheet("background: #f8fafc; border: 1px solid #e2e8f0; border-radius: 12px; padding: 15px;");
    auto *gridSummary = new QGridLayout(cardSummary);

    gridSummary->addWidget(new QLabel("<b>Module inscrit :</b>"), 0, 0);
    m_lblStagiaireCours = new QLabel("—", cardSummary);
    gridSummary->addWidget(m_lblStagiaireCours, 0, 1);

    gridSummary->addWidget(new QLabel("<b>Formateur référent :</b>"), 1, 0);
    m_lblStagiaireFormateur = new QLabel("—", cardSummary);
    gridSummary->addWidget(m_lblStagiaireFormateur, 1, 1);

    gridSummary->addWidget(new QLabel("<b>Salle de formation :</b>"), 2, 0);
    m_lblStagiaireSalle = new QLabel("—", cardSummary);
    gridSummary->addWidget(m_lblStagiaireSalle, 2, 1);

    gridSummary->addWidget(new QLabel("<b>Période de session :</b>"), 3, 0);
    m_lblStagiairePeriode = new QLabel("—", cardSummary);
    gridSummary->addWidget(m_lblStagiairePeriode, 3, 1);

    layoutProg->addWidget(cardSummary);
    layoutProg->addStretch();
    tabWidget->addTab(tabProg, "🚀 Ma Progression & Compétences");

    // --- TAB 2: Justification d'Absence ---
    auto *tabJustif = new QWidget();
    auto *layoutJustif = new QVBoxLayout(tabJustif);
    layoutJustif->setContentsMargins(24, 24, 24, 24);
    layoutJustif->setSpacing(14);

    auto *lblJustif = new QLabel("<b>Déclaration & Justification d'Absence</b>", tabJustif);
    lblJustif->setStyleSheet("font-size: 13pt; color: #0f172a;");
    layoutJustif->addWidget(lblJustif);

    auto *descJustif = new QLabel("Vous pouvez notifier votre formateur et l'administration d'une absence prévue ou passée.", tabJustif);
    descJustif->setStyleSheet("color: #64748b;");
    layoutJustif->addWidget(descJustif);

    m_editJustification = new QTextEdit(tabJustif);
    m_editJustification->setPlaceholderText("Indiquez la date, le motif d'absence et les détails nécessaires...");
    layoutJustif->addWidget(m_editJustification);

    auto *btnSendJustif = new QPushButton("📨  Transmettre le Justificatif", tabJustif);
    btnSendJustif->setStyleSheet("background: #0284c7; color: white; font-weight: bold; padding: 10px 22px; border-radius: 8px;");
    connect(btnSendJustif, &QPushButton::clicked, this, &RoleWorkspace::soumettreJustificationStagiaire);
    layoutJustif->addWidget(btnSendJustif, 0, Qt::AlignRight);

    tabWidget->addTab(tabJustif, "📨 Justification d'Absence");

    // --- TAB 3: Signalement Stagiaire ---
    auto *tabIncidentStag = new QWidget();
    auto *layoutIncidentStag = new QVBoxLayout(tabIncidentStag);
    layoutIncidentStag->setContentsMargins(24, 24, 24, 24);
    layoutIncidentStag->setSpacing(14);

    auto *lblIncStag = new QLabel("<b>Signaler une anomalie (Salle ou Poste)</b>", tabIncidentStag);
    lblIncStag->setStyleSheet("font-size: 13pt; color: #0f172a;");
    layoutIncidentStag->addWidget(lblIncStag);

    auto *gridIncStag = new QGridLayout();
    gridIncStag->addWidget(new QLabel("Nature du problème :"), 0, 0);
    m_comboTypeIncidentStag = new QComboBox(tabIncidentStag);
    m_comboTypeIncidentStag->addItems({"Poste de travail défectueux", "Climatisation / éclairage salle", "Problème d'accès logiciel / réseau", "Autre"});
    gridIncStag->addWidget(m_comboTypeIncidentStag, 0, 1);

    gridIncStag->addWidget(new QLabel("Explication :"), 1, 0, Qt::AlignTop);
    m_editDescIncidentStag = new QTextEdit(tabIncidentStag);
    m_editDescIncidentStag->setPlaceholderText("Expliquez brièvement l'anomalie rencontrée...");
    gridIncStag->addWidget(m_editDescIncidentStag, 1, 1);

    layoutIncidentStag->addLayout(gridIncStag);

    auto *btnSendIncStag = new QPushButton("⚠️  Envoyer le Signalement", tabIncidentStag);
    btnSendIncStag->setStyleSheet("background: #e11d48; color: white; font-weight: bold; padding: 10px 22px; border-radius: 8px;");
    connect(btnSendIncStag, &QPushButton::clicked, this, &RoleWorkspace::envoyerSignalementStagiaire);
    layoutIncidentStag->addWidget(btnSendIncStag, 0, Qt::AlignRight);
    layoutIncidentStag->addStretch();

    tabWidget->addTab(tabIncidentStag, "⚠️ Signalement");

    return tabWidget;
}

void RoleWorkspace::setUser(int id, const QString &firstName, const QString &lastName) {
    m_userId = id;
    m_userFirstName = firstName;
    m_userLastName = lastName;
    m_welcome->setText(QString("Bonjour, %1").arg(firstName));
    refresh();
}

void RoleWorkspace::refresh() {
    if (m_userId < 0) return;
    if (m_mode == Mode::Formateur) {
        refreshFormateur();
    } else {
        refreshStagiaire();
    }
}

// ============================================================================
// DATA LOGIC - FORMATEUR
// ============================================================================
void RoleWorkspace::refreshFormateur() {
    QSqlQuery query(DB::instance().database());

    // KPIs
    query.prepare("SELECT COUNT(*), NVL(SUM(HEURES_REQUISES), 0) FROM COURS WHERE ID_FORMATEUR_RESP = :id");
    query.bindValue(":id", m_userId);
    if (query.exec() && query.next()) {
        m_primaryMetric->setText(query.value(0).toString());
        m_tertiaryMetric->setText(query.value(1).toString() + " h");
    }

    query.prepare("SELECT COUNT(*) FROM STAGIAIRE WHERE ID_FORMATEUR = :id");
    query.bindValue(":id", m_userId);
    if (query.exec() && query.next()) {
        m_secondaryMetric->setText(query.value(0).toString());
    }

    // Populate Courses combo
    m_comboFormateurCours->blockSignals(true);
    m_comboFormateurCours->clear();
    query.prepare("SELECT ID_COURS, TITRE FROM COURS WHERE ID_FORMATEUR_RESP = :id ORDER BY TITRE");
    query.bindValue(":id", m_userId);
    if (query.exec()) {
        while (query.next()) {
            m_comboFormateurCours->addItem(query.value(1).toString(), query.value(0).toInt());
        }
    }
    m_comboFormateurCours->blockSignals(false);

    // Populate Rooms for Incident combo
    m_comboSalleIncidentForm->clear();
    query.prepare("SELECT ID_SALLE, NOM_SALLE FROM SALLE ORDER BY NOM_SALLE");
    if (query.exec()) {
        while (query.next()) {
            m_comboSalleIncidentForm->addItem(query.value(1).toString(), query.value(0).toInt());
        }
    }

    if (m_comboFormateurCours->count() > 0) {
        onFormateurCoursChanged(0);
    }
}

void RoleWorkspace::onFormateurCoursChanged(int index) {
    if (index < 0) return;
    int coursId = m_comboFormateurCours->currentData().toInt();

    QSqlQuery query(DB::instance().database());

    // Resolve assigned room for this course
    query.prepare("SELECT NVL(sa.NOM_SALLE, 'Non assignée') "
                  "FROM STAGIAIRE s JOIN SALLE sa ON s.ID_SALLE_ATTITREE = sa.ID_SALLE "
                  "WHERE s.ID_COURS = :cid AND ROWNUM = 1");
    query.bindValue(":cid", coursId);
    if (query.exec() && query.next()) {
        m_lblSalleSession->setText(query.value(0).toString());
    } else {
        m_lblSalleSession->setText("Non assignée");
    }

    // Populate Attendance Table
    m_tablePresences->setRowCount(0);
    query.prepare("SELECT ID_STAGIAIRE, PRENOM || ' ' || NOM, NVL(HEURES_VALIDEES, 0) "
                  "FROM STAGIAIRE WHERE ID_COURS = :cid AND ID_FORMATEUR = :fid ORDER BY NOM, PRENOM");
    query.bindValue(":cid", coursId);
    query.bindValue(":fid", m_userId);
    if (query.exec()) {
        int row = 0;
        while (query.next()) {
            m_tablePresences->insertRow(row);
            m_tablePresences->setItem(row, 0, new QTableWidgetItem(query.value(0).toString()));
            m_tablePresences->setItem(row, 1, new QTableWidgetItem(query.value(1).toString()));
            m_tablePresences->setItem(row, 2, new QTableWidgetItem(QString::number(query.value(2).toDouble(), 'f', 1) + " h"));

            auto *comboStatut = new QComboBox();
            comboStatut->addItems({"Présent", "Absent", "Retard / Excusé"});
            m_tablePresences->setCellWidget(row, 3, comboStatut);

            auto *comboNiveau = new QComboBox();
            comboNiveau->addItems({"Niveau 1 (Initié)", "Niveau 2 (Intermédiaire)", "Niveau 3 (Autonome)", "Niveau 4 (Maîtrise)"});
            m_tablePresences->setCellWidget(row, 4, comboNiveau);

            ++row;
        }
    }

    // Populate Suivi Table
    m_tableStagiairesSuivi->setRowCount(0);
    query.prepare("SELECT s.ID_STAGIAIRE, s.PRENOM || ' ' || s.NOM, s.EMAIL, NVL(s.HEURES_VALIDEES, 0), "
                  "NVL(c.HEURES_REQUISES, 0), s.STATUT "
                  "FROM STAGIAIRE s LEFT JOIN COURS c ON s.ID_COURS = c.ID_COURS "
                  "WHERE s.ID_FORMATEUR = :fid ORDER BY s.NOM, s.PRENOM");
    query.bindValue(":fid", m_userId);
    if (query.exec()) {
        int row = 0;
        while (query.next()) {
            m_tableStagiairesSuivi->insertRow(row);
            m_tableStagiairesSuivi->setItem(row, 0, new QTableWidgetItem(query.value(0).toString()));
            m_tableStagiairesSuivi->setItem(row, 1, new QTableWidgetItem(query.value(1).toString()));
            m_tableStagiairesSuivi->setItem(row, 2, new QTableWidgetItem(query.value(2).toString()));

            double done = query.value(3).toDouble();
            double total = query.value(4).toDouble();
            int pct = total > 0 ? qBound(0, qRound((done / total) * 100.0), 100) : 0;

            m_tableStagiairesSuivi->setItem(row, 3, new QTableWidgetItem(QString::number(done, 'f', 1) + " h"));
            m_tableStagiairesSuivi->setItem(row, 4, new QTableWidgetItem(QString::number(pct) + " %"));
            m_tableStagiairesSuivi->setItem(row, 5, new QTableWidgetItem(query.value(5).toString()));

            ++row;
        }
    }
}

void RoleWorkspace::validerSeanceEtPresences() {
    double heures = m_spinHeuresSession->value();
    int countValidated = 0;

    QSqlQuery query(DB::instance().database());

    for (int r = 0; r < m_tablePresences->rowCount(); ++r) {
        int idStagiaire = m_tablePresences->item(r, 0)->text().toInt();
        auto *combo = qobject_cast<QComboBox*>(m_tablePresences->cellWidget(r, 3));
        if (combo && combo->currentText() == "Présent") {
            query.prepare("UPDATE STAGIAIRE SET HEURES_VALIDEES = NVL(HEURES_VALIDEES, 0) + :h WHERE ID_STAGIAIRE = :id");
            query.bindValue(":h", heures);
            query.bindValue(":id", idStagiaire);
            if (query.exec()) {
                countValidated++;
            }
        }
    }

    QMessageBox::information(this, "Séance validée",
        QString("Émargement enregistré avec succès !\n\n%1 stagiaire(s) présent(s) crédité(s) de +%2 heures.")
        .arg(countValidated).arg(heures));

    onFormateurCoursChanged(m_comboFormateurCours->currentIndex());
}

void RoleWorkspace::mettreAJourStatutStagiaire() {
    auto selected = m_tableStagiairesSuivi->selectedItems();
    if (selected.isEmpty()) {
        QMessageBox::warning(this, "Sélection requise", "Veuillez sélectionner un stagiaire dans le tableau.");
        return;
    }

    int row = m_tableStagiairesSuivi->row(selected.first());
    int idStagiaire = m_tableStagiairesSuivi->item(row, 0)->text().toInt();
    QString nouveauStatut = m_comboStatutUpdate->currentText();

    QSqlQuery query(DB::instance().database());
    query.prepare("UPDATE STAGIAIRE SET STATUT = :st WHERE ID_STAGIAIRE = :id");
    query.bindValue(":st", nouveauStatut);
    query.bindValue(":id", idStagiaire);

    if (query.exec()) {
        QMessageBox::information(this, "Statut mis à jour", "Le statut du stagiaire a été mis à jour.");
        onFormateurCoursChanged(m_comboFormateurCours->currentIndex());
    } else {
        QMessageBox::critical(this, "Erreur", "Impossible de mettre à jour le statut.");
    }
}

void RoleWorkspace::envoyerSignalementFormateur() {
    int salleId = m_comboSalleIncidentForm->currentData().toInt();
    QString nature = m_comboTypeIncidentForm->currentText();
    QString desc = m_editDescIncidentForm->toPlainText().trimmed();

    if (desc.isEmpty()) {
        QMessageBox::warning(this, "Champs requis", "Veuillez préciser la description de l'incident.");
        return;
    }

    QString incidentComplet = QString("[%1] %2").arg(nature, desc);

    QSqlQuery query(DB::instance().database());
    query.prepare("UPDATE SALLE SET REPORT_STATUS = 'EN_COURS', REPORT_DESCRIPTION = :desc, "
                  "REPORT_AUTHOR = :author WHERE ID_SALLE = :id");
    query.bindValue(":desc", incidentComplet);
    query.bindValue(":author", QString("%1 %2 (Formateur)").arg(m_userFirstName, m_userLastName));
    query.bindValue(":id", salleId);

    if (query.exec()) {
        QMessageBox::information(this, "Signalement transmis", "L'anomalie a été signalée à l'administration.");
        m_editDescIncidentForm->clear();
    } else {
        QMessageBox::critical(this, "Erreur", "Échec de l'envoi du signalement.");
    }
}

// ============================================================================
// DATA LOGIC - STAGIAIRE
// ============================================================================
void RoleWorkspace::refreshStagiaire() {
    QSqlQuery query(DB::instance().database());

    query.prepare(
        "SELECT NVL(c.TITRE, 'Non attribué'), "
        "       NVL(f.PRENOM || ' ' || f.NOM, 'Non assigné'), "
        "       NVL(sa.NOM_SALLE, 'Non assignée'), "
        "       TO_CHAR(s.DATE_DEBUT, 'DD/MM/YYYY') || ' au ' || TO_CHAR(s.DATE_FIN_PREVUE, 'DD/MM/YYYY'), "
        "       NVL(s.HEURES_VALIDEES, 0), "
        "       NVL(c.HEURES_REQUISES, 0), "
        "       s.STATUT, "
        "       s.ID_COURS, "
        "       s.ID_SALLE_ATTITREE "
        "FROM STAGIAIRE s "
        "LEFT JOIN COURS c ON s.ID_COURS = c.ID_COURS "
        "LEFT JOIN FORMATEUR f ON s.ID_FORMATEUR = f.ID_FORMATEUR "
        "LEFT JOIN SALLE sa ON s.ID_SALLE_ATTITREE = sa.ID_SALLE "
        "WHERE s.ID_STAGIAIRE = :id"
    );
    query.bindValue(":id", m_userId);

    if (query.exec() && query.next()) {
        m_lblStagiaireCours->setText(query.value(0).toString());
        m_lblStagiaireFormateur->setText(query.value(1).toString());
        m_lblStagiaireSalle->setText(query.value(2).toString());
        m_lblStagiairePeriode->setText(query.value(3).toString());

        double done = query.value(4).toDouble();
        double total = query.value(5).toDouble();
        QString statut = query.value(6).toString();

        int pct = total > 0 ? qBound(0, qRound((done / total) * 100.0), 100) : 0;
        double restant = qMax(0.0, total - done);

        m_primaryMetric->setText(QString::number(done, 'f', 1) + " h");
        m_secondaryMetric->setText(QString::number(pct) + " %");
        m_tertiaryMetric->setText(statut);

        m_barProgression->setValue(pct);
        m_lblProgressionDetail->setText(
            QString("<b>%1 h validées</b> sur un total de <b>%2 h requises</b> (%3 h restantes pour diplomation).")
            .arg(QString::number(done, 'f', 1))
            .arg(QString::number(total, 'f', 1))
            .arg(QString::number(restant, 'f', 1))
        );
    }
}

void RoleWorkspace::soumettreJustificationStagiaire() {
    QString texte = m_editJustification->toPlainText().trimmed();
    if (texte.isEmpty()) {
        QMessageBox::warning(this, "Champs requis", "Veuillez rédiger le motif de votre absence.");
        return;
    }

    // Escalate via course report
    QSqlQuery query(DB::instance().database());
    query.prepare(
        "UPDATE COURS SET REPORT_STATUS = 'EN_COURS', "
        "REPORT_DESCRIPTION = :desc, "
        "REPORT_AUTHOR = :author "
        "WHERE ID_COURS = (SELECT ID_COURS FROM STAGIAIRE WHERE ID_STAGIAIRE = :id)"
    );
    query.bindValue(":desc", QString("[Demande Absence / Justificatif] %1").arg(texte));
    query.bindValue(":author", QString("%1 %2 (Stagiaire)").arg(m_userFirstName, m_userLastName));
    query.bindValue(":id", m_userId);

    if (query.exec()) {
        QMessageBox::information(this, "Justificatif envoyé", "Votre justification d'absence a été transmise à votre formateur.");
        m_editJustification->clear();
    } else {
        QMessageBox::critical(this, "Erreur", "Échec lors de l'envoi de la justification.");
    }
}

void RoleWorkspace::envoyerSignalementStagiaire() {
    QString desc = m_editDescIncidentStag->toPlainText().trimmed();
    QString type = m_comboTypeIncidentStag->currentText();

    if (desc.isEmpty()) {
        QMessageBox::warning(this, "Champs requis", "Veuillez décrire le problème rencontré.");
        return;
    }

    QSqlQuery query(DB::instance().database());
    query.prepare(
        "UPDATE SALLE SET REPORT_STATUS = 'EN_COURS', "
        "REPORT_DESCRIPTION = :desc, "
        "REPORT_AUTHOR = :author "
        "WHERE ID_SALLE = (SELECT ID_SALLE_ATTITREE FROM STAGIAIRE WHERE ID_STAGIAIRE = :id)"
    );
    query.bindValue(":desc", QString("[%1] %2").arg(type, desc));
    query.bindValue(":author", QString("%1 %2 (Stagiaire)").arg(m_userFirstName, m_userLastName));
    query.bindValue(":id", m_userId);

    if (query.exec()) {
        QMessageBox::information(this, "Signalement envoyé", "Votre signalement a été transmis à l'équipe technique.");
        m_editDescIncidentStag->clear();
    } else {
        QMessageBox::critical(this, "Erreur", "Échec lors de l'envoi du signalement.");
    }
}
