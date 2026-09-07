#include "dashboardwidget.h"
#include "db.h"
#include "moduletools.h"

DashboardWidget::DashboardWidget(QWidget *parent)
    : QWidget(parent)
{
    setupUi();
    refreshDashboard();
}

DashboardWidget::~DashboardWidget() {}

void DashboardWidget::setupUi()
{
    
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(25, 25, 25, 25);

    
    
    
    QHBoxLayout *kpiLayout = new QHBoxLayout();
    kpiLayout->setSpacing(15);

    kpiLayout->addWidget(createKpiCard("Total Salles", QStyle::SP_DirHomeIcon, &m_lblTotalSalles, "#2563EB"));
    kpiLayout->addWidget(createKpiCard("Formateurs", QStyle::SP_FileDialogDetailedView, &m_lblTotalFormateurs, "#7C3AED"));
    kpiLayout->addWidget(createKpiCard("Cours Actifs", QStyle::SP_FileDialogContentsView, &m_lblTotalCours, "#059669"));
    kpiLayout->addWidget(createKpiCard("Stagiaires", QStyle::SP_FileDialogInfoView, &m_lblTotalStagiaires, "#D97706"));

    mainLayout->addLayout(kpiLayout);

    
    
    
    QHBoxLayout *middleLayout = new QHBoxLayout();
    middleLayout->setSpacing(20);

    
    QFrame *scheduleCard = new QFrame(this);
    scheduleCard->setStyleSheet("QFrame { background-color: #FFFFFF; border-radius: 12px; border: 1px solid #E5E7EB; }");
    QVBoxLayout *scheduleCardLayout = new QVBoxLayout(scheduleCard);
    scheduleCardLayout->setContentsMargins(20, 20, 20, 20);

    auto *scheduleHeader = new QWidget(scheduleCard);
    auto *scheduleHeaderLayout = new QHBoxLayout(scheduleHeader);
    scheduleHeaderLayout->setContentsMargins(0, 0, 0, 0);
    auto *scheduleIcon = new QLabel(scheduleHeader);
    scheduleIcon->setPixmap(ModuleTools::standardIcon(QStyle::SP_ArrowRight).pixmap(18, 18));
    auto *lblScheduleHeader = new QLabel("Prochains Cours du Jour", scheduleHeader);
    lblScheduleHeader->setStyleSheet("font-size: 16px; font-weight: bold; color: #1F2937; border: none;");
    scheduleHeaderLayout->addWidget(scheduleIcon);
    scheduleHeaderLayout->addWidget(lblScheduleHeader);
    scheduleHeaderLayout->addStretch();
    scheduleCardLayout->addWidget(scheduleHeader);

    
    QFrame *scheduleItemsContainer = new QFrame(scheduleCard);
    scheduleItemsContainer->setStyleSheet("border: none;");
    m_scheduleListLayout = new QVBoxLayout(scheduleItemsContainer);
    m_scheduleListLayout->setContentsMargins(0, 10, 0, 0);
    m_scheduleListLayout->setSpacing(10);
    scheduleCardLayout->addWidget(scheduleItemsContainer);
    scheduleCardLayout->addStretch();

    middleLayout->addWidget(scheduleCard, 3); 

    
    QFrame *signalsCard = new QFrame(this);
    signalsCard->setStyleSheet("QFrame { background-color: #FEF2F2; border-radius: 12px; border: 1px solid #FCA5A5; }");
    QVBoxLayout *signalsCardLayout = new QVBoxLayout(signalsCard);
    signalsCardLayout->setContentsMargins(20, 20, 20, 20);

    QHBoxLayout *signalsHeaderLayout = new QHBoxLayout();
    auto *signalsIcon = new QLabel(signalsCard);
    signalsIcon->setPixmap(ModuleTools::standardIcon(QStyle::SP_MessageBoxWarning).pixmap(18, 18));
    auto *lblSignalsHeader = new QLabel("Signalements", signalsCard);
    lblSignalsHeader->setStyleSheet("font-size: 16px; font-weight: bold; color: #991B1B; border: none;");

    m_lblSignalsCount = new QLabel("0 Actifs", signalsCard);
    m_lblSignalsCount->setStyleSheet("background-color: #EF4444; color: white; border-radius: 10px; padding: 2px 8px; font-weight: bold; border: none;");

    signalsHeaderLayout->addWidget(signalsIcon);
    signalsHeaderLayout->addWidget(lblSignalsHeader);
    signalsHeaderLayout->addStretch();
    signalsHeaderLayout->addWidget(m_lblSignalsCount);
    signalsCardLayout->addLayout(signalsHeaderLayout);

    
    QFrame *previewBox = new QFrame(signalsCard);
    previewBox->setStyleSheet("background-color: #FFFFFF; border-radius: 8px; border: 1px solid #FEE2E2;");
    QVBoxLayout *previewLayout = new QVBoxLayout(previewBox);
    previewLayout->setContentsMargins(12, 12, 12, 12);

    m_lblSignalsPreviewText = new QLabel("Aucun problème signalé.", previewBox);
    m_lblSignalsPreviewText->setWordWrap(true);
    m_lblSignalsPreviewText->setStyleSheet("font-size: 13px; color: #374151; border: none;");

    m_lblSignalsPreviewMeta = new QLabel("", previewBox);
    m_lblSignalsPreviewMeta->setStyleSheet("font-size: 11px; color: #6B7280; font-style: italic; border: none;");

    previewLayout->addWidget(m_lblSignalsPreviewText);
    previewLayout->addWidget(m_lblSignalsPreviewMeta);
    signalsCardLayout->addWidget(previewBox);

    QPushButton *btnViewAllSignals = new QPushButton("Voir tout →", signalsCard);
    btnViewAllSignals->setCursor(Qt::PointingHandCursor);
    btnViewAllSignals->setStyleSheet("QPushButton { background-color: #DC2626; color: white; font-weight: bold; border-radius: 6px; padding: 8px; border: none; }"
                                     "QPushButton:hover { background-color: #B91C1C; }");
    connect(btnViewAllSignals, &QPushButton::clicked, this, &DashboardWidget::navigateToSignalsRequested);

    signalsCardLayout->addStretch();
    signalsCardLayout->addWidget(btnViewAllSignals);

    middleLayout->addWidget(signalsCard, 2); 

    mainLayout->addLayout(middleLayout, 2);

    
    
    
    QHBoxLayout *bottomLayout = new QHBoxLayout();
    bottomLayout->setSpacing(20);

    
    QFrame *usageCard = new QFrame(this);
    usageCard->setStyleSheet("QFrame { background-color: #FFFFFF; border-radius: 12px; border: 1px solid #E5E7EB; }");
    QVBoxLayout *usageCardLayout = new QVBoxLayout(usageCard);
    usageCardLayout->setContentsMargins(20, 20, 20, 20);

    QHBoxLayout *usageHeaderLayout = new QHBoxLayout();
    QLabel *lblUsageIcon = new QLabel(usageCard);
    lblUsageIcon->setPixmap(ModuleTools::standardIcon(QStyle::SP_FileDialogDetailedView).pixmap(18, 18));
    QLabel *lblUsageHeader = new QLabel("Occupation Globale des Salles", usageCard);
    lblUsageHeader->setStyleSheet("font-size: 15px; font-weight: bold; color: #1F2937; border: none;");

    m_lblOccupancyPercent = new QLabel("0%", usageCard);
    m_lblOccupancyPercent->setStyleSheet("font-size: 14px; font-weight: bold; color: #2563EB; border: none;");

    usageHeaderLayout->addWidget(lblUsageIcon);
    usageHeaderLayout->addWidget(lblUsageHeader);
    usageHeaderLayout->addStretch();
    usageHeaderLayout->addWidget(m_lblOccupancyPercent);
    usageCardLayout->addLayout(usageHeaderLayout);

    m_occupancyBar = new QProgressBar(usageCard);
    m_occupancyBar->setRange(0, 100);
    m_occupancyBar->setValue(0);
    m_occupancyBar->setTextVisible(false);
    m_occupancyBar->setFixedHeight(12);
    m_occupancyBar->setStyleSheet("QProgressBar { background-color: #E5E7EB; border-radius: 6px; border: none; }"
                                  "QProgressBar::chunk { background-color: #2563EB; border-radius: 6px; }");
    usageCardLayout->addWidget(m_occupancyBar);

    bottomLayout->addWidget(usageCard, 3);

    
    QFrame *shortcutsCard = new QFrame(this);
    shortcutsCard->setStyleSheet("QFrame { background-color: #FFFFFF; border-radius: 12px; border: 1px solid #E5E7EB; }");
    QVBoxLayout *shortcutsCardLayout = new QVBoxLayout(shortcutsCard);
    shortcutsCardLayout->setContentsMargins(20, 15, 20, 15);

    auto *shortcutsHeader = new QWidget(shortcutsCard);
    auto *shortcutsHeaderLayout = new QHBoxLayout(shortcutsHeader);
    shortcutsHeaderLayout->setContentsMargins(0, 0, 0, 0);
    auto *shortcutsIcon = new QLabel(shortcutsHeader);
    shortcutsIcon->setPixmap(ModuleTools::standardIcon(QStyle::SP_ArrowRight).pixmap(18, 18));
    auto *lblShortcutsHeader = new QLabel("Raccourcis Rapides", shortcutsHeader);
    lblShortcutsHeader->setStyleSheet("font-size: 15px; font-weight: bold; color: #1F2937; border: none;");
    shortcutsHeaderLayout->addWidget(shortcutsIcon);
    shortcutsHeaderLayout->addWidget(lblShortcutsHeader);
    shortcutsHeaderLayout->addStretch();
    shortcutsCardLayout->addWidget(shortcutsHeader);

    QHBoxLayout *btnShortcutLayout = new QHBoxLayout();
    QPushButton *btnAddRoomBtn = new QPushButton("Nouvelle Salle", shortcutsCard);
    QPushButton *btnAddCourseBtn = new QPushButton("Nouveau Cours", shortcutsCard);
    QPushButton *btnViewPlanningBtn = new QPushButton("Planning Salles", shortcutsCard);
    btnAddRoomBtn->setIcon(ModuleTools::standardIcon(QStyle::SP_FileDialogNewFolder));
    btnAddCourseBtn->setIcon(ModuleTools::standardIcon(QStyle::SP_FileDialogNewFolder));
    btnViewPlanningBtn->setIcon(ModuleTools::standardIcon(QStyle::SP_DirOpenIcon));

    QString btnShortcutStyle = "QPushButton { background-color: #F3F4F6; color: #374151; font-weight: 600; border-radius: 6px; padding: 8px 12px; border: 1px solid #D1D5DB; }"
                               "QPushButton:hover { background-color: #E5E7EB; color: #111827; }";

    btnAddRoomBtn->setStyleSheet(btnShortcutStyle);
    btnAddCourseBtn->setStyleSheet(btnShortcutStyle);
    btnViewPlanningBtn->setStyleSheet(btnShortcutStyle);
    btnAddRoomBtn->setCursor(Qt::PointingHandCursor);
    btnAddCourseBtn->setCursor(Qt::PointingHandCursor);
    btnViewPlanningBtn->setCursor(Qt::PointingHandCursor);

    connect(btnAddRoomBtn, &QPushButton::clicked, this, &DashboardWidget::addRoomRequested);
    connect(btnAddCourseBtn, &QPushButton::clicked, this, &DashboardWidget::addCourseRequested);
    connect(btnViewPlanningBtn, &QPushButton::clicked, this, &DashboardWidget::viewPlanningRequested);

    btnShortcutLayout->addWidget(btnAddRoomBtn);
    btnShortcutLayout->addWidget(btnAddCourseBtn);
    btnShortcutLayout->addWidget(btnViewPlanningBtn);
    shortcutsCardLayout->addLayout(btnShortcutLayout);

    bottomLayout->addWidget(shortcutsCard, 2);

    mainLayout->addLayout(bottomLayout, 1);
}

