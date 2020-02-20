#pragma once

#include <QWidget>
#include <QGraphicsView>

class QGraphicsScene;
class PanelItem;

class MyGraphicView : public QGraphicsView
{
    Q_OBJECT

public:
    explicit MyGraphicView(QWidget* parent = 0);
    ~MyGraphicView();

private:
    QGraphicsScene* scene;
    PanelItem* panelItem;
};
