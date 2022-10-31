#include "about.h"
#include "ui_about.h"

About::About(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::About)
{
    ui->setupUi(this);

    ui->label_Link->setText("<a href=\"https://github.com/Miguel-Gotor/CLI_Calculator\">More Info...</a>");
    ui->label_Link->setTextFormat(Qt::RichText);
    ui->label_Link->setTextInteractionFlags(Qt::TextBrowserInteraction);
    ui->label_Link->setOpenExternalLinks(true);
}

About::~About()
{
    delete ui;
}

void About::on_pushButton_clicked()
{
    this->close();
}