QFrame* DashboardWidget::createKpiCard(const QString &title, QStyle::StandardPixmap icon, QLabel **valueLabel, const QString &accentColor)
{
    QFrame *card = new QFrame(this);
    card->setStyleSheet(QString("QFrame { background-color: #FFFFFF; border-radius: 12px; border: 1px solid #E5E7EB; border-left: 5px solid %1; }").arg(accentColor));

    QVBoxLayout *layout = new QVBoxLayout(card);
    layout->setContentsMargins(15, 15, 15, 15);

    QHBoxLayout *topRow = new QHBoxLayout();
    QLabel *lblTitle = new QLabel(title, card);
    lblTitle->setStyleSheet("font-size: 13px; color: #6B7280; font-weight: 600; border: none;");

    QLabel *lblIcon = new QLabel(card);
    lblIcon->setPixmap(ModuleTools::standardIcon(icon).pixmap(20, 20));
    lblIcon->setStyleSheet("border: none;");

    topRow->addWidget(lblTitle);
    topRow->addStretch();
    topRow->addWidget(lblIcon);

    *valueLabel = new QLabel("0", card);
    (*valueLabel)->setStyleSheet("font-size: 24px; font-weight: bold; color: #111827; border: none; margin-top: 5px;");

    layout->addLayout(topRow);
    layout->addWidget(*valueLabel);

    return card;
}

