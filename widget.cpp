#include "widget.h"
#include "ui_widget.h"
#include <QMessageBox>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QVariantMap>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget),
    manager( new QNetworkAccessManager(this)),
    reply(nullptr),
    buffer_array(new QByteArray)

{
    ui->setupUi(this);  
}

Widget::~Widget()
{
    delete ui;
    delete buffer_array;
}

void Widget::on_fetch_button_clicked()
{
    QNetworkRequest request{};
    request.setUrl(QUrl("https://jsonplaceholder.typicode.com/posts"));

    reply = manager->get(request);
    connect(reply,&QIODevice::readyRead,this,&Widget::data_ready_read);
    connect(reply, &QNetworkReply::finished,this,&Widget::data_read_finished);
}

void Widget::data_ready_read()
{
    buffer_array->append(reply->readAll());
}

void Widget::data_read_finished()
{
    if( reply->error()) {
        QMessageBox::critical(this,
                              "!",
                              reply->errorString());
    } else {
       QJsonDocument json_doc = QJsonDocument::fromJson(*buffer_array);
       QJsonArray const array = json_doc.array();

       for( int i = 0; i < array.size();++i )
       {
           QJsonObject object = array.at(i).toObject();
           QVariantMap map = object.toVariantMap();

           QString title = map["title"].toString();
           qInfo() << "title[" << QString::number(i) << "] "
                   << title << "\n";
       }
    }
}
