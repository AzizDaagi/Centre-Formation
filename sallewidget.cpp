#include "sallewidget.h"
#include "salle.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QFrame>
#include <QMessageBox>
#include <QHeaderView>
#include <QGraphicsDropShadowEffect>

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

    cl->addLayout(hdr); cl->addWidget(m_tableSalles); cl->addLayout(bar);
    lay->addWidget(card);
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
    mettreAJourBoutonsListe();
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
void SalleWidget::retourListe(){
    m_idSalleSelectionnee=-1;
    m_tableSalles->clearSelection(); mettreAJourBoutonsListe(); m_stack->setCurrentIndex(0);
}
