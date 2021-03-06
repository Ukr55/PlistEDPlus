
#include <QtGui>
#include <QtXml>

#include "dommodel.h"
#include "editortabswidget.h"
#include "editortab.h"
extern EditorTabsWidget *tabWidget;
extern DomItem *copy_item;
extern bool paste;

 DomModel::DomModel(QObject *parent)
     : QAbstractItemModel(parent)

 {
     rootItem = new DomItem(0, "Root");
     changed = false;

 }

 DomModel::~DomModel()
 {
     delete rootItem;
 }

 DomItem *DomModel::getRoot()
 {
     return rootItem;
 }

 int DomModel::columnCount(const QModelIndex &/*parent*/) const
 {
     return 3;
 }

 QVariant DomModel::data(const QModelIndex &index, int role) const
 {
     if (!index.isValid())
         return QVariant();

     if (role != Qt::DisplayRole)
         return QVariant();

     DomItem *item = itemForIndex(index);

     QString name = item->getName();
     QString type = item->getType();
     QString value = item->getValue();

     switch (index.column()) {
         case 0:
             return name;
         case 1:
             return type;
         case 2:
             return value;
         default:
             return QVariant();
     }
 }

 bool DomModel::setData(const QModelIndex & index, const QVariant & value, int role)
 {

     if(role){}


    if (!index.isValid()) return false;
    else
    {
        DomItem *item = itemForIndex(index);

        QString str = value.toString();

        switch (index.column())
        {
            case 0:
                item->setName(str);
                break;
            case 1:
                item->setType(str);
                break;
            case 2:
                item->setValue(str);
        };

        emit dataChanged(index, index);
        return true;
    }

 }

 Qt::ItemFlags DomModel::flags(const QModelIndex &index) const
 {
     if (!index.isValid())
         return 0;

     return Qt::ItemIsEnabled | Qt::ItemIsSelectable| Qt::ItemIsEditable;
 }

 QVariant DomModel::headerData(int section, Qt::Orientation orientation,
                               int role) const
 {
     if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
         switch (section) {
             case 0:
                 return tr("Key");
             case 1:
                 return tr("Type");
             case 2:
                 return tr("Value");
             default:
                 return QVariant();
         }
     }

     return QVariant();
 }

 QModelIndex DomModel::index(int row, int column, const QModelIndex &parent)
             const
 {
     if (!hasIndex(row, column, parent))
         return QModelIndex();

     DomItem *parentItem;

     if (!parent.isValid())
         parentItem = rootItem;
     else
         parentItem = itemForIndex(parent);

     DomItem *childItem = parentItem->child(row);

     if (childItem)
         return createIndex(row, column, childItem);
     else
         return QModelIndex();
 }

 QModelIndex DomModel::parent(const QModelIndex &child) const
 {
     if (!child.isValid()) return QModelIndex();

     DomItem *childItem = itemForIndex(child);
     DomItem *parentItem = childItem->parent();

     if (!parentItem || parentItem == rootItem)
         return QModelIndex();

     int row = parentItem->row();

     if (parentItem) return createIndex(row, 0, parentItem);

     return QModelIndex();
 }

 int DomModel::rowCount(const QModelIndex &parent) const
 {
     if (parent.column() > 0)
         return 0;

     DomItem *parentItem;

     if (!parent.isValid())
         parentItem = rootItem;
     else
         parentItem = itemForIndex(parent);

     return parentItem->childCount();
 }

 QModelIndex DomModel::addItem(const QModelIndex &parent, int row, ItemState *state)
 {

     if (parent.isValid())
     {
         QModelIndex index = this->index(parent.row(), 0, parent.parent());
         DomItem *item = itemForIndex(index);

         if (row == -1) row = item->childCount();
         DomItem * child = NULL;

         beginInsertRows(index, row, row);

         if (state != NULL)
         {
             child = state->getState()->clone();
         }
         child = item->addChild(row, child);

         endInsertRows();
         emit itemAdded(index);

         EditorTab *tab = tabWidget->getCurentTab();
         QTreeView *treeView = new QTreeView;
         treeView = (QTreeView*)tab->children().at(1);
         treeView->resizeColumnToContents(0);

         return this->index(child->row(), 0, index);

     }

    return QModelIndex();
 }

DomItem *DomModel::copyItem(const QModelIndex &parent)
 {
     QModelIndex index = this->index(parent.row(), 0, parent.parent());
     DomItem *item = itemForIndex(index)->clone();
     qDebug() << "被复制的条目" << item->getName() << "child个数" << item->childCount();
     return item;
 }

