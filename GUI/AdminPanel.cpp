#include "AdminPanel.h"
#include "ui_AdminPanel.h"
#include "AdminAddHarbour.h"
#include "AdminEditHarbour.h"
#include "AdminAddClient.h"
#include "AdminEditClient.h"
#include "AdminAddManager.h"
#include "AdminEditManager.h"
#include "AuxClass/SQLConnect.h"
#include <QString>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QDebug>
#include "AdminAnchorageDetails.h"
#include "AdminCorridorDetails.h"
#include "AdminDockDetails.h"


AdminPanel::AdminPanel(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AdminPanel)
{
    ui->setupUi(this);
    populateComboBox();
    this->chosenPort = "temp";
}

AdminPanel::~AdminPanel()
{
    delete ui;
}

void AdminPanel::receiveChosenPort(QString newPortName){
    chosenPort = newPortName;
}

void AdminPanel::receiveAnchorageDetails(Anchorage anchorageDetails){
    tempPort.anchorage = anchorageDetails;
    updatePortShowLabels();
}

void AdminPanel::receiveCorridorDetails(TransportCorridor corridorDetails, int numberOfCorridos){
    tempPort.transportCorridor = corridorDetails;
    tempPort.numberOfCorridors = numberOfCorridos;
    updatePortShowLabels();
}

void AdminPanel::receiveDeleteSignal(){
    tempPort.name = "temp";
}

void AdminPanel::receiveDockDetails(Dock dockDetails, int numberOfDocks){
    tempPort.dock = dockDetails;
    tempPort.numberOfDocks = numberOfDocks;
    updatePortShowLabels();
}


void AdminPanel::populateComboBox(){
    SQLConnect::ConnectToDB();
    QSqlQueryModel *model = new QSqlQueryModel();
    model->setQuery("SELECT Name FROM SafeHarbour.Port ORDER BY Name ASC;");
    ui->comboBox->setModel(model);
    SQLConnect::DisconnectDB();
}


void AdminPanel::on_pushCreateNewHarbour_clicked()
{
    AdminAddHarbour adminAddHarbour;
    adminAddHarbour.setModal(true);
    adminAddHarbour.exec();
    populateComboBox();
}

void AdminPanel::on_pushEditHarbour_clicked()
{
    if(tempPort.name != "temp"){
        AdminEditHarbour adminEditHarbour(this);
        adminEditHarbour.setModal(true);
        connect(this,SIGNAL(sendDataToEdit(Port)),&adminEditHarbour,SLOT(receiveDataToEdit(Port)));
        emit sendDataToEdit(tempPort);
        adminEditHarbour.exec();
        if(tempPort.name == "temp"){
            updatePortAfterDeletion();
        }else{
            ui->labelShowChoosenPort->setText(this->chosenPort);
            populatePortInformation(chosenPort);
        }
    }
}

void AdminPanel::on_pushEditAnchorage_clicked()
{
    if(tempPort.name != "temp"){
        AdminAnchorageDetails adminAnchorageDetails(this);
        adminAnchorageDetails.setModal(false);
        connect(this,SIGNAL(sendAnchorageData(Anchorage)),&adminAnchorageDetails,SLOT(receiveAnchorageData(Anchorage)));
        emit sendAnchorageData(tempPort.anchorage);
        adminAnchorageDetails.exec();
    }
}

void AdminPanel::on_pushEditCorridor_clicked()
{
    if(tempPort.name != "temp"){
        AdminCorridorDetails adminCorridorDetails(this);
        adminCorridorDetails.setModal(false);
        connect(this,SIGNAL(sendCorridorData(TransportCorridor,int)),&adminCorridorDetails,SLOT(receiveCorridorData(TransportCorridor, int)));
        emit sendCorridorData(tempPort.transportCorridor, tempPort.numberOfCorridors);
        adminCorridorDetails.exec();
    }
}

void AdminPanel::on_pushEditDock_clicked()
{
    if(tempPort.name != "temp"){
        AdminDockDetails adminDockDetails(this);
        adminDockDetails.setModal(false);
        connect(this, SIGNAL(sendDockData(Dock,int)),&adminDockDetails,SLOT(receiveDockData(Dock,int)));
        emit sendDockData(tempPort.dock, tempPort.numberOfDocks);
        adminDockDetails.exec();
    }
}

