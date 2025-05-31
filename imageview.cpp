#include "ImageView.h"
#include "graph.h"
#include <QPixmap>
#include <QWheelEvent>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QDebug>
#include <QGraphicsEllipseItem>
#include <QMouseEvent>
#include <QLinearGradient>
#include <QTimer>
#include <QGraphicsDropShadowEffect>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include "infodialog.h"

ImageView::ImageView(QWidget* parent)
    : QGraphicsView(parent), scaleFactor(1.0)
{
    scene = new QGraphicsScene(this);
    setScene(scene);//创建并设置场景

    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);//以鼠标为缩放锚点
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);//不显示滚动条
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setRenderHint(QPainter::SmoothPixmapTransform);//平滑缩放

    QPixmap pixmap(":/PekingUniversityMap/PKU.jpg");//加载图片
    if (pixmap.isNull())
    {
        qDebug() << "Failed to load image!";
    }

    pixmapItem = new QGraphicsPixmapItem(pixmap);
    scene->addItem(pixmapItem);//将图片对象加入场景
    setSceneRect(pixmap.rect());
    centerOn(pixmapItem);//居中显示
    scale(0.3, 0.3);//缩小初始大小
    scaleFactor = 0.3;

    loadNodesFromFile(":/PekingUniversityMap/click_nodes.txt");
}

void ImageView::loadNodesFromFile(const QString& filename)//加载可点击节点信息
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::warning(this, "错误", "文件打开失败: " + filename);
        return;
    }

    QTextStream in(&file);
    QString line;//读取坐标信息
    QString image;//读取图片信息

    while (!in.atEnd())
    {
        do {
            image = in.readLine().trimmed();
        } while (image.isEmpty() && !in.atEnd());

        if (image.isEmpty()) break;

        do {
            line = in.readLine().trimmed();
        } while (line.isEmpty() && !in.atEnd());

        if (line.isEmpty()) break;

        QStringList coords = line.split(' ', Qt::SkipEmptyParts);//读取坐标
        if (coords.size() != 2) continue;

        bool okX, okY;
        qreal x = coords[0].toDouble(&okX);
        qreal y = coords[1].toDouble(&okY);
        if (!okX || !okY) continue;

        QStringList infoLines;
        QString Name;
        bool flag = false;
        while (!in.atEnd())
        {
            QString infoLine = in.readLine();
            if (infoLine.trimmed().isEmpty()) break;
            infoLines << infoLine;
            if (!flag) {
                flag = true;
                Name = infoLine;
            }
        }

        QString infoText = infoLines.join("\n");//读取信息
        QPixmap icon(":/PekingUniversityMap/Red_Star.jpg"); // 使用 Qt 资源系统
        icon = icon.scaled(40, 40, Qt::KeepAspectRatio, Qt::SmoothTransformation);

        ClickNode* node = new ClickNode(image, QPointF(x, y), infoText, icon);
        nodeMap[Name] = node;
        scene->addItem(node);

    }

    file.close();
}

void ImageView::wheelEvent(QWheelEvent* event)//缩放图片
{
    qreal zoomFactor = event->angleDelta().y() > 0 ? 1.1 : 0.9;
    scaleFactor *= zoomFactor;//缩放参数

    if (scaleFactor < 0.1)//控制缩放范围
    {
        scaleFactor = 0.1;
        zoomFactor = 1.0 / 0.9;
    }
    if (scaleFactor > 10.0)
    {
        scaleFactor = 10.0;
        zoomFactor = 1.0 / 1.1;
    }

    scale(zoomFactor, zoomFactor);//进行缩放
}

void ImageView::mousePressEvent(QMouseEvent* event)//点击事件
{
    if (event->button() == Qt::LeftButton) {
        setDragMode(QGraphicsView::ScrollHandDrag); // 启用拖动模式
        setCursor(Qt::ClosedHandCursor);            // 设置为闭合的手型
        // QPoint viewPos = event->pos();//获取点击点的坐标
        // QPointF scenePos = mapToScene(viewPos);
        // qDebug() << "Scene position:" << scenePos;

        // QGraphicsEllipseItem* tempDot = new QGraphicsEllipseItem(-2, -2, 4, 4);
        // tempDot->setBrush(Qt::red);
        // tempDot->setPos(scenePos);
        // scene->addItem(tempDot);
    }

    QGraphicsView::mousePressEvent(event);
}

void ImageView::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        setDragMode(QGraphicsView::NoDrag);   // 禁用拖动模式
        setCursor(Qt::ArrowCursor);           // 恢复为箭头光标
    }

    QGraphicsView::mouseReleaseEvent(event);
}

