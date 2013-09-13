#ifndef ASKVC_HPP
#define ASKVC_HPP

#include "ui_askvc.h"

class askvc : public QDialog
{
    Q_OBJECT

public:
    explicit askvc(QWidget *parent = 0);

    std::string getvc();

    void setvcimage(std::string vc);

protected:
    void changeEvent(QEvent *e);

private slots:
    void on_buttonBox_accepted();

private:
    Ui::askvc ui;
};

#endif // ASKVC_HPP