void AdminPanel::on_pushClientAdd_clicked()
{
    AdminAddClient adminAddClient;
    adminAddClient.setModal(true);
    adminAddClient.exec();
}

void AdminPanel::on_pushClientEdit_clicked()
{
    AdminEditClient adminEditClient;
    adminEditClient.setModal(true);
    adminEditClient.exec();
}

void AdminPanel::on_pushManagerAdd_clicked()
{
    AdminAddManager adminAddManager;
    adminAddManager.setModal(true);
    adminAddManager.exec();
}

void AdminPanel::on_pushManagerEdit_clicked()
{
    AdminEditManager adminEditManager;
    adminEditManager.setModal(true);
    adminEditManager.exec();
}

void AdminPanel::on_pushLogOut_clicked()
{
    this->close();
}


void AdminPanel::on_pushAcceptPort_clicked()
{
    //checkIfPortChosen(chosenPort);
    chosenPort = ui->comboBox->currentText();
    if(chosenPort != ""){
        ui->labelShowChoosenPort->setText(this->chosenPort);
        populatePortInformation(chosenPort);
    }
}

void AdminPanel::on_pushPlusAnchorage_clicked()
{
    /*
     * ToDo: Upgrade this if statement
     * */
    if(chosenPort != "temp"){
        tempPort.anchorage.capacity++;
        ui->labelShowAnchorageCopacity->setNum(tempPort.anchorage.capacity);
    }
}

void AdminPanel::on_pushMinusAnchorage_clicked()
{
    /*
     * ToDo: Upgrade this if statement
     * */
    if(chosenPort != "temp"){
        tempPort.anchorage.capacity--;
        if(tempPort.anchorage.capacity <0){
            tempPort.anchorage.capacity = 0;
        }
        ui->labelShowAnchorageCopacity->setNum(tempPort.anchorage.capacity);
    }
}

void AdminPanel::on_pushPlusCorridor_clicked()
{
    /*
     * ToDo: Upgrade this if statement
     * */
    if(chosenPort != "temp"){
        tempPort.numberOfCorridors++;
        ui->labelShowCorridorCount->setNum(tempPort.numberOfCorridors);
    }
}

void AdminPanel::on_pushMinusCorridor_clicked()
{
    /*
     * ToDo: Upgrade this if statement
     * */
    if(chosenPort != "temp"){
        tempPort.numberOfCorridors--;
        if(tempPort.numberOfCorridors <0){
            tempPort.numberOfCorridors = 0;
        }
        ui->labelShowCorridorCount->setNum(tempPort.numberOfCorridors);
    }
}

void AdminPanel::on_pushPlusDock_clicked()
{
    /*
     * ToDo: Upgrade this if statement
     * */
    if(chosenPort != "temp"){
        tempPort.numberOfDocks++;
        ui->labelShowDockCount->setNum(tempPort.numberOfDocks);
    }
}

void AdminPanel::on_pushMinusDock_clicked()
{
    /*
     * ToDo: Upgrade this if statement
     * */
    if(chosenPort != "temp"){
        tempPort.numberOfDocks--;
        if(tempPort.numberOfDocks <0){
            tempPort.numberOfDocks = 0;
        }
        ui->labelShowDockCount->setNum(tempPort.numberOfDocks);
    }
}

void AdminPanel::on_pushPlusTugboat_clicked()
{
    /*
     * ToDo: Upgrade this if statement
     * */
    if(chosenPort != "temp"){
        tempPort.numberOfTugboats++;
        ui->labelShowTugboatCount->setNum(tempPort.numberOfTugboats);
    }
}

void AdminPanel::on_pushMinusTugboat_clicked()
{
    /*
     * ToDo: Upgrade this if statement
     * */
    if(chosenPort != "temp"){
        tempPort.numberOfTugboats--;
        if(tempPort.numberOfTugboats <0){
            tempPort.numberOfTugboats = 0;
        }
        ui->labelShowTugboatCount->setNum(tempPort.numberOfTugboats);
    }
}

void AdminPanel::checkIfPortChosen(QString chosenPort){

    if(chosenPort == "temp"){
        SQLConnect::ConnectToDB();
        QSqlQuery query;
        query.prepare("SELECT Name FROM SafeHarbour.Port ORDER BY Name ASC LIMIT 1");
        query.exec();
        query.first();
        if(query.value(0).toString() != ""){
           this->chosenPort = query.value(0).toString();
        }
        SQLConnect::DisconnectDB();
    }
}



