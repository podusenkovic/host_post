#include "host.h"
#include "ui_host.h"

cardInfo *stringToCard(QString card){
	QStringList qsl = card.split(":");
	cardInfo *temp = new cardInfo(qsl[0], qsl[1], qsl[2], qsl[3]);
	return temp;
}


Host::Host(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Host)
{
	ui->setupUi(this);
	idTran = 0;
    QNetworkConfigurationManager manager;
    if (manager.capabilities() & QNetworkConfigurationManager::NetworkSessionRequired) {
        QSettings settings(QSettings::UserScope, QLatin1String("QtProject"));
        settings.beginGroup(QLatin1String("QtNetwork"));
        const QString id = settings.value(QLatin1String("DefaultNetworkConfiguration")).toString();
        settings.endGroup();
        
        QNetworkConfiguration config = manager.configurationFromIdentifier(id);
        if ((config.state() & QNetworkConfiguration::Discovered) !=
            QNetworkConfiguration::Discovered) {
            config = manager.defaultConfiguration();
        }

        networkSession = new QNetworkSession(config, this);
        connect(networkSession, SIGNAL(opened()), 
                this, SLOT(sessionOpened()));
		
        ui->label->setText(tr("Opening network session."));
        networkSession->open();
    } else {
        sessionOpened();
    }
	
    connect(tcpServer, SIGNAL(newConnection()),
			this, SLOT(handleRequest()));
	
	
	ui->labelForData->setText("");
	//---------------------------------------------
	//--TODO: READ DATABASE WITH CARDS FROM FILE---
	QFile database("C:\\Users\\podus\\Documents\\Qt\\Point of Sale Project\\host_post\\database.txt");
	database.open(QIODevice::ReadOnly | QIODevice::Text);
	
	QTextStream in(&database);
	
	int len = in.readLine().toInt();
	
	for (int i = 0; i < len; i++){
		QString line = in.readLine();
		vectorCards.push_back(stringToCard(line));
		balances.push_back(line.split(":").at(4).toDouble());
		//ui->labelForData->setText(ui->labelForData->text() + "\n" + line);
	}
	//---------------------------------------------

	setWindowTitle("Host for POST");
}

Host::~Host()
{
    delete ui;
}

void Host::sessionOpened(){
    if (networkSession) {
        QNetworkConfiguration config = networkSession->configuration();
        QString id;
        if (config.type() == QNetworkConfiguration::UserChoice)
        id = networkSession->sessionProperty(QLatin1String("UserChoiceConfiguration")).toString();
        else
            id = config.identifier();
        QSettings settings(QSettings::UserScope, QLatin1String("QtProject"));
        settings.beginGroup(QLatin1String("QtNetwork"));
        settings.setValue(QLatin1String("DefaultNetworkConfiguration"), id);
        settings.endGroup();
	}
    
    tcpServer = new QTcpServer(this);
    if (!tcpServer->listen()) {
		QMessageBox::critical(this, tr("Host for post"),
                              tr("Unable to start the server: %1.")
                               .arg(tcpServer->errorString()));
        close();
        return;
    }
    QString ipAddress;
    QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
    for (int i = 0; i < ipAddressesList.size(); ++i) {
		if (ipAddressesList.at(i) != QHostAddress::LocalHost &&
				ipAddressesList.at(i).toIPv4Address()) {
            ipAddress = ipAddressesList.at(i).toString();
            break;
         }
     }
    if (ipAddress.isEmpty())
        ipAddress = QHostAddress(QHostAddress::LocalHost).toString();
	
    ui->label->setText(tr("The host is running on\n\nIP: %1\nport: %2\n\n"
                           "You can connect here with post.")
					   .arg(ipAddress).arg(tcpServer->serverPort()));//tcpServer->serverPort()));
}


void Host::handleRequest(){
	QTcpSocket *Client = tcpServer->nextPendingConnection();
	connect(Client, SIGNAL(readyRead()), this, SLOT(readData()));
	//QByteArray block;
}

void Host::readData(){
	QTcpSocket *Client = (QTcpSocket*)sender();
	
	QDataStream in;
	in.setDevice(Client);
	
	QString dataIn, dataOut;
	in >> dataIn;
	QString code = dataIn.split(":")[1];
	if (code == "0010"){
		dataOut = "0020";
	}
	if (code == "0000"){
		dataOut = "0001";	
	}
	if (code == "0210"){
		QString number = dataIn.split(":")[2];
		QString name = dataIn.split(":")[3];
		QString date = dataIn.split(":")[4];
		QString cvv = dataIn.split(":")[5];
		for (int i = 0; i < vectorCards.size(); i++)
			if (vectorCards[i]->number == number){
				if (vectorCards[i]->cardHolderName == name 
					&& vectorCards[i]->dateExpire == date
					&& vectorCards[i]->cardsCVV == cvv)
					dataOut = "0220:" + QString::number(balances[i]);
				else dataOut = "0123"; // bad name || date || cvv
				break;
			}
		if (dataOut.isEmpty())
			dataOut = "0124"; // bad number
	}
	
	if (code == "0110"){
		QString money = dataIn.split(":")[2];
		QString number = dataIn.split(":")[3];
		QString name = dataIn.split(":")[4];
		QString date = dataIn.split(":")[5];
		QString cvv = dataIn.split(":")[6];
		if (!money.isEmpty()){
			for (int i = 0; i < vectorCards.size(); i++)
				if (vectorCards[i]->number == number){
					if (vectorCards[i]->cardHolderName == name 
						&& vectorCards[i]->dateExpire == date
						&& vectorCards[i]->cardsCVV == cvv)
						if (money.toDouble() <= balances[i]){
							dataOut = "0120";
							balances[i] -= money.toDouble();
							idTran++;
							allTran.push_back(myTransaction{idTran, number, money});
						}
						else {
							dataOut = "0125"; // not enough money
							break;
						}
					else dataOut = "0123"; // bad name || date || cvv
					break;
				}
			if (dataOut.isEmpty())
				dataOut = "0124"; // bad number
		}
		else dataOut = "0126"; //bad request
	}
	
	if(code == "0530"){
		QMap<QString, int> summary;
		for (int i = 0; i < allTran.size(); i++){
			if(!summary.keys().contains(allTran[i].card))
				summary[allTran[i].card] = 0;
			summary[allTran[i].card] = summary[allTran[i].card] + allTran[i].amountTran.toInt();
		}	
		QString sumUp;
		for (int i = 0; i < summary.keys().size(); i++){
			sumUp = summary.keys()[i] + " " + QString::number(summary[summary.keys()[i]]) + "\r\n";
		}
		if (sumUp == dataIn.split(":")[2])
			dataOut = "0540";
		else dataOut = "0127";
	}
		
	QByteArray block;
	QDataStream out(&block, QIODevice::WriteOnly);
	out.setVersion(QDataStream::Qt_4_0);
	
	out << dataOut;
	Client->write(block);
	
	ui->labelForData->setText(dataIn);
}
