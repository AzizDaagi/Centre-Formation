#include "stagiairewidget.h"
#include "stagiaire.h"
#include "formateur.h"
#include "cours.h"
#include "salle.h"
#include "authentification.h"
#include "db.h"
#include <QSqlQuery>
#include "moduletools.h"
#include <QtCharts/QChartView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QFrame>
#include <QMessageBox>
#include <QHeaderView>
#include <QScrollArea>
#include <QGraphicsDropShadowEffect>

static QFrame* makeCardS(){
    QFrame* c=new QFrame(); c->setObjectName("glassCard");
    QGraphicsDropShadowEffect* sh=new QGraphicsDropShadowEffect(c);
    sh->setBlurRadius(25); sh->setColor(QColor(15,23,42,35)); sh->setOffset(0,8); c->setGraphicsEffect(sh);
    return c;
}

StagiaireWidget::StagiaireWidget(QWidget *parent):QWidget(parent){ setAttribute(Qt::WA_StyledBackground,true); setupUi(); rafraichirTable(); }
void StagiaireWidget::setupUi(){
    QVBoxLayout* root=new QVBoxLayout(this); root->setContentsMargins(10,10,10,10);
    m_stack=new QStackedWidget(this);
    m_stack->addWidget(creerPageListe()); m_stack->addWidget(creerPageFormulaire());
    root->addWidget(m_stack);
}
QWidget* StagiaireWidget::creerPageListe(){
    QWidget* page=new QWidget(); QVBoxLayout* lay=new QVBoxLayout(page); lay->setContentsMargins(0,0,0,0);
    QFrame* card=makeCardS(); QVBoxLayout* cl=new QVBoxLayout(card); cl->setContentsMargins(24,20,24,20); cl->setSpacing(14);
    QHBoxLayout* hdr=new QHBoxLayout();
    QLabel* lbl=new QLabel("<b>Stagiaires et Inscriptions</b>"); lbl->setStyleSheet("font-size:15pt;color:#0f172a;");
    m_lblCount=new QLabel("0 stagiaires"); m_lblCount->setStyleSheet("color:#64748b;font-size:10pt;background:rgba(100,116,139,0.1);border-radius:10px;padding:3px 10px;");
    hdr->addWidget(lbl); hdr->addStretch(); hdr->addWidget(m_lblCount);
    m_tableStagiaires=new QTableWidget(); m_tableStagiaires->setColumnCount(7);
    m_tableStagiaires->setHorizontalHeaderLabels({"ID","Nom","Prenom","Email","Cours","Heures","Statut"});
    m_tableStagiaires->setSelectionBehavior(QAbstractItemView::SelectRows); m_tableStagiaires->setSelectionMode(QAbstractItemView::SingleSelection);
    m_tableStagiaires->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_tableStagiaires->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_tableStagiaires->horizontalHeader()->setSectionResizeMode(0,QHeaderView::ResizeToContents);
    m_tableStagiaires->verticalHeader()->setVisible(false); m_tableStagiaires->setShowGrid(false);
    m_tableStagiaires->setAlternatingRowColors(true); m_tableStagiaires->setMinimumHeight(320);
    connect(m_tableStagiaires,&QTableWidget::itemSelectionChanged,this,&StagiaireWidget::onSelectionChanged);
    QHBoxLayout* bar=new QHBoxLayout(); bar->setSpacing(10);
    m_btnAjouter=new QPushButton("+ Nouveau Stagiaire"); m_btnModifier=new QPushButton("Modifier"); m_btnSupprimer=new QPushButton("Supprimer");
    m_btnAjouter->setObjectName("btnAjouter"); m_btnModifier->setObjectName("btnModifier"); m_btnSupprimer->setObjectName("btnSupprimer");
    m_btnModifier->setEnabled(false); m_btnSupprimer->setEnabled(false);
    bar->addWidget(m_btnAjouter); bar->addStretch(); bar->addWidget(m_btnModifier); bar->addWidget(m_btnSupprimer);
    connect(m_btnAjouter,&QPushButton::clicked,this,&StagiaireWidget::ouvrirFormulaireAjout);
    connect(m_btnModifier,&QPushButton::clicked,this,&StagiaireWidget::ouvrirFormulaireModification);
    connect(m_btnSupprimer,&QPushButton::clicked,this,&StagiaireWidget::supprimerStagiaire);
    auto *tools=ModuleTools::createMultiCriteriaTools(m_tableStagiaires,{1,4,6},{"nom","cours","statut"});
    m_chart=new QChartView(); m_chart->setMinimumHeight(210);
    QPushButton *pdf=new QPushButton("Exporter PDF"); pdf->setObjectName("btnVider"); bar->insertWidget(1,pdf);
    connect(pdf,&QPushButton::clicked,this,[this]{ ModuleTools::exportTableToPdf(m_tableStagiaires,"Rapport des stagiaires"); });
    cl->addLayout(hdr); cl->addWidget(tools); cl->addWidget(m_tableStagiaires);
    cl->addWidget(ModuleTools::createPaginationControls(m_tableStagiaires));
    cl->addWidget(m_chart); cl->addLayout(bar); lay->addWidget(card); return page;
}
QWidget* StagiaireWidget::creerPageFormulaire(){
    QWidget* page=new QWidget(); QVBoxLayout* lay=new QVBoxLayout(page); lay->setContentsMargins(0,0,0,0);
    QFrame* card=makeCardS(); QVBoxLayout* cl=new QVBoxLayout(card); cl->setContentsMargins(32,28,32,28); cl->setSpacing(20);
    QHBoxLayout* bc=new QHBoxLayout();
    QPushButton* back=new QPushButton("<- Retour a la liste"); back->setObjectName("btnVider"); back->setFixedWidth(180);
    m_lblFormTitre=new QLabel("Nouveau Stagiaire"); m_lblFormTitre->setStyleSheet("font-size:14pt;font-weight:bold;color:#0f172a;");
    bc->addWidget(back); bc->addSpacing(16); bc->addWidget(m_lblFormTitre); bc->addStretch();
    connect(back,&QPushButton::clicked,this,&StagiaireWidget::retourListe);
    QFrame* sep=new QFrame(); sep->setFrameShape(QFrame::HLine); sep->setStyleSheet("color:#e2e8f0;");
    QScrollArea* scroll=new QScrollArea(); scroll->setWidgetResizable(true); scroll->setFrameShape(QFrame::NoFrame); scroll->setStyleSheet("background:transparent;");
    QWidget* fw=new QWidget(); fw->setStyleSheet("background:transparent;");
    QFormLayout* form=new QFormLayout(fw); form->setSpacing(14); form->setLabelAlignment(Qt::AlignRight); form->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);
    m_editNom=new QLineEdit(); m_editNom->setPlaceholderText("Nom du stagiaire");
    m_editPrenom=new QLineEdit(); m_editPrenom->setPlaceholderText("Prenom");
    m_editEmail=new QLineEdit(); m_editEmail->setPlaceholderText("stagiaire@centre.tn");
    m_editPassword=new QLineEdit(); m_editPassword->setEchoMode(QLineEdit::Password); m_editPassword->setPlaceholderText("Mot de passe");
    m_comboFormateur=new QComboBox(); m_comboCours=new QComboBox(); m_comboSalle=new QComboBox(); rafraichirCombos();
    m_dateDebut=new QDateEdit(QDate::currentDate()); m_dateDebut->setCalendarPopup(true);
    m_dateFin=new QDateEdit(QDate::currentDate().addMonths(3)); m_dateFin->setCalendarPopup(true);
    m_spinHeures=new QDoubleSpinBox(); m_spinHeures->setRange(0,5000); m_spinHeures->setValue(0); m_spinHeures->setSuffix("  h");
    m_comboStatut=new QComboBox(); m_comboStatut->addItems({"ACTIF","SUSPENDU","DIPLOME","ABANDON"});
    form->addRow("Nom *:",m_editNom); form->addRow("Prenom *:",m_editPrenom); form->addRow("Email *:",m_editEmail);
    form->addRow("Mot de passe:",m_editPassword); form->addRow("Formateur tuteur:",m_comboFormateur);
    form->addRow("Cours inscrit:",m_comboCours); form->addRow("Salle attitree:",m_comboSalle);
    form->addRow("Date debut:",m_dateDebut); form->addRow("Date fin prevue:",m_dateFin);
    form->addRow("Heures validees:",m_spinHeures); form->addRow("Statut:",m_comboStatut);
    scroll->setWidget(fw);
    QHBoxLayout* btnRow=new QHBoxLayout(); btnRow->setSpacing(10);
    m_btnEnregistrer=new QPushButton("Enregistrer"); m_btnAnnuler=new QPushButton("Annuler");
    m_btnEnregistrer->setObjectName("btnAjouter"); m_btnAnnuler->setObjectName("btnVider");
    m_btnEnregistrer->setMinimumHeight(40); m_btnAnnuler->setMinimumHeight(40);
    btnRow->addStretch(); btnRow->addWidget(m_btnAnnuler); btnRow->addWidget(m_btnEnregistrer);
    connect(m_btnEnregistrer,&QPushButton::clicked,this,&StagiaireWidget::enregistrer);
    connect(m_btnAnnuler,&QPushButton::clicked,this,&StagiaireWidget::retourListe);
    cl->addLayout(bc); cl->addWidget(sep); cl->addWidget(scroll,1); cl->addLayout(btnRow);
    lay->addWidget(card); return page;
}
void StagiaireWidget::rafraichirCombos(){
    m_comboFormateur->clear(); m_comboFormateur->addItem("-- Aucun / Non assigne --",-1);
    for(const auto& f:Formateur::listerTout()) m_comboFormateur->addItem(f.prenom()+" "+f.nom(),f.id());
    m_comboCours->clear(); m_comboCours->addItem("-- Aucun / Non assigne --",-1);
    for(const auto& c:Cours::listerTout()) m_comboCours->addItem(c.titre(),c.id());
    m_comboSalle->clear(); m_comboSalle->addItem("-- Aucune salle --",-1);
    for(const auto& s:Salle::listerTout()) m_comboSalle->addItem(QString("%1 (%2 places)").arg(s.nom()).arg(s.capacite()),s.id());
}
void StagiaireWidget::rafraichirTable(){
    rafraichirCombos();
    QList<Stagiaire> list=Stagiaire::listerTout(); m_tableStagiaires->setRowCount(0);
    for(int i=0;i<list.size();++i){
        const Stagiaire& s=list[i]; m_tableStagiaires->insertRow(i);
        QString coursTitre="Non inscrit"; if(s.idCours()>0){Cours c=Cours::trouverParId(s.idCours()); if(c.id()>0) coursTitre=c.titre();}
        m_tableStagiaires->setItem(i,0,new QTableWidgetItem(QString::number(s.id())));
        m_tableStagiaires->setItem(i,1,new QTableWidgetItem(s.nom()));
        m_tableStagiaires->setItem(i,2,new QTableWidgetItem(s.prenom()));
        m_tableStagiaires->setItem(i,3,new QTableWidgetItem(s.email()));
        m_tableStagiaires->setItem(i,4,new QTableWidgetItem(coursTitre));
        m_tableStagiaires->setItem(i,5,new QTableWidgetItem(QString::number(s.heuresValidees(),'f',1)+" h"));
        QString st=s.statut(); QTableWidgetItem* si=new QTableWidgetItem(st);
        if(st=="ACTIF") si->setForeground(QColor("#16a34a")); else if(st=="SUSPENDU") si->setForeground(QColor("#d97706"));
        else if(st=="DIPLOME") si->setForeground(QColor("#0d9488")); else si->setForeground(QColor("#dc2626"));
        m_tableStagiaires->setItem(i,6,si);
    }
    m_lblCount->setText(QString::number(list.size())+" stagiaire"+(list.size()>1?"s":""));
    ModuleTools::updateCategoryChart(m_chart,m_tableStagiaires,6,"Répartition des stagiaires par statut");
    ModuleTools::refreshPagination(m_tableStagiaires);
    mettreAJourBoutonsListe();
}
void StagiaireWidget::afficherListe(){ retourListe(); rafraichirTable(); }
void StagiaireWidget::afficherFormulaireAjout(){ ouvrirFormulaireAjout(); }
void StagiaireWidget::onSelectionChanged(){
    QList<QTableWidgetItem*> sel=m_tableStagiaires->selectedItems();
    m_idStagiaireSelectionne=!sel.isEmpty()?m_tableStagiaires->item(m_tableStagiaires->row(sel.first()),0)->text().toInt():-1;
    mettreAJourBoutonsListe();
}
void StagiaireWidget::mettreAJourBoutonsListe(){ bool s=m_idStagiaireSelectionne!=-1; m_btnModifier->setEnabled(s); m_btnSupprimer->setEnabled(s); }
void StagiaireWidget::ouvrirFormulaireAjout(){
    m_modeAjout=true; m_idStagiaireSelectionne=-1; m_lblFormTitre->setText("Nouveau Stagiaire");
    m_editNom->clear(); m_editPrenom->clear(); m_editEmail->clear(); m_editPassword->clear();
    m_editPassword->setPlaceholderText("Mot de passe (requis a la creation)");
    m_comboFormateur->setCurrentIndex(0); m_comboCours->setCurrentIndex(0); m_comboSalle->setCurrentIndex(0);
    m_dateDebut->setDate(QDate::currentDate()); m_dateFin->setDate(QDate::currentDate().addMonths(3));
    m_spinHeures->setValue(0); m_comboStatut->setCurrentIndex(0);
    for(auto* w:{m_editNom,m_editPrenom,m_editEmail,m_editPassword}) w->setStyleSheet("");
    m_stack->setCurrentIndex(1);
}
void StagiaireWidget::ouvrirFormulaireModification(){ if(m_idStagiaireSelectionne==-1) return; m_modeAjout=false; remplirFormulaire(m_idStagiaireSelectionne); m_stack->setCurrentIndex(1); }
void StagiaireWidget::remplirFormulaire(int id){
    Stagiaire s=Stagiaire::trouverParId(id);
    m_lblFormTitre->setText(QString("Modifier : %1 %2").arg(s.prenom(),s.nom()));
    m_editNom->setText(s.nom()); m_editPrenom->setText(s.prenom()); m_editEmail->setText(s.email());
    m_editPassword->clear(); m_editPassword->setPlaceholderText("Laisser vide pour conserver le mot de passe");
    int iF=m_comboFormateur->findData(s.idFormateur()); m_comboFormateur->setCurrentIndex(iF!=-1?iF:0);
    int iC=m_comboCours->findData(s.idCours()); m_comboCours->setCurrentIndex(iC!=-1?iC:0);
    int iS=m_comboSalle->findData(s.idSalleAttitree()); m_comboSalle->setCurrentIndex(iS!=-1?iS:0);
    if(s.dateDebut().isValid()) m_dateDebut->setDate(s.dateDebut());
    if(s.dateFinPrevue().isValid()) m_dateFin->setDate(s.dateFinPrevue());
    m_spinHeures->setValue(s.heuresValidees()); m_comboStatut->setCurrentText(s.statut());
    for(auto* w:{m_editNom,m_editPrenom,m_editEmail,m_editPassword}) w->setStyleSheet("");
}
void StagiaireWidget::enregistrer(){
    bool valid=true;
    auto mark=[](QLineEdit* w,bool bad){ w->setStyleSheet(bad?"border:1.5px solid #f43f5e;border-radius:8px;":""); };
    QString nom=m_editNom->text().trimmed(), prenom=m_editPrenom->text().trimmed(), email=m_editEmail->text().trimmed(), pwd=m_editPassword->text();
    mark(m_editNom,nom.isEmpty()); if(nom.isEmpty()) valid=false;
    mark(m_editPrenom,prenom.isEmpty()); if(prenom.isEmpty()) valid=false;
    const bool invalidEmail = email.isEmpty() || !ModuleTools::isValidEmail(email);
    mark(m_editEmail,invalidEmail); if(invalidEmail) valid=false;
    if (invalidEmail) m_editEmail->setPlaceholderText("Adresse email invalide");
    if (m_dateDebut->date() >= m_dateFin->date()) {
        QMessageBox::warning(this, "Validation", "La date de début doit être antérieure à la date de fin.");
        valid = false;
    }
    if(m_modeAjout&&pwd.isEmpty()){ m_editPassword->setStyleSheet("border:1.5px solid #f43f5e;border-radius:8px;"); m_editPassword->setPlaceholderText("Mot de passe requis"); valid=false; }
    else m_editPassword->setStyleSheet("");
    if(!valid) return;

    // Double-réservation prevention check
    int salleId = m_comboSalle->currentData().toInt();
    if (salleId > 0) {
        QSqlQuery qOverlap(DB::instance().database());
        qOverlap.prepare(
            "SELECT s.NOM, s.PRENOM, c.TITRE, s.DATE_DEBUT, s.DATE_FIN_PREVUE "
            "FROM STAGIAIRE s "
            "LEFT JOIN COURS c ON s.ID_COURS = c.ID_COURS "
            "WHERE s.ID_SALLE_ATTITREE = :salle "
            "  AND s.ID_STAGIAIRE != :myId "
            "  AND s.STATUT = 'ACTIF' "
            "  AND (:dStart <= s.DATE_FIN_PREVUE AND :dEnd >= s.DATE_DEBUT) "
            "  AND ROWNUM = 1"
        );
        qOverlap.bindValue(":salle", salleId);
        qOverlap.bindValue(":myId", m_modeAjout ? -1 : m_idStagiaireSelectionne);
        qOverlap.bindValue(":dStart", m_dateDebut->date());
        qOverlap.bindValue(":dEnd", m_dateFin->date());

        if (qOverlap.exec() && qOverlap.next()) {
            auto rep = QMessageBox::question(
                this, "Avertissement Double-réservation Salle",
                QString("Attention : Cette salle est déjà attribuée à <b>%1 %2</b> pour le cours <i>%3</i> "
                        "sur la période chevauchante du %4 au %5.<br><br>Voulez-vous tout de même affecter cette salle ?")
                .arg(qOverlap.value(1).toString(), qOverlap.value(0).toString(), qOverlap.value(2).toString())
                .arg(qOverlap.value(3).toDate().toString("dd/MM/yyyy"), qOverlap.value(4).toDate().toString("dd/MM/yyyy")),
                QMessageBox::Yes | QMessageBox::No, QMessageBox::No
            );
            if (rep == QMessageBox::No) {
                return;
            }
        }
    }
    if(m_modeAjout){
        Stagiaire s; s.setNom(nom); s.setPrenom(prenom); s.setEmail(email); s.setPasswordHash(Authentification::hashPassword(pwd));
        s.setIdFormateur(m_comboFormateur->currentData().toInt()); s.setIdCours(m_comboCours->currentData().toInt()); s.setIdSalleAttitree(m_comboSalle->currentData().toInt());
        s.setDateDebut(m_dateDebut->date()); s.setDateFinPrevue(m_dateFin->date()); s.setHeuresValidees(m_spinHeures->value()); s.setStatut(m_comboStatut->currentText());
        if(!s.ajouter()){QMessageBox::critical(this,"Erreur","Echec de l'enregistrement du stagiaire."); return;}
    } else {
        Stagiaire s=Stagiaire::trouverParId(m_idStagiaireSelectionne);
        s.setNom(nom); s.setPrenom(prenom); s.setEmail(email);
        if(!pwd.isEmpty()) s.setPasswordHash(Authentification::hashPassword(pwd));
        s.setIdFormateur(m_comboFormateur->currentData().toInt()); s.setIdCours(m_comboCours->currentData().toInt()); s.setIdSalleAttitree(m_comboSalle->currentData().toInt());
        s.setDateDebut(m_dateDebut->date()); s.setDateFinPrevue(m_dateFin->date()); s.setHeuresValidees(m_spinHeures->value()); s.setStatut(m_comboStatut->currentText());
        if(!s.modifier()){QMessageBox::critical(this,"Erreur","Echec de la modification."); return;}
    }
    rafraichirTable(); retourListe();
}
void StagiaireWidget::supprimerStagiaire(){
    if(m_idStagiaireSelectionne==-1) return;
    QList<QTableWidgetItem*> sel=m_tableStagiaires->selectedItems();
    QString nom; if(!sel.isEmpty()){ int r=m_tableStagiaires->row(sel.first()); nom=m_tableStagiaires->item(r,2)->text()+" "+m_tableStagiaires->item(r,1)->text(); }
    auto rep=QMessageBox::question(this,"Confirmer la suppression",QString("Supprimer le stagiaire <b>%1</b> ?").arg(nom),QMessageBox::Yes|QMessageBox::No,QMessageBox::No);
    if(rep==QMessageBox::Yes){ Stagiaire s=Stagiaire::trouverParId(m_idStagiaireSelectionne); if(s.supprimer()){m_idStagiaireSelectionne=-1; rafraichirTable();} else QMessageBox::critical(this,"Erreur","Impossible de supprimer."); }
}
void StagiaireWidget::retourListe(){ m_idStagiaireSelectionne=-1; m_tableStagiaires->clearSelection(); mettreAJourBoutonsListe(); m_stack->setCurrentIndex(0); }
