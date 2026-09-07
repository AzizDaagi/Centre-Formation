#include "incidentresolutiondialog.h"
#include "db.h"
#include "cours.h"
#include "salle.h"
#include "moduletools.h"
#include "emailnotifier.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QPushButton>
#include <QTableWidget>
#include <QComboBox>
#include <QMessageBox>
#include <QSqlQuery>
#include <QTextEdit>

IncidentResolutionDialog::IncidentResolutionDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("CentrePro — Centre de Résolution des Incidents & Justifications");
    resize(900, 680);
    setupUi();
    rafraichirIncidents();
}

void IncidentResolutionDialog::setupUi() {
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(18, 18, 18, 18);
    layout->setSpacing(14);

    auto *headerLayout = new QHBoxLayout();
    auto *title = new QLabel("<h3>Gestion & Résolution des Signalements (Salles & Cours)</h3>", this);
    m_filterType = new QComboBox(this);
    m_filterType->addItems({"Tous les types", "SALLE", "COURS"});
    connect(m_filterType, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &IncidentResolutionDialog::rafraichirIncidents);

    headerLayout->addWidget(title);
    headerLayout->addStretch();
    headerLayout->addWidget(new QLabel("Filtrer par entité :", this));
    headerLayout->addWidget(m_filterType);
    layout->addLayout(headerLayout);

    m_table = new QTableWidget(this);
    m_table->setColumnCount(6);
    m_table->setHorizontalHeaderLabels({"ID", "Type", "Élément", "Description de l'incident / Justificatif", "Auteur", "Statut"});
    m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    m_table->horizontalHeader()->setSectionResizeMode(5, QHeaderView::ResizeToContents);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->verticalHeader()->setVisible(false);
    m_table->setAlternatingRowColors(true);
    m_table->setMinimumHeight(260);
    layout->addWidget(m_table, 1);
    layout->addWidget(ModuleTools::createPaginationControls(m_table, 6));

    m_detailDescription = new QTextEdit(this);
    m_detailDescription->setReadOnly(true);
    m_detailDescription->setPlaceholderText("Sélectionnez un signalement pour lire sa description complète.");
    m_detailDescription->setMinimumHeight(130);
    layout->addWidget(m_detailDescription);
    connect(m_table, &QTableWidget::itemSelectionChanged, this, [this] {
        const auto selected = m_table->selectedItems();
        m_detailDescription->setPlainText(selected.isEmpty() ? QString() : m_table->item(m_table->row(selected.first()), 3)->text());
    });

    auto *actionBar = new QHBoxLayout();
    actionBar->setSpacing(10);

    m_btnResolu = new QPushButton("Marquer comme Résolu / Validé", this);
    m_btnResolu->setIcon(ModuleTools::standardIcon(QStyle::SP_DialogApplyButton));
    m_btnResolu->setStyleSheet("background: #16a34a; color: white; font-weight: bold; padding: 8px 16px; border-radius: 8px;");
    connect(m_btnResolu, &QPushButton::clicked, this, &IncidentResolutionDialog::resoudreSelection);

    m_btnEnCours = new QPushButton("Prendre en Charge (En cours)", this);
    m_btnEnCours->setIcon(ModuleTools::standardIcon(QStyle::SP_BrowserReload));
    m_btnEnCours->setStyleSheet("background: #0284c7; color: white; font-weight: bold; padding: 8px 16px; border-radius: 8px;");
    connect(m_btnEnCours, &QPushButton::clicked, this, &IncidentResolutionDialog::marquerEnCoursSelection);

    m_btnSupprimer = new QPushButton("Clôturer & Effacer", this);
    m_btnSupprimer->setIcon(ModuleTools::standardIcon(QStyle::SP_TrashIcon));
    m_btnSupprimer->setStyleSheet("background: #e11d48; color: white; font-weight: bold; padding: 8px 16px; border-radius: 8px;");
    connect(m_btnSupprimer, &QPushButton::clicked, this, &IncidentResolutionDialog::supprimerSignalementSelection);

    auto *btnFermer = new QPushButton("Fermer", this);
    btnFermer->setObjectName("btnVider");
    btnFermer->setStyleSheet("padding: 8px 16px;");
    connect(btnFermer, &QPushButton::clicked, this, &QDialog::accept);

    actionBar->addWidget(m_btnResolu);
    actionBar->addWidget(m_btnEnCours);
    actionBar->addWidget(m_btnSupprimer);
    actionBar->addStretch();
    actionBar->addWidget(btnFermer);

    layout->addLayout(actionBar);
}

