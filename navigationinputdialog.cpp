#include "NavigationInputDialog.h"
#include "SubstringFilterProxyModel.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QCompleter>
#include <QHBoxLayout>
#include <QStringListModel>
#include <QFile>
#include <QTextStream>
#include <QStringList>
#include <QDebug>
#include <QEvent>

QCompleter* createContainsCompleter(const QStringList& items, QObject* parent = nullptr) {
    auto* model = new QStringListModel(items, parent);
    auto* proxyModel = new SubstringFilterProxyModel(parent);
    proxyModel->setSourceModel(model);
    proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);

    auto* completer = new QCompleter(proxyModel, parent);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setFilterMode(Qt::MatchContains);
    return completer;
}

NavigationInputDialog::NavigationInputDialog(const QStringList& placeNames, QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle("路线规划");

    auto* wrapperLayout = new QVBoxLayout(this);

    auto* topLabel = new QLabel("请输入或从列表中选择起点、终点及途径点", this);
    topLabel->setAlignment(Qt::AlignCenter);
    wrapperLayout->addWidget(topLabel);

    auto* mainLayout = new QHBoxLayout();
    wrapperLayout->addLayout(mainLayout);

    // 左侧输入区域（起点、终点、途径点管理）
    auto* inputLayout = new QVBoxLayout();

    // 起点
    inputLayout->addWidget(new QLabel("起点："));
    startEdit = new QLineEdit(this);
    startEdit->setCompleter(createContainsCompleter(placeNames, this));
    inputLayout->addWidget(startEdit);

    // 终点
    inputLayout->addWidget(new QLabel("终点："));
    endEdit = new QLineEdit(this);
    endEdit->setCompleter(createContainsCompleter(placeNames, this));
    inputLayout->addWidget(endEdit);

    // 途径点输入与列表
    inputLayout->addWidget(new QLabel("途径点："));

    auto* waypointInputLayout = new QHBoxLayout();
    waypointEdit = new QLineEdit(this);
    waypointEdit->setCompleter(createContainsCompleter(placeNames, this));
    waypointInputLayout->addWidget(waypointEdit);

    addWaypointButton = new QPushButton("添加", this);
    waypointInputLayout->addWidget(addWaypointButton);
    inputLayout->addLayout(waypointInputLayout);

    waypointList = new QListWidget(this);
    waypointList->setSelectionMode(QAbstractItemView::SingleSelection);
    inputLayout->addWidget(waypointList);

    removeWaypointButton = new QPushButton("删除选中途径点", this);
    inputLayout->addWidget(removeWaypointButton);

    // 确认和取消按钮
    auto* buttonLayout = new QHBoxLayout();
    okButton = new QPushButton("确定", this);
    cancelButton = new QPushButton("取消", this);
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);
    inputLayout->addLayout(buttonLayout);

    mainLayout->addLayout(inputLayout);

    // 右侧节点列表（可点击添加起点/终点/途径点）
    auto* listLayout = new QVBoxLayout();
    listLayout->addWidget(new QLabel("可导航节点列表："));

    nodeListWidget = new QListWidget(this);
    nodeListWidget->addItems(placeNames);
    nodeListWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    listLayout->addWidget(nodeListWidget);

    mainLayout->addLayout(listLayout);

    // 事件过滤，追踪哪个编辑框获得焦点
    startEdit->installEventFilter(this);
    endEdit->installEventFilter(this);
    waypointEdit->installEventFilter(this);

    // 点击节点列表，填充焦点对应的输入框
    connect(nodeListWidget, &QListWidget::itemClicked, this, [this](QListWidgetItem* item){
        if (!item) return;
        QString nodeName = item->text();

        if (lastFocusedEdit == startEdit) {
            startEdit->setText(nodeName);
        } else if (lastFocusedEdit == endEdit) {
            endEdit->setText(nodeName);
        } else if (lastFocusedEdit == waypointEdit) {
            waypointEdit->setText(nodeName);
        } else {
            // 如果没有焦点，则默认添加到途径点列表（避免重复）
            auto existing = waypointList->findItems(nodeName, Qt::MatchExactly);
            if (existing.isEmpty()) {
                waypointList->addItem(nodeName);
            }
        }
    });


    // 添加途径点按钮点击事件
    connect(addWaypointButton, &QPushButton::clicked, this, [this]() {
        QString wp = waypointEdit->text().trimmed();
        if (wp.isEmpty()) return;

        // 避免重复添加
        auto items = waypointList->findItems(wp, Qt::MatchExactly);
        if (!items.isEmpty()) return;

        waypointList->addItem(wp);
        waypointEdit->clear();
    });

    // 删除途径点按钮点击事件
    connect(removeWaypointButton, &QPushButton::clicked, this, [this]() {
        auto selectedItems = waypointList->selectedItems();
        for (auto* item : selectedItems) {
            delete waypointList->takeItem(waypointList->row(item));
        }
    });

    connect(okButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
}

QStringList NavigationInputDialog::getWaypoints() const {
    QStringList waypoints;
    for (int i = 0; i < waypointList->count(); ++i) {
        waypoints.append(waypointList->item(i)->text());
    }
    return waypoints;
}

QString NavigationInputDialog::getStartName() const {
    return startEdit->text();
}

QString NavigationInputDialog::getEndName() const {
    return endEdit->text();
}

bool NavigationInputDialog::eventFilter(QObject* watched, QEvent* event)
{
    if (event->type() == QEvent::FocusIn) {
        if (watched == startEdit) {
            lastFocusedEdit = startEdit;
        } else if (watched == endEdit) {
            lastFocusedEdit = endEdit;
        } else if (watched == waypointEdit) {
            lastFocusedEdit = waypointEdit;
        }
    }
    return QDialog::eventFilter(watched, event);
}

