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


#pragma once
#include <QLabel>
#include <QMainWindow>
#include <QPushButton>
#include <QVBoxLayout>

class Window : public QMainWindow {
  Q_OBJECT
  QLabel      *label;
  QPushButton *button;

 public:
  Window(QWidget *parent = nullptr) : QMainWindow(parent) {
    // Inisialisasi widget
    label  = new QLabel("Halo Qt", this);
    button = new QPushButton("Klik Saya", this);

    // Layout
    QWidget     *central = new QWidget(this);
    QVBoxLayout *layout  = new QVBoxLayout(central);
    layout->addWidget(label);
    layout->addWidget(button);
    central->setLayout(layout);
    setCentralWidget(central);

    // Event handler
    connect(button, &QPushButton::clicked, this, [this]() { label->setText("Tombol diklik!"); });
  }
};
