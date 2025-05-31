#include "MainWindow.h"
#include <QInputDialog>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QGraphicsView>
#include <QDebug>
#include "navigationinputdialog.h"
#include <QGraphicsDropShadowEffect>
#include <QPropertyAnimation>
#include <QFile>
#include <QRandomGenerator>


//读取地点名称
QStringList readPlaceList(const QString& filePath)
{
    QStringList places;
    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        while (!in.atEnd()) {
            QString line = in.readLine().trimmed();
            if (!line.isEmpty()) {
                places.append(line);
            }
        }
        file.close();
    }
    return places;
}

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    QWidget* centralWidget = new QWidget(this);//创建中心部件

    // 给主窗口设置渐变蓝色背景
    centralWidget->setStyleSheet(
        "QWidget {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:1,"
        "       stop:0 #1e3c72, stop:1 #2a5298);"
        "}"
        );

    QVBoxLayout* mainlayout = new QVBoxLayout(centralWidget);
    mainlayout->setContentsMargins(0, 0, 0, 0);
    mainlayout->setSpacing(0);

    view = new ImageView(this);//创建地图
    view->setFixedSize(1346, 800);
    mainlayout->addWidget(view);

    QWidget* buttonBar = new QWidget(this);
    buttonBar->setFixedHeight(60);
    buttonBar->setStyleSheet(
        "background-color: rgba(30, 60, 114, 0.8);"  // 半透明深蓝色
        "border-top-left-radius: 15px;"
        "border-top-right-radius: 15px;"
        );

    QHBoxLayout* buttonLayout = new QHBoxLayout(buttonBar);
    buttonLayout->setAlignment(Qt::AlignCenter);
    buttonLayout->setContentsMargins(0, 8, 0, 8);

    // 创建并美化按钮 - 炫酷蓝色系
    startNavButton = new QPushButton("开始📍导航", this);
    startNavButton->setFixedSize(160, 48);
    startNavButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #409EFF;"
        "   color: white;"
        "   font-size: 18px;"
        "   font-weight: bold;"
        "   border-radius: 24px;"
        "   border: 2px solid #66b1ff;"
        "   box-shadow: 0 0 15px #409EFF;"
        "   transition: all 0.3s ease;"
        "}"
        "QPushButton:hover {"
        "   background-color: #66b1ff;"
        "   border-color: #a0cfff;"
        "   box-shadow: 0 0 30px #66b1ff;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #337ecc;"
        "   border-color: #337ecc;"
        "   box-shadow: none;"
        "}"
        );

    // 添加发光阴影效果
    auto* glowEffect = new QGraphicsDropShadowEffect(startNavButton);
    glowEffect->setBlurRadius(25);
    glowEffect->setColor(QColor(64, 158, 255)); // 蓝色光晕
    glowEffect->setOffset(0);
    startNavButton->setGraphicsEffect(glowEffect);

    eatButton = new QPushButton("🍴 去吃饭", this);
    eatButton->setFixedSize(120, 48);
    eatButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #FF9966;"  // 橙色
        "   color: white;"
        "   font-size: 16px;"
        "   border-radius: 24px;"
        "   border: 2px solid #FFCC99;"
        "   box-shadow: 0 0 15px #FF9966;"
        "   transition: all 0.3s ease;"
        "}"
        "QPushButton:hover {"
        "   background-color: #FFB385;"
        "   border-color: #FFE0C0;"
        "   box-shadow: 0 0 30px #FFB385;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #FF6633;"
        "   border-color: #FF6633;"
        "   box-shadow: none;"
        "}"
        );
    // 添加阴影效果
    auto* eatGlowEffect = new QGraphicsDropShadowEffect(eatButton);
    eatGlowEffect->setBlurRadius(20);
    eatGlowEffect->setColor(QColor(255, 153, 102)); // 橙色光晕
    eatButton->setGraphicsEffect(eatGlowEffect);

    // ================== 添加去学习按钮 ==================
    studyButton = new QPushButton("📚 去学习", this);
    studyButton->setFixedSize(120, 48);
    studyButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #66CC99;"  // 绿色
        "   color: white;"
        "   font-size: 16px;"
        "   border-radius: 24px;"
        "   border: 2px solid #99FFCC;"
        "   box-shadow: 0 0 15px #66CC99;"
        "   transition: all 0.3s ease;"
        "}"
        "QPushButton:hover {"
        "   background-color: #85E0B2;"
        "   border-color: #CCFFE5;"
        "   box-shadow: 0 0 30px #85E0B2;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #339966;"
        "   border-color: #339966;"
        "   box-shadow: none;"
        "}"
        );
    // 添加阴影效果
    auto* studyGlowEffect = new QGraphicsDropShadowEffect(studyButton);
    studyGlowEffect->setBlurRadius(20);
    studyGlowEffect->setColor(QColor(102, 204, 153)); // 绿色光晕
    studyButton->setGraphicsEffect(studyGlowEffect);

    // ================== 将按钮添加到布局 ==================
    // 顺序：去吃饭按钮 → 开始导航按钮 → 去学习按钮
    buttonLayout->addWidget(eatButton);
    buttonLayout->addWidget(startNavButton); // 原有开始导航按钮
    buttonLayout->addWidget(studyButton);

    mainlayout->addWidget(buttonBar);

    setCentralWidget(centralWidget);

    setWindowTitle("北京大学校内导航");
    setWindowIcon(QIcon(":/PekingUniversityMap/signal of PKU.png"));

    campusGraph.loadNodesFromFile(":/PekingUniversityMap/nodes.txt");
    campusGraph.loadEdgesFromFile(":/PekingUniversityMap/edges.txt");

    BuildingNames = readPlaceList(":/PekingUniversityMap/names.txt");
    eatPlaces = readPlaceList(":/PekingUniversityMap/eat.txt");    // 或者你的实际路径
    studyPlaces = readPlaceList(":/PekingUniversityMap/study.txt");

    connect(startNavButton, &QPushButton::clicked, this, &MainWindow::onStartNavigationClicked);
    connect(eatButton, &QPushButton::clicked, this, &MainWindow::handleRandomEatPlace);
    connect(studyButton, &QPushButton::clicked, this, &MainWindow::handleRandomStudyPlace);
}

