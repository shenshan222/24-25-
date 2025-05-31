#pragma once

#include <QMainWindow>
#include <QPushButton>
#include "graph.h"
#include "imageview.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);

private slots:
    void onStartNavigationClicked();

private:
    QPushButton* startNavButton;//开始导航按钮
    Graph campusGraph;//基本图信息
    ImageView* view;
    QStringList BuildingNames;
    QStringList eatPlaces;
    QStringList studyPlaces;
    QPushButton* eatButton;
    QPushButton* studyButton;
    QVector<QVector<QString>> splitPathByWaypoints(
        const QVector<QString>& fullPath,
        const QString& startId,
        const QVector<QString>& waypointIds,
        const QString& endId);
    void handleRandomEatPlace();
    void handleRandomStudyPlace();
};