void AdminPanel::populatePortInformation(QString nameOfChosenPort){
    SQLConnect::ConnectToDB();

    populatePortBaseInf(nameOfChosenPort);
    populatePortAnchorageInf(tempPort.pAnchorage);
    populatePortCorridorInf(tempPort.pCorridor);
    populatePortDockInf(tempPort.pDock);
    updatePortShowLabels();
    populateComboBox();
    SQLConnect::DisconnectDB();
}

void AdminPanel::populatePortBaseInf(QString nameOfChosenPort){

    QSqlQuery query;
    query.prepare("SELECT * FROM SafeHarbour.Port WHERE name = (:nameOfChosenPort)");
    query.bindValue(":nameOfChosenPort", nameOfChosenPort);
    query.exec();
    query.first();
    tempPort.name = query.value(1).toString();
    tempPort.owner = query.value(2).toString();
    tempPort.location.geoLatitude = query.value(3).toDouble();
    tempPort.location.geoLongitude = query.value(4).toDouble();
    tempPort.numberOfTugboats = query.value(5).toInt();
    tempPort.numberOfCorridors = query.value(6).toInt();
    tempPort.numberOfDocks = query.value(7).toInt();
    tempPort.pAnchorage = query.value(8).toInt();
    tempPort.pCorridor = query.value(9).toInt();
    tempPort.pDock = query.value(10).toInt();
    tempPort.warehouseCapacity = query.value(11).toInt();
}

void AdminPanel::populatePortAnchorageInf(int pAnchorage){

    QSqlQuery query;
    query.prepare("SELECT Capacity, MaxDraft, CostPerHour FROM SafeHarbour.Anchorage WHERE idAnchorage = (:pAnchorage)");
    query.bindValue(":pAnchorage", pAnchorage);
    query.exec();
    query.first();
    tempPort.anchorage.capacity = query.value(0).toInt();
    tempPort.anchorage.maxDraft = query.value(1).toDouble();
    tempPort.anchorage.costPerHour = query.value(2).toDouble();
}

void AdminPanel::populatePortCorridorInf(int pCorridor){

    QSqlQuery query;
    query.prepare("SELECT MaxSpeed, MaxWidth, MaxDraft, CostPerHour, CapPerCorridor FROM SafeHarbour.TransportCorridor WHERE idTransportCorridor = (:pCorridor)");
    query.bindValue(":pCorridor", pCorridor);
    query.exec();
    query.first();
    tempPort.transportCorridor.maxSpeed = query.value(0).toInt();
    tempPort.transportCorridor.maxWidth = query.value(1).toDouble();
    tempPort.transportCorridor.maxDraft = query.value(2).toDouble();
    tempPort.transportCorridor.costPerHour = query.value(3).toDouble();
    tempPort.transportCorridor.capacityPerCorridor = query.value(4).toInt();
}

void AdminPanel::populatePortDockInf(int pDock){

    QSqlQuery query;
    query.prepare("SELECT MaxDraft, ContainersPerHour, PassengersPerHour, CostPerHour, Capacity FROM SafeHarbour.Dock WHERE idDock = (:pDock)");
    query.bindValue(":pDock", pDock);
    query.exec();
    query.first();
    tempPort.dock.maxDraft = query.value(0).toInt();
    tempPort.dock.containerPerHour = query.value(1).toDouble();
    tempPort.dock.passengersPerHour = query.value(2).toDouble();
    tempPort.dock.costPerHour = query.value(3).toDouble();
    tempPort.dock.capacity = query.value(4).toInt();
}

void AdminPanel::updatePortShowLabels(){

    // ToDo - add Manager and Client
    ui->labelShowAnchorageCopacity->setNum(tempPort.anchorage.capacity);
    ui->labelShowTugboatCount->setNum(tempPort.numberOfTugboats);
    ui->labelShowCorridorCount->setNum(tempPort.numberOfCorridors);
    ui->labelShowDockCount->setNum(tempPort.numberOfDocks);
}