QVector<QVector<QString>> MainWindow::splitPathByWaypoints(
    const QVector<QString>& fullPath,
    const QString& startId,
    const QVector<QString>& waypointIds,
    const QString& endId)
{
    QVector<QVector<QString>> segments;
    QVector<QString> points;
    points.append(startId);
    points += waypointIds;
    points.append(endId);

    int currentIndex = 0;
    for (int i = 1; i < points.size(); ++i) {
        QString from = points[i - 1];
        QString to = points[i];

        QVector<QString> segment;
        bool started = false;
        for (int j = currentIndex; j < fullPath.size(); ++j) {
            if (fullPath[j] == from) started = true;
            if (started) segment.append(fullPath[j]);
            if (fullPath[j] == to) {
                currentIndex = j;
                break;
            }
        }
        segments.append(segment);
    }
    return segments;
}


void MainWindow::onStartNavigationClicked()
{
    NavigationInputDialog dialog(BuildingNames, this);
    if (dialog.exec() != QDialog::Accepted) return;

    QString startName = dialog.getStartName();
    QString endName = dialog.getEndName();
    QStringList waypointNames = dialog.getWaypoints();

    QString startId = campusGraph.getIdByName(startName);
    QString endId = campusGraph.getIdByName(endName);

    if (startId.isEmpty() || endId.isEmpty()) {
        QMessageBox::warning(this, "错误", "起点或终点名称无效！");
        return;
    }

    QVector<QString> waypointIds;
    for (const QString& name : waypointNames) {
        QString id = campusGraph.getIdByName(name);
        if (id.isEmpty()) {
            QMessageBox::warning(this, "错误", QString("无效的途径点名称：%1").arg(name));
            return;
        }
        waypointIds.append(id);
    }

    QVector<QString> fullPath;
    if (!waypointIds.isEmpty()) {
        fullPath = campusGraph.findPathThroughWaypoints(startId, waypointIds, endId);
    } else {
        fullPath = campusGraph.aStarSearch(startId, endId);
    }

    if (fullPath.isEmpty()) {
        QMessageBox::information(this, "结果", "未找到路径");
        return;
    }

    if (waypointIds.isEmpty()) {
        // 只有起点和终点，单一路径绘制
        view->highlightPath(fullPath, campusGraph);
    } else {
        // 多段路径绘制，不同颜色
        QVector<QVector<QString>> segments = splitPathByWaypoints(fullPath, startId, waypointIds, endId);
        view->highlightMultiSegmentPaths(segments, campusGraph, waypointIds);
    }

    QMessageBox::information(this, "结果", "导航路径已显示");
}

// 随机选择一个吃饭地点并打开
void MainWindow::handleRandomEatPlace() {
    if (eatPlaces.isEmpty()) {
        QMessageBox::warning(this, "提示", "暂无吃饭地点数据");
        return;
    }

    // 随机选择地点名称
    int randomIndex = QRandomGenerator::global()->bounded(eatPlaces.size());
    QString randomName = eatPlaces[randomIndex];

    // 高亮并显示地点信息
    view->highlightAndClickNode(randomName);
}

void MainWindow::handleRandomStudyPlace() {
    if (studyPlaces.isEmpty()) {
        QMessageBox::warning(this, "提示", "暂无学习地点数据");
        return;
    }

    int randomIndex = QRandomGenerator::global()->bounded(studyPlaces.size());
    QString randomName = studyPlaces[randomIndex];
    QString nodeId = campusGraph.nameToId.value(randomName);

    view->highlightAndClickNode(randomName);
}