void DashboardWidget::refreshDashboard()
{
    loadKpiMetrics();
    loadUpcomingCourses();
    loadSignalsSummary();
    loadOccupancyData();
}

void DashboardWidget::loadKpiMetrics()
{
    QSqlQuery query(DB::instance().database());

    query.prepare("SELECT COUNT(*) FROM SALLE");
    if (query.exec() && query.next()) {
        m_lblTotalSalles->setText(query.value(0).toString());
    }
    query.prepare("SELECT COUNT(*) FROM FORMATEUR");
    if (query.exec() && query.next()) {
        m_lblTotalFormateurs->setText(query.value(0).toString());
    }
    query.prepare("SELECT COUNT(*) FROM COURS");
    if (query.exec() && query.next()) {
        m_lblTotalCours->setText(query.value(0).toString());
    }
    query.prepare("SELECT COUNT(*) FROM STAGIAIRE");
    if (query.exec() && query.next()) {
        m_lblTotalStagiaires->setText(query.value(0).toString());
    }
}

void DashboardWidget::loadUpcomingCourses()
{
    
    QLayoutItem *child;
    while ((child = m_scheduleListLayout->takeAt(0)) != nullptr) {
        if (child->widget()) delete child->widget();
        delete child;
    }

    QSqlQuery query(DB::instance().database());
    
    query.prepare(
        "SELECT c.TITRE, "
        "       NVL(f.PRENOM || ' ' || f.NOM, 'Non assigné') AS NOM_FORMATEUR, "
        "       NVL((SELECT MIN(s.NOM_SALLE) FROM STAGIAIRE st JOIN SALLE s ON st.ID_SALLE_ATTITREE = s.ID_SALLE WHERE st.ID_COURS = c.ID_COURS), 'Non assignée') AS NOM_SALLE, "
        "       c.HEURES_REQUISES "
        "FROM COURS c "
        "LEFT JOIN FORMATEUR f ON c.ID_FORMATEUR_RESP = f.ID_FORMATEUR "
        "ORDER BY c.ID_COURS DESC"
    );

    if (query.exec()) {
        bool hasData = false;
        int count = 0;
        while (query.next() && count < 3) {
            hasData = true;
            count++;
            QString courseName = query.value(0).toString();
            QString trainerName = query.value(1).toString();
            QString roomName = query.value(2).toString();
            int hours = query.value(3).toInt();

            QFrame *itemFrame = new QFrame(this);
            itemFrame->setStyleSheet("background-color: #F9FAFB; border-radius: 6px; border: 1px solid #F3F4F6;");
            QHBoxLayout *itemLayout = new QHBoxLayout(itemFrame);
            itemLayout->setContentsMargins(10, 8, 10, 8);

            QLabel *lblInfo = new QLabel(QString("<b>%1</b> — Salle: <b>%2</b> | Formateur: <i>%3</i> (%4h)")
                                             .arg(courseName, roomName, trainerName).arg(hours), itemFrame);
            lblInfo->setStyleSheet("font-size: 13px; color: #374151; border: none;");

            itemLayout->addWidget(lblInfo);
            m_scheduleListLayout->addWidget(itemFrame);
        }

        if (!hasData) {
            QLabel *lblEmpty = new QLabel("Aucun cours enregistré pour le moment.", this);
            lblEmpty->setStyleSheet("color: #9CA3AF; font-style: italic; border: none;");
            m_scheduleListLayout->addWidget(lblEmpty);
        }
    }
}

