#include "card.h"
#include "ui_card.h"

Card::Card(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::Card),
	name_val(QRegExp("^[A-Za-z]{3,7} [A-Za-z]{4,14}$"))
{
	ui->setupUi(this);	

	ui->label_card_date->setVisible(false);
	ui->label_card_name->setVisible(false);
	ui->label_card_number->setVisible(false);
	ui->label_card_valid_thru->setVisible(false);
	
	connect(ui->button_ok, SIGNAL(clicked(bool)),
			this, SLOT(saveData()));
	connect(ui->button_clear, SIGNAL(clicked(bool)),
			this, SLOT(clearData()));
	connect(ui->button_close, SIGNAL(clicked(bool)),
			this, SLOT(close()));
	
	ui->line_edit_number->setInputMask("9999 9999 9999 9999");
	ui->line_edit_cvv->setInputMask("999");
	ui->line_edit_date->setInputMask("99/99");
	
	keepNameRight();
	connect(ui->line_edit_name, SIGNAL(textEdited(QString)),
			this, SLOT(keepNameRight()));
	
}

Card::~Card()
{
	delete ui;
}

void Card::saveData(){
	savedCard = new cardInfo(ui->line_edit_number->text(),
							 ui->line_edit_name->text(),
							 ui->line_edit_date->text(),
							 ui->line_edit_cvv->text());
	ui->line_edit_cvv->setText("");
	ui->line_edit_name->setText("");
	ui->line_edit_date->setText("");
	ui->line_edit_date->setText("");
	emit accepted();
	drowCard();
}

void Card::clearData(){
	ui->line_edit_cvv->setText("");
	ui->line_edit_name->setText("");
	ui->line_edit_date->setText("");
	ui->line_edit_number->setText("");
}

void Card::drowCard(){
	//-----------------------------------
	ui->button_clear->setVisible(false);
	ui->button_close->setVisible(false);
	ui->button_ok->setVisible(false);
	ui->label_cvv->setVisible(false);
	ui->label_name->setVisible(false);
	ui->label_num->setVisible(false);
	ui->label_date->setVisible(false);
	ui->line_edit_cvv->setVisible(false);
	ui->line_edit_name->setVisible(false);
	ui->line_edit_number->setVisible(false);
	ui->line_edit_date->setVisible(false);
	//-----------------------------------
	ui->label_card_date->setText(savedCard->dateExpire);
	ui->label_card_name->setText(savedCard->cardHolderName.toUpper());
	ui->label_card_number->setText(savedCard->number);	
	//-----------------------------------
	this->setStyleSheet("background-color: rgb(41,157,55)");
	ui->label_card_date->setVisible(true);
	ui->label_card_name->setVisible(true);
	ui->label_card_number->setVisible(true);
	ui->label_card_valid_thru->setVisible(true);
	ui->label_card_date->setEnabled(true);
	ui->label_card_name->setEnabled(true);
	ui->label_card_number->setEnabled(true);
	ui->label_card_valid_thru->setEnabled(true);	
	//-----------------------------------
}

void Card::keepNameRight(){
	QString newText = ui->line_edit_name->text().toUpper();
	int pos = 0;
	if (name_val.validate(newText, pos) == QValidator::Invalid) {
		newText.chop(1);
	}
	ui->line_edit_name->setText(newText);
}

cardInfo* Card::getSavedCard(){
	return savedCard;
}
