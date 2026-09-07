#include "sallewidget.h"
#include "salle.h"
#include "reservation.h"
#include "moduletools.h"
#include "ollamaassistant.h"
#include <QtCharts/QChartView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QFrame>
#include <QMessageBox>
#include <QHeaderView>
#include <QGraphicsDropShadowEffect>
#include <QScrollArea>
#include <QCoreApplication>

SalleWidget::SalleWidget(QWidget *parent) : QWidget(parent) {
    setAttribute(Qt::WA_StyledBackground, true);
    setupUi();
    rafraichirTable();
}

static QFrame* makeCard() {
    QFrame* card = new QFrame(); card->setObjectName("glassCard");
    QGraphicsDropShadowEffect* sh = new QGraphicsDropShadowEffect(card);
    sh->setBlurRadius(25); sh->setColor(QColor(15,23,42,35)); sh->setOffset(0,8);
    card->setGraphicsEffect(sh);
    return card;
}

void SalleWidget::setupUi() {
    QVBoxLayout* root = new QVBoxLayout(this);
    root->setContentsMargins(10,10,10,10);
    m_stack = new QStackedWidget(this);
    m_stack->addWidget(creerPageListe());
    m_stack->addWidget(creerPageFormulaire());
    root->addWidget(m_stack);
}

