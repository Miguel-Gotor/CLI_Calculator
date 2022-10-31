#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtMath>
#include <QDateTime>
#include <QToolTip>

// to enable / disable additional utilites

//#define autocompleter
//#define expressionValidator

#ifdef autocompleter
#include <QCompleter>
#endif

#ifdef expressionValidator
#include <QRegularExpressionValidator>
#endif

#define M_PHI 1.618033988749895 // 15 decimal digits of precision (casted as double)

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setCentralWidget(ui->verticalLayoutWidget); // Important, this can only be done after ui->setupUi(this);
    ui->lineEdit->setFocus();

    v1 = new About(this);

    clearWidgets();

    // Regular Expression Validator to prevent useless characters (symbols beside the operation ones)

#ifdef expressionValidator
    QRegularExpression regExp("^[a-z0-9*+-/^=()]+$");
    QRegularExpressionValidator *myRegExpVal = new QRegularExpressionValidator(regExp, this);
    ui->lineEdit->setValidator(myRegExpVal);
#endif

    // AutoCompleter for the commands

#ifdef autocompleter
    const QStringList commandList = {"ans", "clear", "exit", "test"};
    QCompleter *myCompleter = new QCompleter(commandList, this);
    ui->lineEdit->setCompleter(myCompleter);
#endif
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_lineEdit_returnPressed()
{
    if (ui->lineEdit->text() == "clear")
    {
        clearWidgets();
    }
    else if (ui->lineEdit->text() == "exit")
    {
        this->close();
    }
    else if (ui->lineEdit->text() == "test")
    {
        test();
    }

    else if (ui->lineEdit->text() != "")
    {
        QString input = ui->lineEdit->text();

        qDebug() << "Input:";
        qDebug() << input;

        ui->listWidget_history->addItem(input);
        ui->listWidget_history->item(ui->listWidget_history->count() - 1)->setForeground(QColor(249, 145, 87));
        ui->listWidget_time->addItem(QTime::currentTime().toString());
        ui->lineEdit->clear();

        if (input[0] != '#')
        {
            QString output =  QString::number(solveParenthesis(input), 'g', 15);
            ui->listWidget_history->addItem("= " + output);

            ui->listWidget_time->addItem("");
            ui->listWidget_time->setCurrentRow(ui->listWidget_time->count()-1);


            ui->listWidget_history->item(ui->listWidget_history->count() - 1)->setForeground(QColor(153, 204, 153)); // setForeground expects a QBrush parameter; QColor can be used directly.
            ui->listWidget_history->setCurrentRow(ui->listWidget_history->count()-1);
        }
        else
        {
            qDebug() << "IT'S A COMMENT";
            qDebug() << ui->listWidget_history->count();
            ui->listWidget_history->item(ui->listWidget_history->count() - 1)->setForeground(QColor(102, 204, 204));
        }
    }
}

void MainWindow::test(void)
{
    clearWidgets();

    ui->lineEdit->setText("# NESTED SQUARE ROOTS");
    on_lineEdit_returnPressed();

    ui->lineEdit->setText("sqrt(sqrt(sqrt(256)))");
    on_lineEdit_returnPressed();

    ui->lineEdit->setText("# NESTED PARENTHESIS");
    on_lineEdit_returnPressed();
    ui->lineEdit->setText("1+1*(2+2*(3+3*(4+4*(5*5)/6-6)/7-7)/8-8)/9-9");
    on_lineEdit_returnPressed();

    ui->lineEdit->setText("# NESTED PARENTHESIS INSIDE SQUARE ROOT");
    on_lineEdit_returnPressed();
    ui->lineEdit->setText("sqrt(((25)))");
    on_lineEdit_returnPressed();

    ui->lineEdit->setText("# EXPONENTIAL FUNCTION");
    on_lineEdit_returnPressed();
    ui->lineEdit->setText("2.5e3");
    on_lineEdit_returnPressed();

    ui->lineEdit->setText("50e-3");
    on_lineEdit_returnPressed();

    ui->lineEdit->setText("# MATHEMATICAL CONSTANTS");
    on_lineEdit_returnPressed();
    ui->lineEdit->setText("pi");
    on_lineEdit_returnPressed();

    ui->lineEdit->setText("e");
    on_lineEdit_returnPressed();

    ui->lineEdit->setText("phi");
    on_lineEdit_returnPressed();
}

