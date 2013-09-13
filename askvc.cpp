#include "askvc.hpp"
#include <boost/iterator/iterator_concepts.hpp>

askvc::askvc(QWidget *parent) :
    QDialog(parent)
{
    ui.setupUi(this);
}

void askvc::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui.retranslateUi(this);
        break;
    default:
        break;
    }
}

void askvc::on_buttonBox_accepted()
{
	this->accept();
    //this->destroy();
}

void askvc::setvcimage(std::string vc)
{
	QPixmap img;
	img.loadFromData(reinterpret_cast<const uchar*>(vc.data()), vc.size(), "JPG");
	ui.img->setPixmap(img);
}


std::string askvc::getvc()
{
	return ui.vc->text().toStdString();
}