void AdminPanel::updatePortAfterDeletion(){
    ui->labelShowAnchorageCopacity->setText("");
    ui->labelShowTugboatCount->setText("");
    ui->labelShowCorridorCount->setText("");
    ui->labelShowDockCount->setText("");
    ui->labelShowChoosenPort->setText("");
}



void AdminPanel::on_pushAcceptChanges_clicked()
{
    if(tempPort.name != "temp" || tempPort.name != ""){
        SQLConnect::ConnectToDB();

        QSqlQuery queryUpdateAnchorage;
        queryUpdateAnchorage.prepare("UPDATE SafeHarbour.Anchorage SET Capacity = :Capacity, MaxDraft = :MaxDraft, CostPerHour = :CostPerHour WHERE idAnchorage = :pAnchorage;");
        queryUpdateAnchorage.bindValue(":pAnchorage", tempPort.pAnchorage);
        queryUpdateAnchorage.bindValue(":Capacity", tempPort.anchorage.capacity);
        queryUpdateAnchorage.bindValue(":MaxDraft", tempPort.anchorage.maxDraft);
        queryUpdateAnchorage.bindValue(":CostPerHour", tempPort.anchorage.costPerHour);
        queryUpdateAnchorage.exec();
        SQLConnect::debugQuery(queryUpdateAnchorage);

        QSqlQuery queryUpdateTransportCorridor;
        queryUpdateTransportCorridor.prepare("UPDATE SafeHarbour.TransportCorridor SET MaxSpeed = :MaxSpeed, MaxWidth = :MaxWidth, MaxDraft = :MaxDraft, CostPerHour = :CostPerHour, CapPerCorridor = :CapPerCorridor WHERE idTransportCorridor = :pCorridor;");
        queryUpdateTransportCorridor.bindValue(":pCorridor", tempPort.pCorridor);
        queryUpdateTransportCorridor.bindValue(":MaxSpeed", tempPort.transportCorridor.maxSpeed);
        queryUpdateTransportCorridor.bindValue(":MaxWidth", tempPort.transportCorridor.maxWidth);
        queryUpdateTransportCorridor.bindValue(":MaxDraft", tempPort.transportCorridor.maxDraft);
        queryUpdateTransportCorridor.bindValue(":CostPerHour", tempPort.transportCorridor.costPerHour);
        queryUpdateTransportCorridor.bindValue(":CapPerCorridor", tempPort.transportCorridor.capacityPerCorridor);
        queryUpdateTransportCorridor.exec();
        SQLConnect::debugQuery(queryUpdateTransportCorridor);

        QSqlQuery queryUpdateDock;
        queryUpdateDock.prepare("UPDATE SafeHarbour.Dock SET MaxDraft = :MaxDraft, ContainersPerHour = :ContainersPerHour, PassengersPerHour = :PassengersPerHour, CostPerHour = :CostPerHour, Capacity = :Capacity WHERE idDock = :pDock;");
        queryUpdateDock.bindValue(":pDock", tempPort.pDock);
        queryUpdateDock.bindValue(":MaxDraft", tempPort.dock.maxDraft);
        queryUpdateDock.bindValue(":ContainersPerHour", tempPort.dock.containerPerHour);
        queryUpdateDock.bindValue(":PassengersPerHour", tempPort.dock.passengersPerHour);
        queryUpdateDock.bindValue(":CostPerHour", tempPort.dock.costPerHour);
        queryUpdateDock.bindValue(":Capacity", tempPort.dock.capacity);
        queryUpdateDock.exec();
         SQLConnect::debugQuery(queryUpdateDock);

        QSqlQuery queryUpdatePort;
        queryUpdatePort.prepare("UPDATE SafeHarbour.Port SET NumberOfTugboats = :NumberOfTugboats, NumberOfCorridors = :NumberOfCorridors, NumberOfDocks = :NumberOfDocks WHERE Name = :Name;");
        queryUpdatePort.bindValue(":Name", tempPort.name);
        queryUpdatePort.bindValue(":NumberOfTugboats", tempPort.numberOfTugboats);
        queryUpdatePort.bindValue(":NumberOfCorridors", tempPort.numberOfCorridors);
        queryUpdatePort.bindValue(":NumberOfDocks", tempPort.numberOfDocks);
        queryUpdatePort.exec();
        SQLConnect::debugQuery(queryUpdatePort);

        SQLConnect::DisconnectDB();
        populateComboBox();
    }
}

