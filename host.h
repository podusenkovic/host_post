#ifndef HOST_H
#define HOST_H

#include "card.h"

#include <QWidget>
#include <QTcpServer>
#include <QTcpSocket>
#include <QNetworkConfiguration>
#include <QNetworkSession>
#include <QtNetwork>
#include <QtCore>
#include <QMessageBox>
#include <QDebug>
#include <QFile>
#include <QTextStream>

namespace Ui {
class Host;
}

struct myTransaction{
	int idTran;
	QString card;
	QString amountTran;
};

class Host : public QWidget
{
    Q_OBJECT
    
public:
    explicit Host(QWidget *parent = 0);
    
    ~Host();
private slots:
    void sessionOpened();
    void handleRequest();
	void readData();
	
private:
    Ui::Host *ui;
	QVector<double> balances;
	QVector<cardInfo*> vectorCards;
    QTcpServer* tcpServer = nullptr;
    QNetworkSession* networkSession = nullptr;
	int idTran;
	QVector<myTransaction> allTran;
};

#endif // HOST_H