QWidget* SalleWidget::creerPageListe() {
    QWidget* page = new QWidget();
    QVBoxLayout* lay = new QVBoxLayout(page);
    lay->setContentsMargins(0,0,0,0);

    QFrame* card = makeCard();
    QVBoxLayout* cl = new QVBoxLayout(card);
    cl->setContentsMargins(24,20,24,20); cl->setSpacing(14);

    QHBoxLayout* hdr = new QHBoxLayout();
    QLabel* title = new QLabel("<b>Salles de Formation</b>");
    title->setStyleSheet("font-size:15pt;color:#0f172a;");
    m_lblCount = new QLabel("0 salles");
    m_lblCount->setStyleSheet("color:#64748b;font-size:10pt;background:rgba(100,116,139,0.1);border-radius:10px;padding:3px 10px;");
    hdr->addWidget(title); hdr->addStretch(); hdr->addWidget(m_lblCount);

    m_tableSalles = new QTableWidget();
    m_tableSalles->setColumnCount(5);
    m_tableSalles->setHorizontalHeaderLabels({"ID","Nom de la salle","Capacite","Type","Statut"});
    m_tableSalles->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tableSalles->setSelectionMode(QAbstractItemView::SingleSelection);
    m_tableSalles->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_tableSalles->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_tableSalles->horizontalHeader()->setSectionResizeMode(0,QHeaderView::ResizeToContents);
    m_tableSalles->verticalHeader()->setVisible(false);
    m_tableSalles->setShowGrid(false);
    m_tableSalles->setAlternatingRowColors(true);
    m_tableSalles->setMinimumHeight(320);

    connect(m_tableSalles,&QTableWidget::itemSelectionChanged,this,&SalleWidget::onSelectionChanged);

    QHBoxLayout* bar = new QHBoxLayout(); bar->setSpacing(10);
    m_btnAjouter  = new QPushButton("+ Nouvelle Salle");
    m_btnModifier = new QPushButton("Modifier");
    m_btnSupprimer= new QPushButton("Supprimer");
    m_btnAjouter->setObjectName("btnAjouter");
    m_btnModifier->setObjectName("btnModifier");
    m_btnSupprimer->setObjectName("btnSupprimer");
    m_btnModifier->setEnabled(false); m_btnSupprimer->setEnabled(false);
    bar->addWidget(m_btnAjouter); bar->addStretch();
    bar->addWidget(m_btnModifier); bar->addWidget(m_btnSupprimer);

    connect(m_btnAjouter,  &QPushButton::clicked,this,&SalleWidget::ouvrirFormulaireAjout);
    connect(m_btnModifier, &QPushButton::clicked,this,&SalleWidget::ouvrirFormulaireModification);
    connect(m_btnSupprimer,&QPushButton::clicked,this,&SalleWidget::supprimerSalle);

    auto *tools = ModuleTools::createMultiCriteriaTools(m_tableSalles, {1, 3, 4}, {"nom", "type", "statut"});
    m_chart = new QChartView(); m_chart->setMinimumHeight(210);
    QPushButton *pdf = new QPushButton("Exporter PDF"); pdf->setObjectName("btnVider");
    bar->insertWidget(1, pdf);
    connect(pdf, &QPushButton::clicked, this, [this] { ModuleTools::exportTableToPdf(m_tableSalles, "Rapport des salles"); });

    auto *reservationTitle = new QLabel("<b>Réservations d'étude actives</b>");
    m_tableReservations = new QTableWidget();
    m_tableReservations->setColumnCount(6);
    m_tableReservations->setHorizontalHeaderLabels({"ID", "Stagiaire", "Salle", "Début", "Fin", "Statut"});
    m_tableReservations->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_tableReservations->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_tableReservations->verticalHeader()->setVisible(false);
    m_tableReservations->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tableReservations->setSelectionMode(QAbstractItemView::SingleSelection);
    m_tableReservations->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_tableReservations->setAlternatingRowColors(true);
    m_tableReservations->setColumnHidden(0, true);
    m_tableReservations->setMinimumHeight(180);
    m_tableReservations->setMaximumHeight(280);

    m_btnAnnulerReservation = new QPushButton("Annuler la réservation sélectionnée");
    m_btnAnnulerReservation->setObjectName("btnSupprimer");
    m_btnAnnulerReservation->setEnabled(false);
    connect(m_tableReservations, &QTableWidget::itemSelectionChanged, this, [this] {
        m_btnAnnulerReservation->setEnabled(!m_tableReservations->selectedItems().isEmpty());
    });
    connect(m_btnAnnulerReservation, &QPushButton::clicked, this, &SalleWidget::annulerReservation);

    auto *reservationActions = new QHBoxLayout();
    reservationActions->addStretch();
    reservationActions->addWidget(m_btnAnnulerReservation);

    cl->addLayout(hdr); cl->addWidget(tools); cl->addWidget(m_tableSalles);
    cl->addWidget(ModuleTools::createPaginationControls(m_tableSalles));
    cl->addWidget(m_chart);
    cl->addWidget(reservationTitle); cl->addWidget(m_tableReservations); cl->addLayout(reservationActions); cl->addLayout(bar);
    cl->addWidget(ModuleTools::createPaginationControls(m_tableReservations, 6));

    auto *aiRoomBox = new QFrame(card);
    aiRoomBox->setStyleSheet("QFrame{background:#f5f3ff;border:1px solid #ddd6fe;border-radius:12px;padding:10px;}QLabel{background:transparent;color:#4c1d95;}");
    auto *aiRoomLayout = new QVBoxLayout(aiRoomBox);
    auto *aiRoomTitle = new QLabel("Assistant IA local pour les salles", aiRoomBox);
    aiRoomTitle->setStyleSheet("font-weight:bold;color:#5b21b6;");
    aiRoomLayout->addWidget(aiRoomTitle);
    auto *aiRoomActions = new QHBoxLayout();
    auto *aiCapacity = new QSpinBox(aiRoomBox);
    aiCapacity->setRange(1, 500);
    aiCapacity->setValue(20);
    aiCapacity->setPrefix("Capacité: ");
    aiCapacity->setMinimumWidth(130);
    aiCapacity->setStyleSheet("QSpinBox{background:#ffffff;color:#0f172a;border:1px solid #a78bfa;border-radius:7px;padding:7px;}");
    auto *aiType = new QComboBox(aiRoomBox);
    aiType->addItems({"Tous les types", "POSTE_TRAVAIL", "SALLE_REUNION", "SALLE_SOUTENANCE"});
    aiType->setMinimumWidth(150);
    aiType->setStyleSheet("QComboBox{background:#ffffff;color:#0f172a;border:1px solid #a78bfa;border-radius:7px;padding:7px;}QComboBox QAbstractItemView{background:#ffffff;color:#0f172a;selection-background-color:#7c3aed;selection-color:#ffffff;}");
    auto *aiButton = new QPushButton("Recommander une salle", aiRoomBox);
    aiButton->setIcon(ModuleTools::standardIcon(QStyle::SP_ComputerIcon));
    aiButton->setStyleSheet("QPushButton{background:#6d28d9;color:#ffffff;font-weight:bold;border:1px solid #5b21b6;border-radius:8px;padding:8px 14px;}QPushButton:hover{background:#5b21b6;}QPushButton:pressed{background:#4c1d95;}");
    aiRoomActions->addWidget(aiCapacity);
    aiRoomActions->addWidget(aiType);
    aiRoomActions->addWidget(aiButton);
    aiRoomActions->addStretch();
    aiRoomLayout->addLayout(aiRoomActions);
    m_aiRoomResult = new QTextEdit(aiRoomBox);
    m_aiRoomResult->setReadOnly(true);
    m_aiRoomResult->setStyleSheet("QTextEdit{background:#ffffff;color:#0f172a;border:1px solid #c4b5fd;border-radius:8px;padding:10px;font-size:10.5pt;}QTextEdit::placeholder{color:#64748b;}");
    m_aiRoomResult->setFixedHeight(90);
    m_aiRoomResult->setPlaceholderText("Ollama proposera une salle sans modifier la base.");
    aiRoomLayout->addWidget(m_aiRoomResult);
    connect(aiButton, &QPushButton::clicked, this, [this, aiCapacity, aiType] {
        QString roomData;
        const int requestedCapacity = aiCapacity->value();
        const QString requestedType = aiType->currentText();
        int eligibleRooms = 0;
        for (const Salle &room : Salle::listerTout()) {
            const bool typeMatches = requestedType == "Tous les types" || room.type() == requestedType;
            const bool statusAvailable = room.statut() == "DISPONIBLE";
            if (!typeMatches || !statusAvailable || room.capacite() < requestedCapacity)
                continue;
            roomData += QString("- %1 | capacité %2 | type %3 | statut %4\n")
                .arg(room.nom()).arg(room.capacite()).arg(room.type()).arg(room.statut());
            ++eligibleRooms;
        }

        if (eligibleRooms == 0) {
            m_aiRoomResult->setPlainText(QString("Aucune salle disponible ne répond à la capacité de %1 personnes et au type demandé.")
                                             .arg(requestedCapacity));
            return;
        }
        m_aiRoomResult->setPlainText("Analyse en cours avec Ollama...");
        QCoreApplication::processEvents();
        QString error;
        const QString result = OllamaAssistant::ask(
            QString("Tu es un assistant de gestion de centre de formation. "
                    "Recommande une seule salle pour %1 étudiants, type demandé: %2. "
                    "Voici uniquement les salles déjà validées par le système:\n%3 "
                    "Tu dois choisir uniquement parmi cette liste. Ne change jamais la capacité et ne recommande jamais une salle absente de la liste. "
                    "Réponds brièvement en français et explique le choix.")
                .arg(requestedCapacity).arg(requestedType, roomData), &error);
        m_aiRoomResult->setPlainText(result.isEmpty() ? error : result);
    });
    cl->addWidget(aiRoomBox);
    auto *scroll = new QScrollArea(page);
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setWidget(card);
    lay->addWidget(scroll);
    return page;
}

