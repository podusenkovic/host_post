#ifndef CARD_H
#define CARD_H

#include <QDialog>
#include <QRegExpValidator>
#include <QDebug>
namespace Ui {
	class Card;
}

class cardInfo{
	friend class Card;	
	friend class Host;
	QString number;
	QString cardHolderName;
	QString dateExpire;
	QString cardsCVV;
public:
	cardInfo(QString n, QString h, QString d, QString cvv){
		number = n;
		cardHolderName = h;
		dateExpire = d;
		cardsCVV = cvv;
	}
	QString getNumber(){
		return number;
	}
	QString cardToString(){
		QString card = number + ":"
					   + cardHolderName + ":"
					   + dateExpire + ":"
					   + cardsCVV;
		return card;
	}
};

class Card : public QDialog
{
	Q_OBJECT
	
public:
	explicit Card(QWidget *parent = 0);
	~Card();
	
public slots:
	void saveData();
	void clearData();
	void drowCard();
	void keepNameRight();
	cardInfo* getSavedCard();
private:
	cardInfo *savedCard;
	Ui::Card *ui;
	QRegExpValidator name_val;
};

#endif // CARD_H
