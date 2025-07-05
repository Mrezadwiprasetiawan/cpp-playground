#pragma once
#include <QLabel>
#include <QMainWindow>
#include <QPushButton>
#include <QVBoxLayout>

class Window : public QMainWindow {
  Q_OBJECT
  QLabel *label;
  QPushButton *button;

 public:
  Window(QWidget *parent = nullptr) : QMainWindow(parent) {
    // Inisialisasi widget
    label = new QLabel("Halo Qt", this);
    button = new QPushButton("Klik Saya", this);

    // Layout
    QWidget *central = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(central);
    layout->addWidget(label);
    layout->addWidget(button);
    central->setLayout(layout);
    setCentralWidget(central);

    // Event handler
    connect(button, &QPushButton::clicked, this, [this]() { label->setText("Tombol diklik!"); });
  }
};