void IncidentResolutionDialog::rafraichirIncidents() {
    m_table->setRowCount(0);
    m_detailDescription->clear();
    QString filter = m_filterType->currentText();

    QSqlQuery query(DB::instance().database());
    QString sql = 
        "SELECT ID_OBJET, TYPE_OBJET, NOM_OBJET, REPORT_DESCRIPTION, REPORT_AUTHOR, REPORT_STATUS FROM ("
        "  SELECT ID_SALLE AS ID_OBJET, 'SALLE' AS TYPE_OBJET, NOM_SALLE AS NOM_OBJET, "
        "         REPORT_DESCRIPTION, REPORT_AUTHOR, REPORT_STATUS "
        "  FROM SALLE WHERE REPORT_STATUS IS NOT NULL AND UPPER(TRIM(REPORT_STATUS)) NOT IN ('NONE', 'CLOTURE') "
        "  UNION ALL "
        "  SELECT ID_COURS AS ID_OBJET, 'COURS' AS TYPE_OBJET, TITRE AS NOM_OBJET, "
        "         REPORT_DESCRIPTION, REPORT_AUTHOR, REPORT_STATUS "
        "  FROM COURS WHERE REPORT_STATUS IS NOT NULL AND UPPER(TRIM(REPORT_STATUS)) NOT IN ('NONE', 'CLOTURE') "
        ") WHERE 1=1 ";

    if (filter == "SALLE") {
        sql += " AND TYPE_OBJET = 'SALLE' ";
    } else if (filter == "COURS") {
        sql += " AND TYPE_OBJET = 'COURS' ";
    }
    sql += " ORDER BY TYPE_OBJET, ID_OBJET";

    query.prepare(sql);
    if (query.exec()) {
        int row = 0;
        while (query.next()) {
            m_table->insertRow(row);
            m_table->setItem(row, 0, new QTableWidgetItem(query.value(0).toString()));
            m_table->setItem(row, 1, new QTableWidgetItem(query.value(1).toString()));
            m_table->setItem(row, 2, new QTableWidgetItem(query.value(2).toString()));
            m_table->setItem(row, 3, new QTableWidgetItem(query.value(3).toString()));
            m_table->setItem(row, 4, new QTableWidgetItem(query.value(4).toString()));

            QString status = query.value(5).toString();
            auto *statusItem = new QTableWidgetItem(status);
            if (status.contains("RESOLU", Qt::CaseInsensitive)) {
                statusItem->setForeground(QColor("#16a34a"));
            } else if (status.contains("EN_COURS", Qt::CaseInsensitive)) {
                statusItem->setForeground(QColor("#0284c7"));
            } else {
                statusItem->setForeground(QColor("#e11d48"));
            }
            m_table->setItem(row, 5, statusItem);
            row++;
        }
    }
    ModuleTools::refreshPagination(m_table);
}

void IncidentResolutionDialog::resoudreSelection() {
    executerMiseAJour("RESOLU", false);
}

void IncidentResolutionDialog::marquerEnCoursSelection() {
    executerMiseAJour("EN_COURS", false);
}

void IncidentResolutionDialog::supprimerSignalementSelection() {
    executerMiseAJour("NONE", true);
}

void IncidentResolutionDialog::executerMiseAJour(const QString &nouveauStatut, bool effacerComplet) {
    auto sel = m_table->selectedItems();
    if (sel.isEmpty()) {
        QMessageBox::warning(this, "Sélection requise", "Veuillez sélectionner un signalement dans la liste.");
        return;
    }

    int row = m_table->row(sel.first());
    int idObjet = m_table->item(row, 0)->text().toInt();
    QString typeObjet = m_table->item(row, 1)->text();
    const QString description = m_table->item(row, 3)->text();
    const QString auteur = m_table->item(row, 4)->text();

    bool ok = false;

    if (typeObjet == "SALLE") {
        if (effacerComplet) {
            ok = Salle::effacerSignalement(idObjet);
        } else {
            ok = Salle::mettreAJourSignalement(idObjet, nouveauStatut, description, auteur);
        }
    } else if (typeObjet == "COURS") {
        if (effacerComplet) {
            ok = Cours::effacerSignalement(idObjet);
        } else {
            ok = Cours::mettreAJourSignalement(idObjet, nouveauStatut, description, auteur);
        }
    }

    if (ok) {
        if (!effacerComplet) {
            EmailNotifier::notifyReportResolved(typeObjet, m_table->item(row, 2)->text(),
                                                description, auteur, nouveauStatut);
        }
        QMessageBox::information(this, "Statut mis à jour", QString("Le signalement a été marqué comme : %1").arg(nouveauStatut));
        rafraichirIncidents();
    } else {
        QMessageBox::critical(this, "Erreur", "Impossible de mettre à jour le signalement dans la base Oracle.");
    }
}
