#include "formateurwidget.h"
#include "formateur.h"
#include "authentification.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QFrame>
#include <QMessageBox>
#include <QHeaderView>
#include <QGraphicsDropShadowEffect>

static QFrame* makeCardF(){
    QFrame* c=new QFrame(); c->setObjectName("glassCard");
    QGraphicsDropShadowEffect* sh=new QGraphicsDropShadowEffect(c);
    sh->setBlurRadius(25); sh->setColor(QColor(15,23,42,35)); sh->setOffset(0,8); c->setGraphicsEffect(sh);
    return c;
}

FormateurWidget::FormateurWidget(QWidget *parent):QWidget(parent){
    setAttribute(Qt::WA_StyledBackground,true); setupUi(); rafraichirTable();
}
void FormateurWidget::setupUi(){
    QVBoxLayout* root=new QVBoxLayout(this); root->setContentsMargins(10,10,10,10);
    m_stack=new QStackedWidget(this);
    m_stack->addWidget(creerPageListe()); m_stack->addWidget(creerPageFormulaire());
    root->addWidget(m_stack);
}
QWidget* FormateurWidget::creerPageListe(){
    QWidget* page=new QWidget(); QVBoxLayout* lay=new QVBoxLayout(page); lay->setContentsMargins(0,0,0,0);
    QFrame* card=makeCardF(); QVBoxLayout* cl=new QVBoxLayout(card); cl->setContentsMargins(24,20,24,20); cl->setSpacing(14);
    QHBoxLayout* hdr=new QHBoxLayout();
    QLabel* lbl=new QLabel("<b>Formateurs et Utilisateurs</b>"); lbl->setStyleSheet("font-size:15pt;color:#0f172a;");
    m_lblCount=new QLabel("0 formateurs"); m_lblCount->setStyleSheet("color:#64748b;font-size:10pt;background:rgba(100,116,139,0.1);border-radius:10px;padding:3px 10px;");
    hdr->addWidget(lbl); hdr->addStretch(); hdr->addWidget(m_lblCount);
    m_tableFormateurs=new QTableWidget(); m_tableFormateurs->setColumnCount(6);
    m_tableFormateurs->setHorizontalHeaderLabels({"ID","Nom","Prenom","Email","Role","Statut"});
    m_tableFormateurs->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tableFormateurs->setSelectionMode(QAbstractItemView::SingleSelection);
    m_tableFormateurs->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_tableFormateurs->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_tableFormateurs->horizontalHeader()->setSectionResizeMode(0,QHeaderView::ResizeToContents);
    m_tableFormateurs->verticalHeader()->setVisible(false); m_tableFormateurs->setShowGrid(false);
    m_tableFormateurs->setAlternatingRowColors(true); m_tableFormateurs->setMinimumHeight(320);
    connect(m_tableFormateurs,&QTableWidget::itemSelectionChanged,this,&FormateurWidget::onSelectionChanged);
    QHBoxLayout* bar=new QHBoxLayout(); bar->setSpacing(10);
    m_btnAjouter=new QPushButton("+ Nouveau Formateur"); m_btnModifier=new QPushButton("Modifier"); m_btnSupprimer=new QPushButton("Supprimer");
    m_btnAjouter->setObjectName("btnAjouter"); m_btnModifier->setObjectName("btnModifier"); m_btnSupprimer->setObjectName("btnSupprimer");
    m_btnModifier->setEnabled(false); m_btnSupprimer->setEnabled(false);
    bar->addWidget(m_btnAjouter); bar->addStretch(); bar->addWidget(m_btnModifier); bar->addWidget(m_btnSupprimer);
    connect(m_btnAjouter,&QPushButton::clicked,this,&FormateurWidget::ouvrirFormulaireAjout);
    connect(m_btnModifier,&QPushButton::clicked,this,&FormateurWidget::ouvrirFormulaireModification);
    connect(m_btnSupprimer,&QPushButton::clicked,this,&FormateurWidget::supprimerFormateur);
    cl->addLayout(hdr); cl->addWidget(m_tableFormateurs); cl->addLayout(bar); lay->addWidget(card); return page;
}
QWidget* FormateurWidget::creerPageFormulaire(){
    QWidget* page=new QWidget(); QVBoxLayout* lay=new QVBoxLayout(page); lay->setContentsMargins(0,0,0,0);
    QFrame* card=makeCardF(); QVBoxLayout* cl=new QVBoxLayout(card); cl->setContentsMargins(32,28,32,28); cl->setSpacing(20);
    QHBoxLayout* bc=new QHBoxLayout();
    QPushButton* back=new QPushButton("<- Retour a la liste"); back->setObjectName("btnVider"); back->setFixedWidth(180);
    m_lblFormTitre=new QLabel("Nouveau Formateur"); m_lblFormTitre->setStyleSheet("font-size:14pt;font-weight:bold;color:#0f172a;");
    bc->addWidget(back); bc->addSpacing(16); bc->addWidget(m_lblFormTitre); bc->addStretch();
    connect(back,&QPushButton::clicked,this,&FormateurWidget::retourListe);
    QFrame* sep=new QFrame(); sep->setFrameShape(QFrame::HLine); sep->setStyleSheet("color:#e2e8f0;");
    QFormLayout* form=new QFormLayout(); form->setSpacing(14); form->setLabelAlignment(Qt::AlignRight); form->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);
    m_editNom=new QLineEdit(); m_editNom->setPlaceholderText("Nom de famille");
    m_editPrenom=new QLineEdit(); m_editPrenom->setPlaceholderText("Prenom");
    m_editEmail=new QLineEdit(); m_editEmail->setPlaceholderText("exemple@centre.tn");
    m_editPassword=new QLineEdit(); m_editPassword->setEchoMode(QLineEdit::Password); m_editPassword->setPlaceholderText("Mot de passe");
    m_comboRole=new QComboBox(); m_comboRole->addItems({"FORMATEUR","ADMIN","SUPER_ADMIN"});
    m_comboStatut=new QComboBox(); m_comboStatut->addItems({"ACTIF","INACTIF"});
    form->addRow("Nom *:",m_editNom); form->addRow("Prenom *:",m_editPrenom); form->addRow("Email *:",m_editEmail);
    form->addRow("Mot de passe:",m_editPassword); form->addRow("Role:",m_comboRole); form->addRow("Statut:",m_comboStatut);
    QHBoxLayout* btnRow=new QHBoxLayout(); btnRow->setSpacing(10);
    m_btnEnregistrer=new QPushButton("Enregistrer"); m_btnAnnuler=new QPushButton("Annuler");
    m_btnEnregistrer->setObjectName("btnAjouter"); m_btnAnnuler->setObjectName("btnVider");
    m_btnEnregistrer->setMinimumHeight(40); m_btnAnnuler->setMinimumHeight(40);
    btnRow->addStretch(); btnRow->addWidget(m_btnAnnuler); btnRow->addWidget(m_btnEnregistrer);
    connect(m_btnEnregistrer,&QPushButton::clicked,this,&FormateurWidget::enregistrer);
    connect(m_btnAnnuler,&QPushButton::clicked,this,&FormateurWidget::retourListe);
    cl->addLayout(bc); cl->addWidget(sep); cl->addLayout(form); cl->addStretch(); cl->addLayout(btnRow);
    lay->addWidget(card); return page;
}
void FormateurWidget::rafraichirTable(){
    QList<Formateur> list=Formateur::listerTout(); m_tableFormateurs->setRowCount(0);
    for(int i=0;i<list.size();++i){
        const Formateur& f=list[i]; m_tableFormateurs->insertRow(i);
        m_tableFormateurs->setItem(i,0,new QTableWidgetItem(QString::number(f.id())));
        m_tableFormateurs->setItem(i,1,new QTableWidgetItem(f.nom()));
        m_tableFormateurs->setItem(i,2,new QTableWidgetItem(f.prenom()));
        m_tableFormateurs->setItem(i,3,new QTableWidgetItem(f.email()));
        QTableWidgetItem* ri=new QTableWidgetItem(f.role());
        if(f.role()=="SUPER_ADMIN") ri->setForeground(QColor("#7c3aed"));
        else if(f.role()=="ADMIN") ri->setForeground(QColor("#0d9488"));
        m_tableFormateurs->setItem(i,4,ri);
        QTableWidgetItem* si=new QTableWidgetItem(f.statutCompte());
        si->setForeground(f.statutCompte()=="ACTIF"?QColor("#16a34a"):QColor("#dc2626"));
        m_tableFormateurs->setItem(i,5,si);
    }
    m_lblCount->setText(QString::number(list.size())+" formateur"+(list.size()>1?"s":""));
    mettreAJourBoutonsListe();
}
void FormateurWidget::afficherListe(){ retourListe(); rafraichirTable(); }
void FormateurWidget::afficherFormulaireAjout(){ ouvrirFormulaireAjout(); }
void FormateurWidget::onSelectionChanged(){
    QList<QTableWidgetItem*> sel=m_tableFormateurs->selectedItems();
    m_idFormateurSelectionne=!sel.isEmpty()?m_tableFormateurs->item(m_tableFormateurs->row(sel.first()),0)->text().toInt():-1;
    mettreAJourBoutonsListe();
}
void FormateurWidget::mettreAJourBoutonsListe(){ bool s=m_idFormateurSelectionne!=-1; m_btnModifier->setEnabled(s); m_btnSupprimer->setEnabled(s); }
void FormateurWidget::ouvrirFormulaireAjout(){
    m_modeAjout=true; m_idFormateurSelectionne=-1; m_lblFormTitre->setText("Nouveau Formateur");
    m_editNom->clear(); m_editPrenom->clear(); m_editEmail->clear(); m_editPassword->clear();
    m_editPassword->setPlaceholderText("Mot de passe (requis a la creation)");
    m_comboRole->setCurrentIndex(0); m_comboStatut->setCurrentIndex(0);
    for(auto* w:{m_editNom,m_editPrenom,m_editEmail,m_editPassword}) w->setStyleSheet("");
    m_stack->setCurrentIndex(1);
}
void FormateurWidget::ouvrirFormulaireModification(){
    if(m_idFormateurSelectionne==-1) return;
    m_modeAjout=false; remplirFormulaire(m_idFormateurSelectionne); m_stack->setCurrentIndex(1);
}
void FormateurWidget::remplirFormulaire(int id){
    Formateur f=Formateur::trouverParId(id);
    m_lblFormTitre->setText(QString("Modifier : %1 %2").arg(f.prenom(),f.nom()));
    m_editNom->setText(f.nom()); m_editPrenom->setText(f.prenom()); m_editEmail->setText(f.email());
    m_editPassword->clear(); m_editPassword->setPlaceholderText("Laisser vide pour conserver le mot de passe");
    m_comboRole->setCurrentText(f.role()); m_comboStatut->setCurrentText(f.statutCompte());
    for(auto* w:{m_editNom,m_editPrenom,m_editEmail,m_editPassword}) w->setStyleSheet("");
}
void FormateurWidget::enregistrer(){
    bool valid=true;
    auto mark=[](QLineEdit* w,bool bad){ w->setStyleSheet(bad?"border:1.5px solid #f43f5e;border-radius:8px;":""); };
    QString nom=m_editNom->text().trimmed(), prenom=m_editPrenom->text().trimmed(), email=m_editEmail->text().trimmed(), pwd=m_editPassword->text();
    mark(m_editNom,nom.isEmpty()); if(nom.isEmpty()) valid=false;
    mark(m_editPrenom,prenom.isEmpty()); if(prenom.isEmpty()) valid=false;
    mark(m_editEmail,email.isEmpty()); if(email.isEmpty()) valid=false;
    if(m_modeAjout&&pwd.isEmpty()){ m_editPassword->setStyleSheet("border:1.5px solid #f43f5e;border-radius:8px;"); m_editPassword->setPlaceholderText("Mot de passe requis"); valid=false; }
    else m_editPassword->setStyleSheet("");
    if(!valid) return;
    if(m_modeAjout){
        Formateur f; f.setNom(nom); f.setPrenom(prenom); f.setEmail(email);
        f.setPasswordHash(Authentification::hashPassword(pwd)); f.setRole(m_comboRole->currentText()); f.setStatutCompte(m_comboStatut->currentText());
        if(!f.ajouter()){QMessageBox::critical(this,"Erreur","Echec de l'enregistrement."); return;}
    } else {
        Formateur f=Formateur::trouverParId(m_idFormateurSelectionne);
        f.setNom(nom); f.setPrenom(prenom); f.setEmail(email); f.setRole(m_comboRole->currentText()); f.setStatutCompte(m_comboStatut->currentText());
        f.setPasswordHash(!pwd.isEmpty()?Authentification::hashPassword(pwd):"");
        if(!f.modifier()){QMessageBox::critical(this,"Erreur","Echec de la modification."); return;}
    }
    rafraichirTable(); retourListe();
}
void FormateurWidget::supprimerFormateur(){
    if(m_idFormateurSelectionne==-1) return;
    QList<QTableWidgetItem*> sel=m_tableFormateurs->selectedItems();
    QString nom; if(!sel.isEmpty()){ int r=m_tableFormateurs->row(sel.first()); nom=m_tableFormateurs->item(r,2)->text()+" "+m_tableFormateurs->item(r,1)->text(); }
    auto rep=QMessageBox::question(this,"Confirmer la suppression",QString("Supprimer le formateur <b>%1</b> ?").arg(nom),QMessageBox::Yes|QMessageBox::No,QMessageBox::No);
    if(rep==QMessageBox::Yes){ Formateur f=Formateur::trouverParId(m_idFormateurSelectionne); if(f.supprimer()){m_idFormateurSelectionne=-1; rafraichirTable();} else QMessageBox::critical(this,"Erreur","Impossible de supprimer."); }
}
void FormateurWidget::retourListe(){ m_idFormateurSelectionne=-1; m_tableFormateurs->clearSelection(); mettreAJourBoutonsListe(); m_stack->setCurrentIndex(0); }
