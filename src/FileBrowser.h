#ifndef	__FILEBROWSER_H__
#define __FILEBROWSER_H__
/*
 * This file is (or was, at some point) part of the QBoard project
 * (http://code.google.com/p/qboard)
 *
 * Copyright (c) 2008 Stephan Beal (http://wanderinghorse.net/home/stephan/)
 *
 * This file may be used under the terms of the GNU General Public
 * License versions 2.0 or 3.0 as published by the Free Software
 * Foundation and appearing in the files LICENSE.GPL2 and LICENSE.GPL3
 * included in the packaging of this file.
 *
 */

#include <QString>
#include <QListWidget>
#include <QFileInfo>
#include <QDir>
class QWidget;
class QListWidgetItem;
class QFileIconProvider;
/**
	A very basic file browser widget for Qt4,
	based off of older Qt3 example code from the Qt website:

	http://doc.trolltech.com/qq/qq09-file-browser.html
*/
class FileBrowser : public QListWidget
{
Q_OBJECT
public:
	/**
	Sets the filename filter and parent widgets.
	*/
	FileBrowser(const QString &filter, QWidget *parent = 0);
	virtual ~FileBrowser();
public Q_SLOTS:
	/**
		Sets the dir view to path (does not actually change
		the working directory).
	*/
	void setDir(const QString &path);
	/**
		Re-reads whatever directory was set last via setDir().
	*/
	void reloadDir();
    QFileIconProvider * iconProvider();
Q_SIGNALS:
	/**
		Triggered when a file is selected.
	*/
	void pickedFile(const QFileInfo &);
	/**
		Triggered when a directory is selected.
	*/
	void pickedDir(const QDir & dir);
  
  private Q_SLOTS:
	void somethingSelected(QListWidgetItem*);
private:
    struct Impl;
    Impl * impl;
};
#endif // __FILEBROWSER_H__