// 辅助函数：创建带发光边框的图标
QPixmap createGlowMarker(const QPixmap& icon, const QColor& glowColor, int glowSize = 96, int glowWidth = 10) {
    QPixmap glowPixmap(glowSize, glowSize);
    glowPixmap.fill(Qt::transparent);

    QPainter painter(&glowPixmap);
    painter.setRenderHint(QPainter::Antialiasing);

    // 画发光边框（圆形描边）
    QPen pen(glowColor, glowWidth);
    painter.setPen(pen);
    painter.setBrush(Qt::NoBrush);
    QPoint center(glowSize / 2, glowSize / 2);
    int radius = glowSize / 2 - glowWidth / 2;
    painter.drawEllipse(center, radius, radius);

    // 画图标
    QPixmap scaledIcon = icon.scaled(glowSize - glowWidth * 2, glowSize - glowWidth * 2,
                                     Qt::KeepAspectRatio, Qt::SmoothTransformation);
    QPoint iconPos(center.x() - scaledIcon.width() / 2, center.y() - scaledIcon.height() / 2);
    painter.drawPixmap(iconPos, scaledIcon);

    return glowPixmap;
}

void ImageView::drawNodeMarkers(const QVector<QString>& path, const Graph& graph, const QVector<QString>& waypointNames) {
    // 清除旧标注
    for (QGraphicsItem* marker : nodeMarkers.values()) {
        scene->removeItem(marker);
        delete marker;
    }
    nodeMarkers.clear();

    // 标注起点
    if (!path.isEmpty()) {
        QString startName = path.first();
        if (graph.nodes.contains(startName)) {
            Position pos = graph.nodes[startName].pos;
            QPixmap icon(":/PekingUniversityMap/start_flag.jpg");
            QPixmap glowIcon = createGlowMarker(icon, QColor(0, 255, 0, 180)); // 绿色发光
            QGraphicsPixmapItem* marker = new QGraphicsPixmapItem(glowIcon);
            marker->setOffset(-glowIcon.width() / 2, -glowIcon.height() / 2);
            marker->setPos(pos.x, pos.y);
            marker->setToolTip("起点：" + graph.nodes[startName].name);
            scene->addItem(marker);
            nodeMarkers[startName] = marker;
        }
    }

    // 标注终点
    if (path.size() >= 2) {
        QString endName = path.last();
        if (graph.nodes.contains(endName)) {
            Position pos = graph.nodes[endName].pos;
            QPixmap icon(":/PekingUniversityMap/end_flag.jpg");
            QPixmap glowIcon = createGlowMarker(icon, QColor(255, 0, 0, 180)); // 红色发光
            QGraphicsPixmapItem* marker = new QGraphicsPixmapItem(glowIcon);
            marker->setOffset(-glowIcon.width() / 2, -glowIcon.height() / 2);
            marker->setPos(pos.x, pos.y);
            marker->setToolTip("终点：" + graph.nodes[endName].name);
            scene->addItem(marker);
            nodeMarkers[endName] = marker;
        }
    }

    // 标注途径点
    for (const QString& waypointName : waypointNames) {
        if (graph.nodes.contains(waypointName)) {
            Position pos = graph.nodes[waypointName].pos;
            QPixmap icon(":/PekingUniversityMap/waypoint_flag.jpg");
            QPixmap glowIcon = createGlowMarker(icon, QColor(0, 128, 255, 180)); // 蓝色发光
            QGraphicsPixmapItem* marker = new QGraphicsPixmapItem(glowIcon);
            marker->setOffset(-glowIcon.width() / 2, -glowIcon.height() / 2);
            marker->setPos(pos.x, pos.y);
            marker->setToolTip("途径点：" + graph.nodes[waypointName].name);
            scene->addItem(marker);
            nodeMarkers[waypointName] = marker;
        }
    }
}



void ImageView::highlightPath(const QVector<QString>& path, const Graph& graph)//画出路径图
{
    // 先清除之前的路径线条
    for (QGraphicsLineItem* lineItem : pathLines) {
        scene->removeItem(lineItem);
        delete lineItem;
    }
    pathLines.clear();

    if (path.size() < 2) return;

    drawNodeMarkers(path, graph, QVector<QString>()); // 途径点列表为空

    // 创建渐变色（起点到终点颜色渐变）
    // 从绿色到紫色
    QColor startColor = Qt::green;
    QColor endColor = Qt::magenta;

    // 用于动态波动宽度的定时器
    static int frame = 0;

    for (int i = 0; i < path.size() - 1; ++i) {
        QString id1 = path[i];
        QString id2 = path[i + 1];

        if (!graph.nodes.contains(id1) || !graph.nodes.contains(id2))
            continue;

        Position pos1 = graph.nodes[id1].pos;
        Position pos2 = graph.nodes[id2].pos;

        QPointF p1(pos1.x, pos1.y);
        QPointF p2(pos2.x, pos2.y);

        QLinearGradient gradient(p1, p2);
        gradient.setColorAt(0, startColor);
        gradient.setColorAt(1, endColor);

        // 计算当前线条宽度波动，周期为60帧
        double width = 3 + 2 * std::sin((frame + i * 10) * 3.14159 / 30);

        QPen pen(QBrush(gradient), width);
        pen.setCapStyle(Qt::RoundCap);

        QGraphicsLineItem* line = scene->addLine(QLineF(p1, p2), pen);

        // 添加发光效果
        auto* glowEffect = new QGraphicsDropShadowEffect();
        glowEffect->setColor(Qt::cyan);
        glowEffect->setBlurRadius(15);
        glowEffect->setOffset(0);
        line->setGraphicsEffect(glowEffect);

        pathLines.append(line);
    }

    // 让路径动画起来（波动宽度）
    if (!pathAnimationTimer) {
        pathAnimationTimer = new QTimer(this);
        connect(pathAnimationTimer, &QTimer::timeout, this, [this, path, graph]() {
            static int frame = 0;
            frame++;
            for (int i = 0; i < pathLines.size(); ++i) {
                QGraphicsLineItem* line = pathLines[i];
                double width = 10 + 2 * std::sin((frame + i * 10) * 3.14159 / 30);
                QPen pen = line->pen();
                pen.setWidthF(width);
                line->setPen(pen);
            }
        });
        pathAnimationTimer->start(50); // 20fps动画
    }
}

