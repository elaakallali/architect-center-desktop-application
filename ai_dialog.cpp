#include "ai_dialog.h"
#include "ui_ai_dialog.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QNetworkRequest>

AIDialog::AIDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::AIDialog), networkManager(new QNetworkAccessManager(this))
{
    ui->setupUi(this);
    setWindowTitle("Chat with AI");

    connect(ui->buttonSend, &QPushButton::clicked, this, &AIDialog::onSendClicked);
}

AIDialog::~AIDialog()
{
    delete ui;
}

void AIDialog::addMessage(const QString &sender, const QString &message)
{
    QListWidgetItem *item = new QListWidgetItem(QString("%1: %2").arg(sender, message));
    if (sender == "You") {
        item->setForeground(Qt::blue);
    } else {
        item->setForeground(Qt::green);
    }
    ui->listWidgetChat->addItem(item);
    ui->listWidgetChat->scrollToBottom();
}

void AIDialog::onSendClicked()
{
    QString text = ui->textEditInput->toPlainText().trimmed();
    if (text.isEmpty()) {
        return;
    }

    addMessage("You", text);
    ui->textEditInput->clear();
    sendToAI(text);
}

void AIDialog::sendToAI(const QString &text)
{
    QString apiKey = "YOUR_API_KEY"; // Replace with your actual API key
    QUrl url("https://generativelanguage.googleapis.com/v1beta/models/gemini-1.5-flash:generateContent?key=" + apiKey);

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject systemInstruction;
    systemInstruction["role"] = "user";
    systemInstruction["parts"] = QJsonArray{
        QJsonObject{{"text", "You are an AI assistant to help managing employees. Only answer questions regarding employees."}}
    };

    QJsonObject userInput;
    userInput["role"] = "user";
    userInput["parts"] = QJsonArray{
        QJsonObject{{"text", text}}
    };

    QJsonObject payload;
    payload["contents"] = QJsonArray{ systemInstruction, userInput };

    QJsonDocument doc(payload);

    currentReply = networkManager->post(request, doc.toJson());
    connect(currentReply, &QNetworkReply::finished, this, &AIDialog::handleReply);
}

void AIDialog::handleReply()
{
    currentReply->deleteLater();

    if (currentReply->error() != QNetworkReply::NoError) {
        QMessageBox::warning(this, "Error", "Failed to get a response from the AI.");
        return;
    }

    QByteArray response = currentReply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(response);
    QJsonObject obj = doc.object();

    QString result;
    if (obj.contains("candidates")) {
        QJsonArray candidates = obj["candidates"].toArray();
        if (!candidates.isEmpty()) {
            result = candidates[0].toObject()["text"].toString();
        }
    }

    addMessage("AI", result);
}