QWidget* SalleWidget::creerPageFormulaire() {
    QWidget* page = new QWidget();
    QVBoxLayout* lay = new QVBoxLayout(page);
    lay->setContentsMargins(0,0,0,0);

    QFrame* card = makeCard();
    QVBoxLayout* cl = new QVBoxLayout(card);
    cl->setContentsMargins(32,28,32,28); cl->setSpacing(20);

    QHBoxLayout* bc = new QHBoxLayout();
    QPushButton* back = new QPushButton("<- Retour a la liste");
    back->setObjectName("btnVider"); back->setFixedWidth(180);
    m_lblFormTitre = new QLabel("Nouvelle Salle");
    m_lblFormTitre->setStyleSheet("font-size:14pt;font-weight:bold;color:#0f172a;");
    bc->addWidget(back); bc->addSpacing(16); bc->addWidget(m_lblFormTitre); bc->addStretch();
    connect(back,&QPushButton::clicked,this,&SalleWidget::retourListe);

    QFrame* sep = new QFrame(); sep->setFrameShape(QFrame::HLine); sep->setStyleSheet("color:#e2e8f0;");

    QFormLayout* form = new QFormLayout();
    form->setSpacing(14); form->setLabelAlignment(Qt::AlignRight);
    form->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);

    m_editNomSalle = new QLineEdit(); m_editNomSalle->setPlaceholderText("ex: Labo Reseaux A1");
    m_spinCapacite = new QSpinBox(); m_spinCapacite->setRange(1,500); m_spinCapacite->setValue(25); m_spinCapacite->setSuffix("  places");
    m_comboType    = new QComboBox(); m_comboType->addItems({"POSTE_TRAVAIL","SALLE_SOUTENANCE","SALLE_REUNION"});
    m_comboStatut  = new QComboBox(); m_comboStatut->addItems({"DISPONIBLE","MAINTENANCE","HORS_SERVICE"});

    form->addRow("Nom de la salle *:", m_editNomSalle);
    form->addRow("Capacite:", m_spinCapacite);
    form->addRow("Type:", m_comboType);
    form->addRow("Statut:", m_comboStatut);

    QHBoxLayout* btnRow = new QHBoxLayout(); btnRow->setSpacing(10);
    m_btnEnregistrer = new QPushButton("Enregistrer");
    m_btnAnnuler     = new QPushButton("Annuler");
    m_btnEnregistrer->setObjectName("btnAjouter"); m_btnAnnuler->setObjectName("btnVider");
    m_btnEnregistrer->setMinimumHeight(40); m_btnAnnuler->setMinimumHeight(40);
    btnRow->addStretch(); btnRow->addWidget(m_btnAnnuler); btnRow->addWidget(m_btnEnregistrer);

    connect(m_btnEnregistrer,&QPushButton::clicked,this,&SalleWidget::enregistrer);
    connect(m_btnAnnuler,    &QPushButton::clicked,this,&SalleWidget::retourListe);

    cl->addLayout(bc); cl->addWidget(sep); cl->addLayout(form); cl->addStretch(); cl->addLayout(btnRow);
    lay->addWidget(card);
    return page;
}

