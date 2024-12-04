st = """
            QMainWindow {
                background-color: #2c3e50;
                color: #ecf0f1;
            }
            QGroupBox {
                font-size: 30px;
                font-weight: bold;
                color: #ecf0f1;
                border: 2px solid #34495e;
                padding: 15px;
                margin-top: 10px;
                border-radius: 8px;
            }
            QLabel {
                font-size: 30px;
                color: #bdc3c7;
            }
            QPushButton {
                font-size: 30px;
                padding: 10px;
                color: #ffffff;
                background-color: #1abc9c;
                border-radius: 5px;
                margin-right: 15px;
            }
            QPushButton:hover {
                background-color: #16a085;
            }
            QComboBox, QSpinBox, QLineEdit {
                padding: 6px;
                font-size: 30px;
                border: 1px solid #34495e;
                border-radius: 5px;
            }
            QProgressBar {
                background-color: #34495e;
                color: #ffffff;
                border: 1px solid #1abc9c;
                border-radius: 5px;
                text-align: center;
                font-weight: bold;
            }
            QProgressBar::chunk {
                background-color: #1abc9c;
            }
            QTabWidget::pane {
                border: 1px solid #34495e;
                border-radius: 8px;
            }
            QTabBar::tab {
                padding: 20px;
                min-width: 200px;
                min-height: 20px;
                color: #ecf0f1;
                background-color: #2c3e50;
                font-size: 10pt;  /* Aumenta el tamaño de la fuente aquí */
                border: 1px solid #1abc9c;
                border-radius: 5px;    
            }
            QTabBar::tab:selected {
                background-color: #1abc9c;
                color: #2c3e50;
            }
            QStatusBar {
                /*color: #f39c12;  Change the color of the status message */
                font-weight: bold; /* Optional: make the message bold */
                font-size: 10pt;
                background-color: #1abc9c; 
                color: #2c3e50;
            }
        """