QModelIndex DomModel::pasteItem(const QModelIndex &parent, int row, ItemState *state)
 {

    if (parent.isValid())
    {
        const QModelIndex index = this->index(parent.row(), 0, parent.parent());
        DomItem *item = itemForIndex(index);//这里指父级
        qDebug() << item->getName() << index.data().toString();

        if (row == -1) row = item->childCount();

        DomItem * child = NULL;

        beginInsertRows(index, row, row);

        if (state != NULL)
        {
            child = state->getState()->clone();
            child->setName(child->getName() + "--" + QTime::currentTime().toString());

        }

        child = item->addChild(row, child);
        child->setData(copy_item->getName() + "--" + QTime::currentTime().toString(), copy_item->getType(), copy_item->getValue());

        DomItem *item0 = NULL;
        DomItem *item1 = NULL;
        DomItem *item2 = NULL;
        DomItem *item3 = NULL;
        DomItem *item4 = NULL;
        DomItem *sub1 = NULL;
        DomItem *sub2 = NULL;
        DomItem *sub3 = NULL;
        DomItem *sub4 = NULL;

        for(int i = 0; i < copy_item->childCount(); i ++)
        {
            QString name = copy_item->childItems.at(i)->getName();
            QString type = copy_item->childItems.at(i)->getType();
            QString value = copy_item->childItems.at(i)->getValue();
            item0 = NULL;
            item0 = child->addChild(row + i, item0);
            item0->setData(name, type, value); //先增加，再设置数据

            sub1 = copy_item->childItems.at(i);
            if(sub1->childCount() > 0)
            {
                for(int j = 0; j < sub1->childCount(); j ++)
                {
                    QString name = sub1->childItems.at(j)->getName();
                    QString type = sub1->childItems.at(j)->getType();
                    QString value = sub1->childItems.at(j)->getValue();
                    item1 = NULL;
                    item1 = item0->addChild(j, item1);
                    item1->setData(name, type, value); //先增加，再设置数据

                    sub2 = sub1->childItems.at(j);
                    if(sub2->childCount() > 0)
                    {
                        for(int k = 0; k < sub2->childCount(); k ++)
                        {
                            QString name = sub2->childItems.at(k)->getName();
                            QString type = sub2->childItems.at(k)->getType();
                            QString value = sub2->childItems.at(k)->getValue();
                            item2 = NULL;
                            item2 = item1->addChild(k, item2);
                            item2->setData(name, type, value); //先增加，再设置数据

                            sub3 = sub2->childItems.at(k);
                            if(sub3->childCount() > 0)
                            {
                                for(int l = 0; l < sub3->childCount(); l ++)
                                {
                                    QString name = sub3->childItems.at(l)->getName();
                                    QString type = sub3->childItems.at(l)->getType();
                                    QString value = sub3->childItems.at(l)->getValue();
                                    item3 = NULL;
                                    item3 = item2->addChild(l, item3);
                                    item3->setData(name, type, value); //先增加，再设置数据

                                    sub4 = sub3->childItems.at(l);
                                    if(sub4->childCount() > 0)
                                    {
                                        for(int n = 0; n < sub4->childCount(); n ++)
                                        {
                                            QString name = sub4->childItems.at(n)->getName();
                                            QString type = sub4->childItems.at(n)->getType();
                                            QString value = sub4->childItems.at(n)->getValue();
                                            item4 = NULL;
                                            item4 = item3->addChild(n, item4);
                                            item4->setData(name, type, value); //先增加，再设置数据

                                        }
                                    }

                                }
                            }
                        }

                    }
                }


            }
        }


        endInsertRows();
        emit itemAdded(index);

        EditorTab *tab = tabWidget->getCurentTab();
        QTreeView *treeView = new QTreeView;
        treeView = (QTreeView*)tab->children().at(1);
        treeView->resizeColumnToContents(0);

        //this->resetInternalData();

        return this->index(child->row(), 0, index);

    }


    return QModelIndex();
 }

void DomModel::removeItem(const QModelIndex &index)
 {

    if (index.isValid())
     {
         DomItem *item = itemForIndex(index);
         int row = index.row();
         QModelIndex parent = index.parent();
         beginRemoveRows(parent, row, row);
         item->removeFromParent(index.row());
         endRemoveRows();

     }

     /*EditorTab *tab = tabWidget->getCurentTab();
     QTreeView *treeView = new QTreeView;
     treeView = (QTreeView*)tab->children().at(1);
     treeView->resizeColumnToContents(0);*/


 }

 ItemState *DomModel::saveItemState(const QModelIndex &index)
 {
     return new ItemState(itemForIndex(index));
 }

 DomItem *DomModel::itemForIndex(const QModelIndex &index)
 {
     return static_cast<DomItem*>(index.internalPointer());
 }

 bool DomModel::itemNotPlist(const QModelIndex &index)
 {
    return itemForIndex(QModelIndex(index))->getName() != "plist";
 }


