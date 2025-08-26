/*
  cpp-playground - C++ experiments and learning playground
  Copyright (C) 2025 M. Reza Dwi Prasetiawan


  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program. If not, see <https://www.gnu.org/licenses/>.
*/


#include <QApplication>
#include <QScreen>

#include "GLwidget.hxx"

int main(int argc, char** argv) {
  QApplication app(argc, argv);

  // Optional: request OpenGL 3.3 core
  QSurfaceFormat fmt;
  fmt.setVersion(3, 3);
  fmt.setProfile(QSurfaceFormat::CoreProfile);
  QSurfaceFormat::setDefaultFormat(fmt);

  // Ambil ukuran layar utama
  QScreen* screen       = QGuiApplication::primaryScreen();
  int      screenWidth  = screen->geometry().width();
  int      screenHeight = screen->geometry().height();

  // Buat window dengan ukuran full screen (atau bisa ¾ layar)
  GLwidget w;
  w.resize(screenWidth, screenHeight);
  w.show();
  return app.exec();
}