// Clear all UI widgets
void MainWindow::clearWidgets()
{
    ui->lineEdit->clear();
    ui->listWidget_history->clear();
    ui->listWidget_time->clear();
}

// Solve operations within parenthesis
double MainWindow::solveParenthesis(const QString &input)
{
    qDebug() << "ENTERING: solveParenthesis()";

    QStringList list = {""};

    QList <uint8_t> levelSqrt;
    uint8_t indexSqrt   = -1;

    for (int i = 0; i < input.length(); i++)
    {
        if (input[i] == '(')
        {
            qDebug() << "FOUND: LEFT PARENTHESIS";

            // This block adds multiplication symbol before the parenthesis, when it's preceded by a number e.g. 2(3-2) = 2*(3-2)
                if (i > 0)
                {
                    if (input[i-1].isNumber())
                    {
                        qDebug() << "APPENDED '*'";
                        list.last().append('*');
                    }

                    // Check the keyword before the parenthesis

                    else if (input.length() > 4)
                    {
                        if (input.mid(i-4,4) == "sqrt")
                        {
                            list.last().remove(list.last().length()-4, list.last().length());   // Last 4 characters are trimmed ("sqrt")
                            qDebug() << "FOUND: SQUARE ROOT";
                            levelSqrt.append(1);
                            indexSqrt++;
                        }

                        else if (levelSqrt.count()>0) // Solo se empiezan a contar parentesis desde que se encuentra la primera funcion
                        {
                            qDebug() << "INCREASED SQRT LVL";
                            levelSqrt[indexSqrt]++;
                        }
                    }
                }

                list.append("");
                qDebug() << "LEVELSQRT:" <<levelSqrt;
        }
        else if (input[i] == ')' && list.length() > 1) // Second condition prevents program form crashing (index out of range)
        {
            qDebug() << "FOUND: RIGHT PARENTHESIS";

            if (levelSqrt.isEmpty() == false)
            {
                levelSqrt.last()--;
                qDebug() << "LEVELSQRT:" <<levelSqrt;
                if (levelSqrt.last() == 0)   // Level will be '0' after leaving the last parenthesis (that matches the first left parenthesis)
                {
                    indexSqrt--;
                    qDebug() << "EVALUATING: SQRT CONTENT";
                    list[list.length()-2].append(QString::number(sqrt(evaluateExpression(list.last())), 'g', 15));
                    levelSqrt.removeLast();
                    qDebug() << i << "º iteration:" << list;
                    list.removeLast();
                }
            }
            else
            {
                list[list.length()-2].append(QString::number(evaluateExpression(list.last())));
                list.removeLast();
            }
        }
        else
        {
            list.last().append(input[i]);
        }

        qDebug() << i << "# ITERATION. LIST:     "      << list;

    }
    return evaluateExpression(list.last());
}

// Actually replaces a mathematical expression with the corresponding numerical result

