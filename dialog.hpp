#ifndef DIALOG_HPP
#define DIALOG_HPP

#include <QtCore/QScopedPointer>
#include <QtCore/QSharedPointer>
#include <QStringListModel>

#include <boost/thread.hpp>
#include "ui_dialog.h"
#include "libwebqq/webqq.hpp"
#include "buddymodel.hpp"

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    virtual ~Dialog();

protected:
    void changeEvent(QEvent *e);

    void detect_login_clickable();

    void updatebuddy_view();

    void libwebqq_need_vc(std::string);

    void sended(boost::system::error_code);

private slots:

	void on_vc(std::string str);

    void on_pushButton_broswe_clicked();

    void on_qqnum_returnPressed();

    void on_qqnum_textChanged(const QString &arg1);

    void on_qqpwd_textChanged(const QString &arg1);

    void on_pushButton_login_clicked();

    void on_pushButton_upload_clicked();

    void on_listView_buddy_indexesMoved(const QModelIndexList &indexes);

    void on_update_buddy_list(QSharedPointer< std::vector<webqq::qqBuddy_ptr> >);
    void on_listView_buddy_clicked(const QModelIndex &index);

signals:

	void need_vc(std::string);
	void update_buddy_list(QSharedPointer< std::vector<webqq::qqBuddy_ptr> >);

private:
    Ui::MainDialog ui;
    QScopedPointer<webqq::webqq> m_webqq;

    boost::asio::io_service io_service;
    boost::thread m_io_thread;
    BuddyModel m_buddymodel;
};

#endif // DIALOG_HPP
