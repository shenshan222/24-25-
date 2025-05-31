#include "InfoDialog.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QDialogButtonBox>
#include <QTextBrowser>
#include <QPushButton>
#include <QGraphicsDropShadowEffect>
#include <QPropertyAnimation>

InfoDialog::InfoDialog(const QString& infoText, const QString& imagePath, QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle("当前位置信息");
    setFixedSize(640, 480);
    setWindowIcon(QIcon(":/PekingUniversityMap/signal of PKU.png"));
    setWindowModality(Qt::ApplicationModal); // 模态弹窗

    // 设置渐变背景
    setStyleSheet(
        "InfoDialog {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:1,"
        "       stop:0 #1e3c72, stop:1 #2a5298);"
        "   border-radius: 15px;"
        "}"
        );

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(20, 20, 20, 20);
    layout->setSpacing(15);

    if (!imagePath.isEmpty()) {
        QLabel* imageLabel = new QLabel;
        QPixmap imagePixmap(imagePath);
        if (!imagePixmap.isNull()) {
            int maxWidth = this->width() - 40;
            int maxHeight = this->height() - 40 - 250; // 预留给其他控件的空间

            imagePixmap = imagePixmap.scaled(maxWidth, maxHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            imageLabel->setPixmap(imagePixmap);
            imageLabel->setAlignment(Qt::AlignCenter);
            layout->addWidget(imageLabel);
        }
    }


    QTextBrowser* textBrowser = new QTextBrowser;
    textBrowser->setText(infoText);
    textBrowser->setReadOnly(true);
    textBrowser->setOpenExternalLinks(true);
    textBrowser->setStyleSheet(
        "QTextBrowser {"
        "   font-size: 28px;"
        "   font-family: '微软雅黑';"
        "   padding: 15px;"
        "   background: rgba(255, 255, 255, 0.15);"
        "   border: 2px solid rgba(255, 255, 255, 0.3);"
        "   border-radius: 12px;"
        "   color: #ffffff;"
        "   text-shadow: 1px 1px 2px rgba(0, 0, 0, 0.6);"
        "}"
        );
    layout->addWidget(textBrowser);

    QPushButton* okButton = new QPushButton("好的");
    okButton->setFixedSize(120, 40);
    okButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #00c6ff;"
        "   color: #004e92;"
        "   font-weight: bold;"
        "   border-radius: 20px;"
        "   border: 2px solid #00f0ff;"
        "   box-shadow: 0 0 15px #00c6ff;"
        "   transition: all 0.3s ease;"
        "}"
        "QPushButton:hover {"
        "   background-color: #00f0ff;"
        "   color: #002f4b;"
        "   box-shadow: 0 0 25px #00f0ff;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #007ea7;"
        "   color: #001b26;"
        "   box-shadow: none;"
        "}"
        );

    // 给按钮添加发光阴影效果
    QGraphicsDropShadowEffect* glowEffect = new QGraphicsDropShadowEffect(okButton);
    glowEffect->setBlurRadius(20);
    glowEffect->setColor(QColor(0, 255, 255));
    glowEffect->setOffset(0);
    okButton->setGraphicsEffect(glowEffect);

    layout->addWidget(okButton, 0, Qt::AlignCenter);

    // 按钮点击关闭对话框
    connect(okButton, &QPushButton::clicked, this, &QDialog::accept);

    // 弹窗淡入动画
    this->setWindowOpacity(0);
    QPropertyAnimation* fadeIn = new QPropertyAnimation(this, "windowOpacity");
    fadeIn->setDuration(100);
    fadeIn->setStartValue(0);
    fadeIn->setEndValue(1);
    fadeIn->start(QAbstractAnimation::DeleteWhenStopped);
}