double MainWindow::evaluateExpression(const QString &expr)
{
    qDebug() << "ENTERING: evaluateExpression()";

    QList <int>     operationList;
    QList <double>  numberList;
    QStringList     strNumberList = {""};

    bool lastCharWasSymbol = 0; // Used to ignore redundant operators

    // strNumberList is built

    for (int i = 0; i < expr.length(); i++)
    {
        if (expr[i].isNumber() || expr[i] == '.' || (lastCharWasSymbol && !expr[i].isLetter()))
        {
            lastCharWasSymbol = false;
            strNumberList.last().append(expr[i]);
        }
        //--------------------------------------------------------------------------------------------------
        else if (expr[i] == 'e')    // There's 2 options for the letter 'e'
        {
            // 1st: Scientific Input. Exponent. (e.g 2e3 = 2000)

            if (lastCharWasSymbol == false && i > 0 && i < expr.length())    // In wich case, will not be preceeded by a symbol, but a number.
            {
                qDebug() << "E X P O N E N T E" ;
                strNumberList.last().append("e");
                if (expr.length()> i + 1)
                {
                    strNumberList.last().append(expr[i+1]); i++;
                }
            }

            // 2nd: Irrational Euler's Number: 2.7182818284590452353...

            else
            {
                if (i > 0)
                {
                    if (expr[i-1].isNumber())
                    {
                        strNumberList.append("");
                        operationList.append(1);
                    }
                }
                strNumberList.last().append(QString::number(M_E, 'g', 15));
            }

            lastCharWasSymbol = false;
        }
        //---------------------------------------------------------------------------------------------------
        else if (expr.mid(i-2, 3) == "ans" && ui->listWidget_history->count() > 0)
        {
                strNumberList.last().append(ui->listWidget_history->item(ui->listWidget_history->count()-1)->text().remove(0,2));
        }
        else if (expr.mid(i-1, 2) == "pi")
        {
            if (i > 1)
            {
                if (expr[i-2].isNumber())
                {
                    strNumberList.append("");
                    operationList.append(1);
                }
            }

            strNumberList.last().append(QString::number(M_PI, 'g', 15));
            lastCharWasSymbol = false;
        }
        else if (expr.mid(i-2, 3) == "phi")
        {
            if (i > 2)
            {
                if (expr[i-3].isNumber())
                {
                    strNumberList.append("");
                    operationList.append(1);
                }
            }
            strNumberList.last().append(QString::number(M_PHI, 'g', 15));
        }

                // List of math operators
        else if (   expr[i] == '+' ||
                    expr[i] == '-' ||
                    expr[i] == '*' ||
                    expr[i] == '/' ||
                    expr[i] == '%' ||
                    expr[i] == '^'  )
        {
                lastCharWasSymbol = true;
                strNumberList.append("");

                qDebug() << "Strnumberlist updated" << strNumberList;

            // operationList is built

            if (expr[i] == '*')         operationList.append(1);
            else if (expr[i] == '/')    operationList.append(2);
            else if (expr[i] == '+')    operationList.append(3);
            else if (expr[i] == '-')    operationList.append(4);
            else if (expr[i] == '^')    operationList.append(5);
            else if (expr[i] == '%')    operationList.append(6);
        }
    }

    qDebug() << "First iteration after building lists";
    qDebug() << "numberList:   " << qSetRealNumberPrecision(15) << numberList; // Double has 15 decimal digits of precision.
    qDebug() << "operationList:" << operationList << "\n";

    // 1º - STRING TO DOUBLE

    for (int i = 0; i < strNumberList.length(); i++)
    {
        qDebug() << "CONVERSION: STR TO DOUBLE";
        qDebug() << strNumberList[i];
        numberList.append(strNumberList[i].toDouble());
        qDebug() << numberList[i];
    }

    qDebug() << "After replacing letters with numbers:";
    qDebug() << "numberList:   " << qSetRealNumberPrecision(15) << numberList; // Double has 15 decimal digits of precision.
    qDebug() << "operationList:" << operationList << "\n";

    // 2º - POWERS AND PERCENTS

    for (int i = 0; i < operationList.length(); i++)
    {
        if (operationList[i] == 5)              // Power ('^')
        {
            // Simple function to raise double to integer
            /*
            int exponent = numberList[i+1];
            double base  = numberList[i];

            for (int j = 1; j < exponent; j++)
            {
                numberList[i] *= base;
            }
            */
            // QMath function to raise double to double
            numberList[i] = pow(numberList[i],numberList[i+1]);
            numberList.removeAt(i+1);
            operationList.removeAt(i);
            i--;
        }
        else if (operationList[i] == 6)         // Percent ('%')
        {
            numberList[i] = numberList[i] / 100 * numberList[i+1];
            operationList.removeAt(i);
            i--;
        }
    }

    // 2º - PRODUCTS AND DIVISIONS

    for (int i = 0; i < operationList.length(); i++)
    {
        if (operationList[i] == 1)              // Product ('*')
        {
                numberList[i] *= numberList[i + 1];
                numberList.removeAt(i + 1);
                operationList.removeAt(i);
                i--;
        }
        else if (operationList[i] == 2)         // Division ('/')
        {
                numberList[i] /= numberList[i + 1];
                numberList.removeAt(i + 1);
                operationList.removeAt(i);
                i--;
        }
    }

    qDebug() << "After solving factors:";
    qDebug() << "numberList:   " << qSetRealNumberPrecision(15) << numberList; // Double has 15 decimal digits of precision.
    qDebug() << "operationList:" << operationList << "\n";

    // 3º - ADDITIONS AND SUBSTRACTION

    for (int i = 0; i < operationList.length(); i++)
    {
        if (operationList[i] == 3)              // Addition ('+')
        {
                numberList[i] += numberList[i + 1];
                numberList.removeAt(i + 1);
                operationList.removeAt(i);
                i--;
        }
        else if (operationList[i] == 4)         // Substraction ('-')
        {
                numberList[i] -= numberList[i + 1];
                numberList.removeAt(i + 1);
                operationList.removeAt(i);
                i--;
        }
    }

    qDebug() << "After solving summands";
    qDebug() << "numberList:   " << qSetRealNumberPrecision(15) << numberList; // Double has 15 decimal digits of precision.
    qDebug() << "operationList:" << operationList << "\n";

    if (numberList.isEmpty())
    {
        return 0;
    }
    else
    {
        return numberList.first();
    }
}

