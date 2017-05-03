#ifndef WARNINGDIALOG_H
#define WARNINGDIALOG_H

#include <QDialog>

namespace Ui {
class WarningDialog;
}

class QSound;
class WarningDialog : public QDialog
{
    Q_OBJECT

public:
    explicit WarningDialog(QWidget *parent = 0);
    ~WarningDialog();

    void initDialogTitle();
    void setText(QString text);

public slots:
    void closeDialog();

private slots:
    void on_pushButton_clicked();

private:
    Ui::WarningDialog *ui;

//    QSound *bells;
};

#endif // WARNINGDIALOG_H