void ImageView::highlightMultiSegmentPaths(const QVector<QVector<QString>>& segments, const Graph& graph, const QVector<QString>& waypointIds)
{
    // 清除之前所有路径线条
    for (QGraphicsLineItem* lineItem : pathLines) {
        scene->removeItem(lineItem);
        delete lineItem;
    }
    pathLines.clear();

    if (segments.isEmpty()) return;

    drawNodeMarkers(flattenPath(segments), graph, waypointIds); // flattenPath 将多段路径展平为完整节点列表

    // 定义多个渐变起始色和终止色（可以调整颜色数量）
    QVector<QPair<QColor, QColor>> gradientColors = {
        { Qt::green, Qt::darkGreen },
        { Qt::blue, Qt::darkBlue },
        { Qt::yellow, Qt::darkYellow },
        { Qt::magenta, Qt::darkMagenta },
        { Qt::cyan, Qt::darkCyan }
    };

    // 每段路径分别绘制
    for (int segIndex = 0; segIndex < segments.size(); ++segIndex) {
        const QVector<QString>& path = segments[segIndex];
        if (path.size() < 2) continue;

        QColor startColor = gradientColors[segIndex % gradientColors.size()].first;
        QColor endColor = gradientColors[segIndex % gradientColors.size()].second;

        for (int i = 0; i < path.size() - 1; ++i) {
            QString id1 = path[i];
            QString id2 = path[i + 1];

            if (!graph.nodes.contains(id1) || !graph.nodes.contains(id2))
                continue;

            Position pos1 = graph.nodes[id1].pos;
            Position pos2 = graph.nodes[id2].pos;

            QPointF p1(pos1.x, pos1.y);
            QPointF p2(pos2.x, pos2.y);

            QLinearGradient gradient(p1, p2);
            gradient.setColorAt(0, startColor);
            gradient.setColorAt(1, endColor);

            // 这里不做渐变动态，不同段线条颜色固定，宽度动画保持一致
            QPen pen(QBrush(gradient), 8);  // 默认宽度8
            pen.setCapStyle(Qt::RoundCap);

            QGraphicsLineItem* line = scene->addLine(QLineF(p1, p2), pen);

            // 发光效果
            auto* glowEffect = new QGraphicsDropShadowEffect();
            glowEffect->setColor(startColor.lighter(150));
            glowEffect->setBlurRadius(15);
            glowEffect->setOffset(0);
            line->setGraphicsEffect(glowEffect);

            pathLines.append(line);
        }
    }

    // 启动动画（波动宽度）
    if (!pathAnimationTimer) {
        pathAnimationTimer = new QTimer(this);
        connect(pathAnimationTimer, &QTimer::timeout, this, [this]() {
            static int frame = 0;
            frame++;
            for (int i = 0; i < pathLines.size(); ++i) {
                QGraphicsLineItem* line = pathLines[i];
                double width = 4 + 2 * std::sin((frame + i * 10) * 3.14159 / 30);
                QPen pen = line->pen();
                pen.setWidthF(width);
                line->setPen(pen);
            }
        });
        pathAnimationTimer->start(50);
    }
}

QVector<QString> ImageView::flattenPath(const QVector<QVector<QString>>& segments) {
    QVector<QString> fullPath;
    for (const auto& seg : segments) {
        fullPath += seg;
    }
    return fullPath;
}

void ImageView::highlightAndClickNode(const QString& nodeName)
{
    if (!nodeMap.contains(nodeName)) {
        QMessageBox::warning(this, "错误", QString("未找到节点 ID：%1").arg(nodeName));
        return;
    }

    ClickNode* node = nodeMap[nodeName];

    centerOn(node);

    // 模拟点击：弹出 InfoDialog
    InfoDialog* dialog = new InfoDialog(node->infoText, node->imagePath);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->show();
}
