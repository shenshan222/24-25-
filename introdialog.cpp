#include "IntroDialog.h"
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QFont>
#include <QIcon>
#include <QGraphicsDropShadowEffect>
#include <QPropertyAnimation>
#include <QTimer>

IntroDialog::IntroDialog(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle("欢迎使用");
    setWindowIcon(QIcon(":/PekingUniversityMap/signal of PKU.png"));
    setFixedSize(450, 270);
    setWindowModality(Qt::ApplicationModal);

    // 设置渐变背景
    setStyleSheet(
        "IntroDialog {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:1,"
        "       stop:0 #34495E, stop:1 #2C3E50);"
        "   border-radius: 15px;"
        "}"
        );

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(30, 30, 30, 30);
    layout->setSpacing(20);

    QLabel* titleLabel = new QLabel("北京大学校内导航");
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet(
        "font-size: 28px;"
        "font-weight: bold;"
        "color: #00F0FF;"
        "font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;"
        "text-shadow: 0 0 8px #00F0FF;"
        );
    layout->addWidget(titleLabel);

    label = new QLabel(
        "欢迎使用北京大学校内导航！\n\n"
        "本程序可用于浏览校园地图，查看各地点信息，"
        "并实现路径导航功能，助您更高效地了解校园。\n\n"
        "本程序还可帮您随机选择吃饭或学习的地点。", this);
    label->setWordWrap(true);
    label->setStyleSheet(
        "font-size: 17px;"
        "color: #B0E0FF;"
        "font-family: '微软雅黑';"
        "text-shadow: 0 0 4px rgba(0, 255, 255, 0.6);"
        );
    layout->addWidget(label);

    btn = new QPushButton("开始使用", this);
    btn->setFixedSize(140, 48);
    btn->setStyleSheet(
        "QPushButton {"
        "  background-color: #00CFFF;"
        "  color: #002F4B;"
        "  font-size: 18px;"
        "  font-weight: bold;"
        "  border-radius: 24px;"
        "  border: 2px solid #00E5FF;"
        "  box-shadow: 0 0 15px #00CFFF;"
        "  transition: all 0.3s ease;"
        "}"
        "QPushButton:hover {"
        "  background-color: #00E5FF;"
        "  color: #001B26;"
        "  box-shadow: 0 0 30px #00E5FF;"
        "}"
        "QPushButton:pressed {"
        "  background-color: #008EA0;"
        "  color: #001B26;"
        "  box-shadow: none;"
        "}"
        );

    // 按钮光晕效果
    auto* glowEffect = new QGraphicsDropShadowEffect(btn);
    glowEffect->setBlurRadius(30);
    glowEffect->setColor(QColor(0, 255, 255));
    glowEffect->setOffset(0);
    btn->setGraphicsEffect(glowEffect);

    layout->addWidget(btn, 0, Qt::AlignCenter);

    connect(btn, &QPushButton::clicked, this, &QDialog::accept);

    // 弹窗打开淡入动画
    setWindowOpacity(0);
    auto* fadeIn = new QPropertyAnimation(this, "windowOpacity");
    fadeIn->setDuration(200);
    fadeIn->setStartValue(0);
    fadeIn->setEndValue(1);
    fadeIn->start(QAbstractAnimation::DeleteWhenStopped);

}