void SalleWidget::rafraichirTable() {
    QList<Salle> salles = Salle::listerTout();
    m_tableSalles->setRowCount(0);
    for(int i=0;i<salles.size();++i){
        const Salle& s=salles[i];
        m_tableSalles->insertRow(i);
        m_tableSalles->setItem(i,0,new QTableWidgetItem(QString::number(s.id())));
        m_tableSalles->setItem(i,1,new QTableWidgetItem(s.nom()));
        m_tableSalles->setItem(i,2,new QTableWidgetItem(QString::number(s.capacite())));
        m_tableSalles->setItem(i,3,new QTableWidgetItem(s.type()));
        QString st=s.statut(); QTableWidgetItem* si=new QTableWidgetItem(st);
        if(st=="DISPONIBLE") si->setForeground(QColor("#16a34a"));
        else if(st=="MAINTENANCE") si->setForeground(QColor("#d97706"));
        else si->setForeground(QColor("#dc2626"));
        m_tableSalles->setItem(i,4,si);
    }
    m_lblCount->setText(QString::number(salles.size())+" salle"+(salles.size()>1?"s":""));
    ModuleTools::updateCategoryChart(m_chart, m_tableSalles, 4, "Répartition des salles par statut");
    rafraichirReservations();
    ModuleTools::refreshPagination(m_tableReservations);
    mettreAJourBoutonsListe();
}

void SalleWidget::rafraichirReservations()
{
    m_tableReservations->setRowCount(0);
    const QList<Reservation> reservations = Reservation::listerTout();
    for (int row = 0; row < reservations.size(); ++row) {
        const Reservation &reservation = reservations.at(row);
        m_tableReservations->insertRow(row);
        m_tableReservations->setItem(row, 0, new QTableWidgetItem(QString::number(reservation.id())));
        m_tableReservations->setItem(row, 1, new QTableWidgetItem(reservation.stagiaire()));
        m_tableReservations->setItem(row, 2, new QTableWidgetItem(reservation.salle()));
        m_tableReservations->setItem(row, 3, new QTableWidgetItem(reservation.dateDebut().toString("dd/MM/yyyy HH:mm")));
        m_tableReservations->setItem(row, 4, new QTableWidgetItem(reservation.dateFin().toString("dd/MM/yyyy HH:mm")));
        m_tableReservations->setItem(row, 5, new QTableWidgetItem(reservation.statut()));
    }
    m_btnAnnulerReservation->setEnabled(false);
}
void SalleWidget::afficherListe() {
    retourListe();
    rafraichirTable();
}
void SalleWidget::afficherFormulaireAjout() { ouvrirFormulaireAjout(); }

