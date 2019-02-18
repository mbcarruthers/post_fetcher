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
    setFixedSize(850,850);
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
       ui->list_widget->addItem("[");
       for( int i = 0; i < array.size();++i )
       {
           ui->list_widget->addItem("   {");
           QJsonObject object = array.at(i).toObject();
           QVariantMap map = object.toVariantMap();

           // get the the values from the keys
           QString user_id = map["userId"].toString();
           QString id = map["id"].toString();
           QString title = map["title"].toString();
           QString body = map["body"].toString();

           //add values to the list_widget
           ui->list_widget->addItem( "      userId: " + user_id);
           ui->list_widget->addItem( "      id: " + id);
           ui->list_widget->addItem("       title: " + title);
           ui->list_widget->addItem("       body: " + body);
           ui->list_widget->addItem("   },");
       }
       ui->list_widget->addItem("]");
    }
}