void DashboardWidget::loadSignalsSummary()
{
    QSqlQuery query(DB::instance().database());
    
    query.prepare(
        "SELECT "
        "  (SELECT COUNT(*) FROM SALLE WHERE REPORT_STATUS IS NOT NULL AND UPPER(TRIM(REPORT_STATUS)) != 'NONE') + "
        "  (SELECT COUNT(*) FROM COURS WHERE REPORT_STATUS IS NOT NULL AND UPPER(TRIM(REPORT_STATUS)) != 'NONE') AS TOTAL_REPORTS "
        "FROM DUAL"
    );

    int totalReports = 0;
    if (query.exec() && query.next()) {
        totalReports = query.value("TOTAL_REPORTS").toInt();
    }

    m_lblSignalsCount->setText(QString("%1 Actif%2").arg(totalReports).arg(totalReports > 1 ? "s" : ""));

    if (totalReports == 0) {
        m_lblSignalsPreviewText->setText("Tout est en ordre. Aucun signalement en attente.");
        m_lblSignalsPreviewMeta->setText("");
    } else {
        
        QSqlQuery previewQuery(DB::instance().database());
        previewQuery.prepare(
            "SELECT TYPE_OBJET, NOM_OBJET, REPORT_DESCRIPTION, REPORT_AUTHOR, REPORT_STATUS FROM ("
            "  SELECT 'SALLE' AS TYPE_OBJET, NOM_SALLE AS NOM_OBJET, REPORT_DESCRIPTION, REPORT_AUTHOR, REPORT_STATUS "
            "  FROM SALLE WHERE REPORT_STATUS IS NOT NULL AND UPPER(TRIM(REPORT_STATUS)) != 'NONE' "
            "  UNION ALL "
            "  SELECT 'COURS' AS TYPE_OBJET, TITRE AS NOM_OBJET, REPORT_DESCRIPTION, REPORT_AUTHOR, REPORT_STATUS "
            "  FROM COURS WHERE REPORT_STATUS IS NOT NULL AND UPPER(TRIM(REPORT_STATUS)) != 'NONE' "
            ") WHERE ROWNUM = 1"
        );

        if (previewQuery.exec() && previewQuery.next()) {
            QString type = previewQuery.value("TYPE_OBJET").toString();
            QString name = previewQuery.value("NOM_OBJET").toString();
            QString desc = previewQuery.value("REPORT_DESCRIPTION").toString();
            QString author = previewQuery.value("REPORT_AUTHOR").toString();
            QString status = previewQuery.value("REPORT_STATUS").toString();

            m_lblSignalsPreviewText->setText(QString("<b>[%1 - %2]:</b> \"%3\"").arg(type, name, desc));
            m_lblSignalsPreviewMeta->setText(QString("Signalé par: %1 | Statut: %2").arg(author, status));
        }
    }
}

void DashboardWidget::loadOccupancyData()
{
    QSqlQuery query(DB::instance().database());
    int totalRooms = 0;
    int occupiedRooms = 0;

    query.prepare("SELECT COUNT(*) FROM SALLE");
    if (query.exec() && query.next()) {
        totalRooms = query.value(0).toInt();
    }

    query.prepare(
            "SELECT COUNT(*) FROM SALLE s "
            "WHERE UPPER(TRIM(s.STATUT)) != 'DISPONIBLE' "
            "OR EXISTS (SELECT 1 FROM SALLE_RESERVATION r "
            "WHERE r.ID_SALLE = s.ID_SALLE AND r.STATUT = 'CONFIRMEE' AND r.DATE_FIN >= SYSDATE)"
        );
    if (query.exec() && query.next()) {
        occupiedRooms = query.value(0).toInt();
    }

    if (totalRooms > 0) {
        int percent = (occupiedRooms * 100) / totalRooms;
        m_occupancyBar->setValue(percent);
        m_lblOccupancyPercent->setText(QString("%1% (%2/%3)").arg(percent).arg(occupiedRooms).arg(totalRooms));
    } else {
        m_occupancyBar->setValue(0);
        m_lblOccupancyPercent->setText("0%");
    }
}