#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidgetItem>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    double evaluateExpression(const QString &expr);

    double solveParenthesis(const QString &input);

    void test(void);

    void clearWidgets(void);

    void on_lineEdit_returnPressed();

    void on_lineEdit_textEdited(const QString &arg1);

    void on_listWidget_history_itemDoubleClicked(QListWidgetItem *item);

    void on_actionClear_history_triggered();

    void on_actionClear_input_triggered();

    void on_actionZoom_in_triggered();

    void on_actionShow_time_stamps_triggered();

    void on_actionClear_all_triggered();

    void on_actionZoom_out_triggered();

    void on_actionInsert_Previous_Input_triggered();

    void on_actionInsert_Next_Input_triggered();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
