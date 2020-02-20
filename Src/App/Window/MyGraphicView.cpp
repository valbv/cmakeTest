#include "MyGraphicView.h"

#include <QGraphicsScene>

#include "SpinBoxPanel/PanelItem.h"

MyGraphicView::MyGraphicView(QWidget* parent)
    : QGraphicsView(parent)
{
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setAlignment(Qt::AlignCenter);
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    this->setMinimumHeight(800);
    this->setMinimumWidth(800);

    scene = new QGraphicsScene();
    this->setScene(scene);

//    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::X11BypassWindowManagerHint
//#if !defined(Q_OS_MAC)
//        | Qt::Tool
//#endif
//    );

    setAttribute(Qt::WA_TranslucentBackground);
    setMouseTracking(true);
    setAutoFillBackground(false);

    PanelItem* panelItem = new PanelItem();

    scene->addItem(panelItem);
}

MyGraphicView::~MyGraphicView()
{
    panelItem->deleteLater();
}