void MainWindow::on_lineEdit_textEdited(const QString &arg1)
{
    if (arg1 != "" && arg1[0] != '#')
    {
        qDebug() << "NOT A COMMENT";
        QToolTip::showText(ui->lineEdit->mapToGlobal(QPoint(-2, 0)), "Current result: <font color='blue'>" + QString::number(solveParenthesis(arg1), 'g', 15));
    }
    else
    {
        QToolTip::hideText();
    }
}

void MainWindow::on_actionClear_history_triggered()
{
    ui->listWidget_history->clear();
    ui->listWidget_time->clear();
}

void MainWindow::on_actionClear_input_triggered()
{
    ui->lineEdit->clear();
}

void MainWindow::on_actionZoom_in_triggered()
{
    QFont appFont(ui->lineEdit->font());

    appFont.setPointSize(appFont.pointSize() + 1);

    ui->lineEdit->setFont(appFont);
    ui->listWidget_history->setFont(appFont);
    ui->listWidget_time->setFont(appFont);
}

void MainWindow::on_actionShow_time_stamps_triggered()
{
    if (ui->listWidget_time->isHidden() == false)
    {
        ui->listWidget_time->hide();
    }
    else
    {
        ui->listWidget_time->show();
    }
}

void MainWindow::on_actionClear_all_triggered()
{
    clearWidgets();
}

void MainWindow::on_actionZoom_out_triggered()
{
    QFont appFont(ui->lineEdit->font());
    appFont.setPointSize(appFont.pointSize() - 1);

    ui->lineEdit->setFont(appFont);
    ui->listWidget_history->setFont(appFont);
    ui->listWidget_time->setFont(appFont);
}

void MainWindow::on_actionInsert_Previous_Input_triggered()
{
    int currentRow = ui->listWidget_history->currentRow();
    if (currentRow > 0)
    {
        currentRow--;
        ui->listWidget_history->setCurrentItem(ui->listWidget_history->item(currentRow));
        ui->listWidget_time->setCurrentItem(ui->listWidget_time->item(currentRow));
        ui->lineEdit->setText(ui->listWidget_history->currentItem()->text());
    }
}

void MainWindow::on_actionInsert_Next_Input_triggered()
{
    int currentRow = ui->listWidget_history->currentRow();
    if (currentRow < ui->listWidget_history->count() - 1)
    {
        currentRow++;
        ui->listWidget_time->setCurrentItem(ui->listWidget_time->item(currentRow));
        ui->listWidget_history->setCurrentItem(ui->listWidget_history->item(currentRow));
        ui->lineEdit->setText(ui->listWidget_history->currentItem()->text());
    }
}

// Green toolbar color
// background-color:rgb(102, 180, 20);\ncolor:rgb(255, 255, 255);

void MainWindow::on_listWidget_history_itemClicked(QListWidgetItem *item)
{
    ui->lineEdit->setText(item->text());
}

void MainWindow::on_actionAbout_triggered()
{
    v1->show();
}