void SalleWidget::onSelectionChanged(){
    QList<QTableWidgetItem*> sel=m_tableSalles->selectedItems();
    m_idSalleSelectionnee=!sel.isEmpty()?m_tableSalles->item(m_tableSalles->row(sel.first()),0)->text().toInt():-1;
    mettreAJourBoutonsListe();
}
void SalleWidget::mettreAJourBoutonsListe(){
    bool s=(m_idSalleSelectionnee!=-1);
    m_btnModifier->setEnabled(s); m_btnSupprimer->setEnabled(s);
}
void SalleWidget::ouvrirFormulaireAjout(){
    m_modeAjout=true; m_idSalleSelectionnee=-1;
    m_lblFormTitre->setText("Nouvelle Salle");
    m_editNomSalle->clear(); m_editNomSalle->setStyleSheet(""); m_spinCapacite->setValue(25);
    m_comboType->setCurrentIndex(0); m_comboStatut->setCurrentIndex(0);
    m_stack->setCurrentIndex(1);
}
void SalleWidget::ouvrirFormulaireModification(){
    if(m_idSalleSelectionnee==-1) return;
    m_modeAjout=false; remplirFormulaire(m_idSalleSelectionnee); m_stack->setCurrentIndex(1);
}
void SalleWidget::remplirFormulaire(int id){
    Salle s=Salle::trouverParId(id);
    m_lblFormTitre->setText(QString("Modifier : %1").arg(s.nom()));
    m_editNomSalle->setText(s.nom()); m_editNomSalle->setStyleSheet("");
    m_spinCapacite->setValue(s.capacite());
    m_comboType->setCurrentText(s.type()); m_comboStatut->setCurrentText(s.statut());
}
void SalleWidget::enregistrer(){
    QString nom=m_editNomSalle->text().trimmed();
    if(nom.isEmpty()){
        m_editNomSalle->setStyleSheet("border:1.5px solid #f43f5e;border-radius:8px;");
        m_editNomSalle->setPlaceholderText("Ce champ est obligatoire"); return;
    }
    m_editNomSalle->setStyleSheet("");
    if(m_modeAjout){
        Salle s; s.setNom(nom); s.setCapacite(m_spinCapacite->value());
        s.setType(m_comboType->currentText()); s.setStatut(m_comboStatut->currentText());
        if(!s.ajouter()){QMessageBox::critical(this,"Erreur","Echec lors de l'ajout."); return;}
    } else {
        Salle s=Salle::trouverParId(m_idSalleSelectionnee);
        s.setNom(nom); s.setCapacite(m_spinCapacite->value());
        s.setType(m_comboType->currentText()); s.setStatut(m_comboStatut->currentText());
        if(!s.modifier()){QMessageBox::critical(this,"Erreur","Echec de la modification."); return;}
    }
    rafraichirTable(); retourListe();
}
void SalleWidget::supprimerSalle(){
    if(m_idSalleSelectionnee==-1) return;
    QList<QTableWidgetItem*> sel=m_tableSalles->selectedItems();
    QString nomSalle;
    if(!sel.isEmpty()) nomSalle=m_tableSalles->item(m_tableSalles->row(sel.first()),1)->text();
    auto rep=QMessageBox::question(this,"Confirmer la suppression",
        QString("Supprimer la salle <b>%1</b> ?").arg(nomSalle),
        QMessageBox::Yes|QMessageBox::No,QMessageBox::No);
    if(rep==QMessageBox::Yes){
        Salle s=Salle::trouverParId(m_idSalleSelectionnee);
        if(s.supprimer()){m_idSalleSelectionnee=-1; rafraichirTable();}
        else QMessageBox::critical(this,"Erreur","Impossible de supprimer cette salle.");
    }
}

void SalleWidget::annulerReservation()
{
    const QList<QTableWidgetItem *> selected = m_tableReservations->selectedItems();
    if (selected.isEmpty()) {
        return;
    }

    const int row = m_tableReservations->row(selected.first());
    const int idReservation = m_tableReservations->item(row, 0)->text().toInt();
    const QString stagiaire = m_tableReservations->item(row, 1)->text();
    if (QMessageBox::question(this, "Annuler la réservation",
                              QString("Annuler la réservation de <b>%1</b> ?").arg(stagiaire),
                              QMessageBox::Yes | QMessageBox::No, QMessageBox::No) != QMessageBox::Yes) {
        return;
    }

    QString errorMessage;
    if (!Reservation::annuler(idReservation, -1, &errorMessage)) {
        QMessageBox::warning(this, "Annulation impossible", errorMessage);
        return;
    }
    QMessageBox::information(this, "Réservation annulée", "La réservation a été annulée par l'administration.");
    rafraichirTable();
}
void SalleWidget::retourListe(){
    m_idSalleSelectionnee=-1;
    m_tableSalles->clearSelection(); mettreAJourBoutonsListe(); m_stack->setCurrentIndex(0);
}
