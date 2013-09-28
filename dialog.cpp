
#include <QFileDialog>
#include <QMessageBox>
#include <QItemSelection>
#include <QStandardItemModel>
#include <QInputDialog>
#include <QDataWidgetMapper>
#include <boost/format.hpp>

#include <boost/make_shared.hpp>
#include "dialog.hpp"
#include "boost/avloop.hpp"
#include "askvc.hpp"

Dialog::Dialog(QWidget* parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	ui.listView_buddy->setModel(&m_buddymodel);

	ui.qqnum->setText("1239795499");
	ui.qqpwd->setText("tdsb123sbtd");

	qRegisterMetaType<QSharedPointer<std::vector<webqq::qqBuddy_ptr> > > ("QSharedPointer<std::vector<webqq::qqBuddy_ptr> >");
	qRegisterMetaType<std::string>("std::string");
	qRegisterMetaType<boost::system::error_code>("boost::system::error_code");

	connect(this, SIGNAL(update_buddy_list(QSharedPointer<std::vector<webqq::qqBuddy_ptr> >)),
			this, SLOT(on_update_buddy_list(QSharedPointer<std::vector<webqq::qqBuddy_ptr> >)), Qt::QueuedConnection);
	connect(this, SIGNAL(need_vc(std::string)),
			this, SLOT(on_vc(std::string)), Qt::QueuedConnection);
	connect(this, SIGNAL(send_finshed(boost::system::error_code)),
			this, SLOT(on_send_finshed(boost::system::error_code)), Qt::QueuedConnection);
}

Dialog::~Dialog()
{
	m_webqq.reset();

	io_service.stop();

	if (m_io_thread.joinable())
		m_io_thread.join();
}

void Dialog::changeEvent(QEvent* e)
{
	QDialog::changeEvent(e);

	switch (e->type())
	{
		case QEvent::LanguageChange:
			ui.retranslateUi(this);
			break;

		default:
			break;
	}
}

void Dialog::on_pushButton_broswe_clicked()
{
	// 打开一个文件选择界面.
	ui.filetoupload->setText(QFileDialog(this).getOpenFileName(this));

	bool pushButton_upload_enable = false;

	if (m_webqq && m_webqq->is_online())
	{
		if (!ui.filetoupload->text().isEmpty())
			pushButton_upload_enable = true;
	}

	ui.pushButton_upload->setEnabled(pushButton_upload_enable);
}

void Dialog::on_qqnum_returnPressed()
{
	ui.qqpwd->setFocus();
}

void Dialog::detect_login_clickable()
{
	if (!ui.qqnum->text().isEmpty() && !ui.qqpwd->text().isEmpty())
		ui.pushButton_login->setEnabled(true);
	else
		ui.pushButton_login->setEnabled(false);
}

void Dialog::on_qqnum_textChanged(const QString&)
{
	detect_login_clickable();
}

void Dialog::on_qqpwd_textChanged(const QString&)
{
	detect_login_clickable();
}

void Dialog::on_pushButton_login_clicked()
{
	std::string qqnum = ui.qqnum->text().toStdString();
	std::string qqpwd = ui.qqpwd->text().toStdString();
	// 登录吧,少年.
	m_webqq.reset(new webqq::webqq(io_service, qqnum, qqpwd, true));

	ui.pushButton_login->setEnabled(false);
	ui.qqnum->setEnabled(false);
	ui.qqpwd->setEnabled(false);

	m_webqq->disable_group_fetch();

	m_webqq->on_logined(boost::bind(&Dialog::updatebuddy_view, this));
	m_webqq->on_verify_code(boost::bind(&Dialog::libwebqq_need_vc, this, _1));

	m_io_thread = boost::thread(boost::bind(&avloop_run, boost::ref(io_service)));

	ui.checkBox_is_logined->setText(tr("正在登录, 请稍后......"));
}

void Dialog::libwebqq_need_vc(std::string vc)
{
	emit need_vc(vc);
}

void Dialog::on_vc(std::string vcimg)
{
	askvc  askvcdlg(this);

	askvcdlg.setvcimage(vcimg);
	askvcdlg.setWindowTitle("输入验证码");
	askvcdlg.exec();

	std::string vc = askvcdlg.getvc();

	io_service.post(boost::bind(&webqq::webqq::feed_vc, m_webqq.data(), vc, boost::function<void() >()));
}

void Dialog::updatebuddy_view()
{
	// 从 m_webqq 获得 buddy list
	std::vector<webqq::qqBuddy_ptr> buddies = m_webqq->get_buddies();

	QSharedPointer<std::vector<webqq::qqBuddy_ptr> > buddies_ptr(
		new std::vector<webqq::qqBuddy_ptr> (buddies));

	// 现在是不在主线程上, 因此通过 emit 信号让 slot 执行在主线程.
	// 这是 Qt 版的 傻逼 io_service.post 克隆.
	emit update_buddy_list(buddies_ptr);
}

void Dialog::on_update_buddy_list(QSharedPointer< std::vector< webqq::qqBuddy_ptr > > buddies)
{
	ui.checkBox_is_logined->setText(tr("已登录!"));
	ui.checkBox_is_logined->setCheckState(Qt::Checked);
	m_buddymodel.set_buddies(buddies);
}

void Dialog::on_pushButton_upload_clicked()
{
	std::string uin = ui.listView_buddy->currentIndex().data(Qt::UserRole).toString().toStdString();

	m_webqq->send_offline_file(uin, ui.filetoupload->text().toStdString(),
							   boost::bind<void> (&Dialog::sended, this, _1)
							  );
	ui.upload_progress->setTextVisible (true);
	ui.upload_progress->setFormat(tr("上传中，请稍后..."));
}

void Dialog::sended(boost::system::error_code ec)
{
	// 发送完毕! 发送信号过去, 这样就在 GUI 线程里跑了.
	emit send_finshed(ec);
}

void Dialog::on_send_finshed(boost::system::error_code ec)
{
	// 这才是发送完毕后应该执行的，这里在 GUI 线程里执行.
	if (ec)
	{
		ui.upload_progress->setFormat(tr("上传失败"));
	}else
	{
		ui.upload_progress->setFormat(tr("离线文件上传成功！"));
	}
}

void Dialog::on_listView_buddy_indexesMoved(const QModelIndexList& indexes)
{
	bool pushButton_upload_enable = false;

	if (m_webqq && m_webqq->is_online())
	{
		if (!ui.filetoupload->text().isEmpty())
			pushButton_upload_enable = true;
	}

	ui.pushButton_upload->setEnabled(pushButton_upload_enable);
}

void Dialog::on_listView_buddy_clicked(const QModelIndex& index)
{
	bool pushButton_upload_enable = false;

	if (m_webqq && m_webqq->is_online())
	{
		if (!ui.filetoupload->text().isEmpty())
			pushButton_upload_enable = true;
	}

	ui.pushButton_upload->setEnabled(pushButton_upload_enable);
}
