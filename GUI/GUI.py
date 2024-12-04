import os
import sys
import subprocess
from PyQt5.QtWidgets import (
    QApplication, QMainWindow, QVBoxLayout, QHBoxLayout, QWidget,
    QPushButton, QLabel, QComboBox, QSpinBox, QGroupBox, QProgressBar, 
    QTabWidget, QSlider, QDoubleSpinBox, QLineEdit, QAction, QMessageBox
)
from PyQt5.QtCore import Qt, QTimer, QSize
from PyQt5.QtGui import QWindow, QDoubleValidator, QPixmap
import pygetwindow as gw
import win32gui
from style import st

import webbrowser
import math

class SimulationApp(QMainWindow):
    def __init__(self):
        super().__init__()

        self.setWindowTitle("Advanced Geant4 Simulator")
        self.setGeometry(100, 100, 1400, 800)

        self.setStyleSheet(st)

                # Agregar barra de menú
        self.menu_bar = self.menuBar()

        # Crear un menú "Help"
        help_menu = self.menu_bar.addMenu("Help")

        # Agregar acciones al menú "Help"
        help_action = QAction("User Guide", self)
        help_action.triggered.connect(self.show_help)
        help_menu.addAction(help_action)

        about_action = QAction("About", self)
        about_action.triggered.connect(self.show_about)
        help_menu.addAction(about_action)

        # Main layout
        main_layout = QHBoxLayout()

        # Sidebar
        sidebar = QVBoxLayout()

        # Panel de Configuración
        config_panel = QGroupBox("Configuration")
        config_layout = QVBoxLayout()

        config_layout.addWidget(QLabel("Simulation Type:"))
        self.sim_type = QComboBox()
        self.sim_type.addItems(["Radiography", "CT", "Attenuation", "Characterization"])
        self.sim_type.currentIndexChanged.connect(self.update_initialization_panel)
        config_layout.addWidget(self.sim_type)

        config_layout.addWidget(QLabel("Spectrum:"))
        self.spectrum_type = QComboBox()
        self.spectrum_type.addItems(["Monochromatic", "Real"])
        config_layout.addWidget(self.spectrum_type)

        config_layout.addWidget(QLabel("Photon Resolution:"))
        
        # Slider de resolución de fotones
        self.photon_slider = QSlider(Qt.Horizontal)
        self.photon_slider.setRange(0, 5)
        self.photon_slider.setSingleStep(1)
        self.photon_slider.setValue(0)
        self.photon_slider.valueChanged.connect(self.update_photon_label)

        # Layout horizontal para slider y etiquetas de extremos
        photon_slider_layout = QHBoxLayout()
        low_label = QLabel("Low")
        high_label = QLabel("High")
        photon_slider_layout.addWidget(low_label)
        photon_slider_layout.addWidget(self.photon_slider)
        photon_slider_layout.addWidget(high_label)
        config_layout.addLayout(photon_slider_layout)

        # Etiqueta dinámica para mostrar la resolución de fotones
        self.photon_label = QLabel("Photons: 1 million")
        config_layout.addWidget(self.photon_label)
        # Botón de simulación
        self.max_energy_value = 100.0
        self.min_energy_value = 0.0
        self.step_size_value = 1.0

        self.start_simulation_button = QPushButton("Start Simulation")
        self.start_simulation_button.clicked.connect(self.run_simulation)
        # self.start_simulation_button.clicked.connect(self.change_image_in_results)
        # self.start_simulation_button.clicked.connect(self.show_simulation_time)
        # self.start_simulation_button.clicked.connect(self.change_image_in_results)
        config_layout.addWidget(self.start_simulation_button)

        # Ruta del ejecutable Sim.exe
        self.sim_path = r'C:\Users\conej\Documents\Universidad\Geant4\Gui\GUI-final\G4Snippets\RadCT\Thorax\Main\MedX_Engine\build\Release\Sim.exe'

        # Botón de visualización
        self.start_visualization_button = QPushButton("Start Visualization")
        self.start_visualization_button.clicked.connect(self.run_visualization)
        config_layout.addWidget(self.start_visualization_button)

        config_panel.setLayout(config_layout)
        sidebar.addWidget(config_panel)

        # Panel de Initialization
        self.init_panel = QGroupBox("Initialization")
        self.init_layout = QVBoxLayout()
        self.init_panel.setLayout(self.init_layout)
        sidebar.addWidget(self.init_panel)

        # Barra de progreso
        self.progress_bar = QProgressBar()
        self.progress_bar.setValue(0)
        sidebar.addWidget(self.progress_bar)
        sidebar.setAlignment(self.progress_bar, Qt.AlignBottom)

        main_layout.addLayout(sidebar, 1)

        # Pestañas de visualización y resultados
        self.tab_widget = QTabWidget()
        self.visualization_tab = QWidget()

        self.visualization_tab.setStyleSheet("""
            QWidget {
                background-image: url('Medx_logo.png');
                background-repeat: no-repeat;
                background-position: center;
                background-clip: padding;
            }
        """)
        visualization_layout = QVBoxLayout()
        self.visualization_container = QWidget()
        visualization_layout.addWidget(self.visualization_container)
        self.visualization_tab.setLayout(visualization_layout)
        self.tab_widget.addTab(self.visualization_tab, "Visualization")


        # self.results_tab = QWidget()
        # # self.results_layout = QVBoxLayout()
        # # self.results_layout.addWidget(QLabel("Results will be displayed here."))
        # # self.results_tab.setLayout(self.results_layout)
        # self.tab_widget.addTab(self.results_tab, "Results")

        # Crear la pestaña de resultados
        self.results_tab = QWidget()
        # Crear un QLabel que contendrá la imagen
        self.image_label = QLabel(self.results_tab)
        self.image_label.setAlignment(Qt.AlignCenter)  # Centrar la imagen dentro del QLabel
        # Establecer un tamaño fijo para la imagen
        self.fixed_image_size = QSize(600*2, 400*2)  # Puedes ajustar el tamaño deseado aquí
        # Agregar el QLabel a la pestaña de resultados
        self.results_layout = QVBoxLayout(self.results_tab)
        self.results_layout.addWidget(self.image_label)
        # Añadir la pestaña de resultados al QTabWidget
        self.tab_widget.addTab(self.results_tab, "Results")
        self.results_tab.setStyleSheet("""  
            QWidget {
                background-image: url('not.png');
                background-repeat: no-repeat;
                background-position: center;
                background-clip: padding;
            }
        """)  



        main_layout.addWidget(self.tab_widget, 3)
        
        # Configuración principal
        central_widget = QWidget()
        central_widget.setLayout(main_layout)
        self.setCentralWidget(central_widget)
        self.statusBar().showMessage("Ready to start simulation")

    #---------------------------------------------------------------------------------------------#
    #---------------------------------------Temporizadores----------------------------------------#
    #---------------------------------------------------------------------------------------------#

        # Temporizador para ventanas
        self.check_windows_timer = QTimer()
        self.check_windows_timer.timeout.connect(self.check_open_windows)

        # Temporizador de progreso
        self.progress_timer = QTimer()
        self.progress_timer.timeout.connect(self.update_progress)
        # self.progress_timer.timeout.connect(self.advance_progress)

        # Contador de progreso
        self.progress_value = 0

        self.sim_time = 1

    #---------------------------------------------------------------------------------------------#
    #---------------------------------Flags y variables de estado---------------------------------#
    #---------------------------------------------------------------------------------------------#

        # Flags para ventanas
        self.sim_hidden = False
        self.viewer_contained = False
        self.sim_running = False  # Indicador para rastrear si Sim.exe está corriendo
        # self.process = None  # Referencia al proceso de Sim.exe
        self.force_complete_progress = False  # Inicialmente está en False
        # Flags de estado sim type
        self.past_st = 0
        self.current_st = 0
        # Flags de estado model type
        self.Mpast_st = 0
        self.Mcurrent_st = 0
        # self.models = QComboBox()
        self.dexa = False
  
        self.update_initialization_panel()
        self.showMaximized()

        # photonN = self.photon_slider.value()
        # self.statusBar().showMessage(str(photonN))
        # dexa =  self.mode_toggle.isChecked()
        # self.statusBar().showMessage(str(dexa))



    # def run_simulation(self):
    #     # Reutilizar este método para simulación
    #     self.execute_geant4("simulation")

    #---------------------------------------------------------------------------------------------#
    #---------------------------------------Help Guide--------------------------------------------#
    #---------------------------------------------------------------------------------------------#
    def show_help(self):
        # Mostrar un mensaje de ayuda
        QMessageBox.information(
            self, 
            "User Guide", 
            "This is where you can provide detailed help information.\n\n"
            "For more details, visit the Geant4 forum."
        )
        
        # Abrir el sitio web en el navegador
        webbrowser.open("https://geant4-forum.web.cern.ch/")

    def show_about(self):
        # Lógica para mostrar información 'About'
        QMessageBox.information(
            self, "About", "Advanced Geant4 Simulator\nVersion 1.0 - h4xter\n© 2024"
        )

    #---------------------------------------------------------------------------------------------#
    #-------------------------------------Config Panel--------------------------------------------#
    #---------------------------------------------------------------------------------------------#
    def update_photon_label(self, value):
            # Diccionario para mapear el valor del slider con las etiquetas de fotones
            photon_values = {
                0: "Photons: 1 million",
                1: "Photons: 10 million",
                2: "Photons: 100 million",
                3: "Photons: 1K million",
                4: "Photons: 10K million",
                5: "Photons: 100K million",
            }
            self.photon_label.setText(photon_values.get(value, "Photons: Undefined"))
    #---------------------------------------------------------------------------------------------#
    #-------------------------------------Init Panel----------------------------------------------#
    #---------------------------------------------------------------------------------------------#    

    def update_initialization_panel(self):
        """Update the Initialization panel based on the selected simulation type."""
        sim_type = self.sim_type.currentText()

        # Eliminar todos los widgets del layout existente
        while self.init_layout.count():
            child = self.init_layout.takeAt(0)
            if child.widget():
                child.widget().deleteLater()

        # Agregar widgets según el tipo de simulación seleccionado
        if sim_type == "Radiography":
            self.sim_path = r'C:\Users\conej\Documents\Universidad\Geant4\Gui\GUI-final\G4Snippets\RadCT\Thorax\Main\MedX_Engine\build\Release\Sim.exe'
            self.init_layout.addWidget(QLabel("3D Models:"))
            self.models = QComboBox()
            self.models.addItems(["Thorax", "Arm"])
            self.models.currentIndexChanged.connect(self.update_model_vis)
            self.init_layout.addWidget(self.models)
            
            self.init_layout.addWidget(QLabel("Ailment:"))
            ailments = QComboBox()
            ailments.addItems(["Normal", "Osteoporosis", "Tumor"])
            self.init_layout.addWidget(ailments)
            self.current_st = 0

            # Agregar la etiqueta "Mode:"
            self.init_layout.addWidget(QLabel("Mode:"))

            # Crear la palanca como un botón checkable
            self.mode_toggle = QPushButton("Single")
            self.mode_toggle.setCheckable(True)
            self.mode_toggle.setStyleSheet("""
                QPushButton {
                    background-color: #1abc9c;
                    border: 1px solid gray;
                    border-radius: 10px;
                    padding: 5px;
                    min-width: 80px;
                }
                QPushButton:checked {
                    background-color: #f39c12;
                    border: 1px solid blue;
                }
            """)

            # Conectar el cambio de estado al método para actualizar el texto
            self.mode_toggle.toggled.connect(self.update_mode_text)

            # Añadir la palanca al layout
            self.init_layout.addWidget(self.mode_toggle)


            if self.current_st == self.past_st:
                self.start_visualization_button.setStyleSheet("background-color: #1abc9c;")
                

        elif sim_type == "CT":
            self.init_layout.addWidget(QLabel("3D Models:"))
            self.models = QComboBox()
            self.models.addItems(["Thorax", "Arm"])
            self.models.currentIndexChanged.connect(self.update_model_vis)
            self.init_layout.addWidget(self.models)

            self.init_layout.addWidget(QLabel("Ailment:"))
            ailments = QComboBox()
            ailments.addItems(["Normal", "Osteoporosis", "Tumor"])
            self.init_layout.addWidget(ailments)

            # self.init_layout.addWidget(QLabel("Resolution:"))
            # step_size = QSlider(Qt.Horizontal)
            # step_size.setRange(0, 5)
            # step_size.setSingleStep(1)
            # step_size.setValue(1)
            # # step_size.setSuffix(" mm")
            # self.init_layout.addWidget(step_size)

            # Etiqueta para la resolución
            self.init_layout.addWidget(QLabel("Resolution:"))

            # Crear el slider
            self.step_size = QSlider(Qt.Horizontal)
            self.step_size.setRange(0, 5)  # Establecer el rango de valores (Low a High)
            self.step_size.setSingleStep(1)  # Establecer el paso del slider
            self.step_size.setValue(3)  # Establecer el valor inicial del slider
            self.step_size.valueChanged.connect(self.update_label)  # Conectar el cambio de valor a una función

            # Crear layout horizontal para el slider y las etiquetas
            slider_layout = QHBoxLayout()

            # Etiquetas para los extremos
            low_label = QLabel("Low")
            high_label = QLabel("High")

            # Etiquetas al mismo nivel que el slider
            slider_layout.addWidget(low_label)
            slider_layout.addWidget(self.step_size)
            slider_layout.addWidget(high_label)

            # Añadir layout con el slider y las etiquetas al layout principal
            self.init_layout.addLayout(slider_layout)

            # Crear etiqueta para mostrar el valor seleccionado
            # self.value_label = QLabel(f"Angle step: {step_size.value()}")
            self.value_label = QLabel(f"Angle step: 1 deg")

            # Añadir la etiqueta que muestra el valor
            self.init_layout.addWidget(self.value_label)

            # Configurar el layout de la ventana
            self.setLayout(self.init_layout)

            self.current_st = 1
            if self.current_st == self.past_st:
                self.start_visualization_button.setStyleSheet("background-color: #1abc9c;")
            self.sim_path = r'C:\Users\conej\Documents\Universidad\Geant4\Gui\GUI-final\G4Snippets\RadCT\Thorax\Main\MedX_Engine\build\Release\Sim.exe'

        elif sim_type == "Attenuation":
            # self.start_simulation_button.clicked.connect(self.read_energy_values)

            self.sim_path = r'C:\Users\conej\Documents\Universidad\Geant4\Gui\GUI-final\G4Snippets\Att\Main\MedX_Engine\build\Release\Sim.exe'
            self.init_layout.addWidget(QLabel("Material:"))
            material = QComboBox()
            material.addItems(["Aluminum", "Tungsten", "Bone"])
            self.init_layout.addWidget(material)

            self.init_layout.addWidget(QLabel("Energy (keV) Range:"))
            
            # Crear layout horizontal para min y max energy
            energy_layout = QHBoxLayout()

            # Campo de texto para min energy
            min_energy_label = QLabel("Min:")
            self.min_energy = QLineEdit()
            self.min_energy.setPlaceholderText("0.0")  # Placeholder para indicar un ejemplo
            self.min_energy.setValidator(QDoubleValidator(0.0, 500.0, 2))  # Validador de números flotantes

            # Campo de texto para max energy
            max_energy_label = QLabel("Max:")
            self.max_energy = QLineEdit()
            self.max_energy.setPlaceholderText("100.0")  # Placeholder para indicar un ejemplo
            self.max_energy.setValidator(QDoubleValidator(0.0, 500.0, 2))  # Validador de números flotantes

            # Añadir widgets al layout horizontal
            energy_layout.addWidget(min_energy_label)
            energy_layout.addWidget(self.min_energy)
            energy_layout.addWidget(max_energy_label)
            energy_layout.addWidget(self.max_energy)

            # Añadir el layout de energía al layout principal
            self.init_layout.addLayout(energy_layout)

            # Campo para definir Step Size
            self.init_layout.addWidget(QLabel("Step Size (keV):"))
            self.step_size = QLineEdit()
            self.step_size.setPlaceholderText("0.5")
            self.step_size.setValidator(QDoubleValidator(0.0, 50.0, 3))  # Validador para valores flotantes
            self.init_layout.addWidget(self.step_size)

            # Configurar el layout de la ventana
            self.setLayout(self.init_layout)

            self.current_st = 2
            if self.current_st == self.past_st:
                self.start_visualization_button.setStyleSheet("background-color: #1abc9c;")


        elif sim_type == "Characterization":
            self.init_layout.addWidget(QLabel("Source:"))
            source = QComboBox()
            source.addItems(["Tungsten", "Copper"])
            self.init_layout.addWidget(source)

            self.init_layout.addWidget(QLabel("Detector:"))
            detector = QComboBox()
            detector.addItems(["Silicon", "Medipix"])
            self.init_layout.addWidget(detector)
            self.sim_path = r'C:\Users\conej\Documents\Universidad\Geant4\Gui\GUI-final\G4Snippets\Char\Main\build\Release\PCM.exe'
            self.current_st = 3
            if self.current_st == self.past_st:
                self.start_visualization_button.setStyleSheet("background-color: #1abc9c;")
            

        # Cambio color para visualization
        if self.past_st != self.current_st and self.sim_running:
            self.start_visualization_button.setStyleSheet("background-color: #da1633;")
        elif self.past_st == self.current_st and self.sim_running:
            self.start_visualization_button.setStyleSheet("background-color: #f39c12;")
            self.past_st = self.current_st
        else:
            self.past_st = self.current_st
            # self.past_st = self.current_st
        # elif self.sim_running:
        #     self.start_visualization_button.setStyleSheet("background-color: #1abc9c;")
        # else:
        #     self.past_st = True
        # else:
        #     self.start_visualization_button.setStyleSheet("background-color: #f39c12;")


        # Asegúrate de que los cambios sean visibles
        self.init_panel.setLayout(self.init_layout)
    
    def update_model_vis(self):
        model_type = self.models.currentText()
        if model_type == "Thorax":
            self.Mcurrent_st = 0
            if self.Mcurrent_st == self.Mpast_st:
                self.start_visualization_button.setStyleSheet("background-color: #1abc9c;")
            self.sim_path = r'C:\Users\conej\Documents\Universidad\Geant4\Gui\GUI-final\G4Snippets\RadCT\Thorax\Main\MedX_Engine\build\Release\Sim.exe'
        else:
            self.Mcurrent_st = 1
            if self.Mcurrent_st == self.Mpast_st:
                self.start_visualization_button.setStyleSheet("background-color: #1abc9c;")
            self.sim_path = r'C:\Users\conej\Documents\Universidad\Geant4\Gui\GUI-final\G4Snippets\RadCT\Arm\Main\MedX_Engine\build\Release\Sim.exe'
        if self.Mpast_st != self.Mcurrent_st and self.sim_running:
            self.start_visualization_button.setStyleSheet("background-color: #da1633;")
        elif self.Mpast_st == self.Mcurrent_st and self.sim_running:
            self.start_visualization_button.setStyleSheet("background-color: #f39c12;")
            self.Mpast_st = self.Mcurrent_st
        else:
            self.Mpast_st = self.Mcurrent_st

    def update_label(self, value):
        # Diccionario para mapear los valores del slider a las cadenas deseadas
        angle_steps = {
            0: "Angle step: 10 deg",
            1: "Angle step: 5 deg",
            2: "Angle step: 2 deg",
            3: "Angle step: 1 deg",
            4: "Angle step: 0.5 deg",
            5: "Angle step: 0.25 deg"
        }

        # Actualizar la etiqueta con el valor correspondiente
        self.value_label.setText(angle_steps.get(value, "Unknown value"))
    # Método para actualizar el texto de la palanca
    def update_mode_text(self, checked):
        if checked:
            self.mode_toggle.setText("DEXA")
            self.dexa = True
        else:
            self.mode_toggle.setText("Single")
            self.dexa = False

    # Método para leer los valores de min_energy y max_energy
    def read_energy_values(self):
        # Leer el valor de min_energy
        min_energy_text = self.min_energy.text()
        if min_energy_text:  # Verificar que no esté vacío
            try:
                min_energy = float(min_energy_text)  # Convertir a float
                # print(f"Min Energy: {min_energy}")
            except ValueError:
                # print("El valor de Min Energy no es válido.")
                min_energy = 0.0
        else:
            # print("Min Energy no fue ingresado.")
            min_energy = 0.0

        # Leer el valor de max_energy
        max_energy_text = self.max_energy.text()
        if max_energy_text:  # Verificar que no esté vacío
            try:
                max_energy = float(max_energy_text)  # Convertir a float
                # print(f"Max Energy: {max_energy}")
            except ValueError:
                # print("El valor de Max Energy no es válido.")
                max_energy = 100.0
        else:
            # print("Max Energy no fue ingresado.")
            max_energy = 100.0

        # Leer step-size
        step_size_text = self.step_size.text()
        if step_size_text :  # Verificar que no esté vacío
            try:
                ss = float(step_size_text)  # Convertir a float
                # print(f"Step-size: {ss}")
            except ValueError:
                # print("El valor step size no es valido.")
                ss = 1.0
        else:
            # print("Step Size no fue ingresado.")
            ss = 1.0

        # Guardar los valores en variables (puedes usarlas más adelante)
        # Si quieres guardarlos en atributos de la clase, puedes hacer:
        self.min_energy_value = min_energy
        self.max_energy_value = max_energy
        self.step_size_value = ss 
    #---------------------------------------------------------------------------------------------#
    #-------------------------------------Visualization-------------------------------------------#
    #---------------------------------------------------------------------------------------------#
    def run_visualization(self):
        # Abris vis.mac
        self.current_st = self.current_st
        if self.sim_running:
            # Cambiar el texto y el color a verde para "Start Visualization"
            self.start_visualization_button.setStyleSheet("background-color: #1abc9c;")
            self.start_visualization_button.setText("Start Visualization")
            self.past_st = self.current_st
            self.Mpast_st = self.Mcurrent_st
            self.stop_visualization()
        else:
            # Iniciar temporizador para avanzar la barra de progreso
            self.progress_timer.start(200)  # Actualizar cada N ms
            # Cambiar el texto y el color a amarillo para "Stop Visualization"
            self.start_visualization_button.setStyleSheet("background-color: #f39c12;")
            self.start_visualization_button.setText("Stop Visualization")
            self.execute_geant4("visualization")
        
    def execute_geant4(self, mode):
        # Desactivar el botón para evitar que el usuario haga clic mientras se está ejecutando
        self.start_visualization_button.setEnabled(False)
               
        # Cambiar al directorio de Sim.exe
        sim_dir = os.path.dirname(self.sim_path)
        os.chdir(sim_dir)
        
        # Ejecutar Sim.exe
        self.process = subprocess.Popen([self.sim_path])
        print(f"Sim.exe (PID: {self.process.pid}) ejecutado en modo: {mode}")
        
        # Cambiar el mensaje de la barra de estado
        self.statusBar().showMessage(f"{mode.capitalize()} Loading...")
        
        # Iniciar temporizador para buscar ventanas
        self.check_windows_timer.start(100)  # Revisar cada 1 segundo


    def check_open_windows(self):
        # Obtener ventanas abiertas usando pygetwindow
        open_windows = gw.getWindowsWithTitle("")
        visible_windows = [window for window in open_windows if window.visible]

        # Ocultar la ventana de Sim.exe si está abierta y aún no se ha ocultado
        if not self.sim_hidden:
            self.hide_sim_window()

        # Buscar y contener la ventana del viewer si aún no se ha contenido
        if not self.viewer_contained:
            self.contain_viewer_window(visible_windows)

        # Detener el temporizador si ambas ventanas han cumplido su función
        if self.sim_hidden and self.viewer_contained:
            self.check_windows_timer.stop()
            self.statusBar().showMessage("All windows processed successfully.")
            self.sim_running = True
            self.on_simulation_complete()  # Llamar a la función cuando termine la simulación
            # Activar la bandera para forzar el progreso a 100
            self.force_complete_progress = True
            # Actualizar la barra de progreso al 100%
            self.progress_bar.setValue(100)

    def hide_sim_window(self):
        # Buscar la ventana de Sim.exe
        sim_windows = gw.getWindowsWithTitle("Sim.exe")
        sim_windows1 = gw.getWindowsWithTitle("PCM.exe")
        for window in sim_windows or sim_windows1:
            hwnd = window._hWnd  # Handle de la ventana
            print(f"Ocultando ventana: {window.title}")

            # Ocultar la ventana
            win32gui.ShowWindow(hwnd, 0)  # SW_HIDE = 0
            self.sim_hidden = True
            self.statusBar().showMessage("Sim.exe window hidden successfully.")

            # Actualizar la barra de progreso
            self.progress_bar.setValue(50)
            return

    def contain_viewer_window(self, visible_windows):
        # Buscar la ventana del viewer y contenerla
        for window in visible_windows:
            if "viewer-0 (TOOLSSG_WINDOWS_ZB)" in window.title:
                hwnd = window._hWnd  # Handle de la ventana
                print(f"Conteniendo ventana del viewer: {window.title}")

                try:
                    qwindow = QWindow.fromWinId(hwnd)
                    qwindow.setFlags(Qt.FramelessWindowHint)  # Eliminar bordes de la ventana

                    # Crear un contenedor para la ventana
                    container = QWidget.createWindowContainer(qwindow, self.visualization_container)

                    # Configurar el layout si no existe
                    if not self.visualization_container.layout():
                        layout = QVBoxLayout()
                        self.visualization_container.setLayout(layout)

                    # Agregar el contenedor al layout
                    self.visualization_container.layout().addWidget(container)

                    self.viewer_contained = True
                    self.statusBar().showMessage("Viewer window contained successfully.")

                    # Actualizar la barra de progreso
                    self.progress_bar.setValue(75)
                    return
                except Exception as e:
                    print(f"Error al contener la ventana del viewer: {e}")
                    self.statusBar().showMessage(f"Error al contener la ventana del viewer: {str(e)}")

    def stop_visualization(self):
        if self.sim_running:
            try:
                # Terminar el proceso Sim.exe si está en ejecución
                if self.process:
                    self.process.terminate()
                    self.process.wait()  # Esperar a que el proceso termine
                    print("Sim.exe terminado.")
                    self.process = None

                # Limpiar contenedores de visualización
                for i in reversed(range(self.visualization_container.layout().count())):
                    widget = self.visualization_container.layout().itemAt(i).widget()
                    if widget:
                        widget.setParent(None)  # Desasignar el widget del contenedor

                # Restablecer indicadores de estado
                self.viewer_contained = False
                self.sim_hidden = False
                self.sim_running = False

                self.statusBar().showMessage("Visualization stopped and containers cleaned.")

                # Actualizar la barra de progreso
                self.progress_bar.setValue(0)
                self.force_complete_progress = False
                self.progress_value = 0

            except Exception as e:
                self.statusBar().showMessage(f"Error al detener Sim.exe: {str(e)}")

    def on_simulation_complete(self):
        # Habilitar el botón cuando la simulación termine
        self.start_visualization_button.setEnabled(True)
    
    def update_progress(self):
        # Revisar si la bandera está activada
        if self.force_complete_progress:
            self.progress_value = 100
            self.progress_bar.setValue(self.progress_value)
            self.progress_timer.stop()  # Detener el temporizador
        elif self.progress_value < 43:
            self.progress_value += 1
            self.progress_bar.setValue(self.progress_value)
        else:
            self.progress_timer.stop()  # Detener el temporizador cuando se llegue al 100%

    #---------------------------------------------------------------------------------------------#
    #-------------------------------------Sim config----------------------------------------------#
    #---------------------------------------------------------------------------------------------#    
    def run_simulation(self):
        # sim_type = self.sim_type.currentText()
        self.force_complete_progress = False
        self.progress_value = 0
        self.progress_bar.setValue(0)
        # Cambiar el texto y el color a verde para "Start Visualization"
        self.start_visualization_button.setStyleSheet("background-color: #1abc9c;")
        self.start_visualization_button.setText("Start Visualization")
        self.sim_time = 1
        sp_type = self.spectrum_type.currentText()
        photonN = self.photon_slider.value()
        
        #self.Mcurrent_st = 0 self.current_st = 0
        if self.current_st == 0:
            if sp_type == "Real":
                self.sim_time = self.sim_time*4/3/3
            if photonN == 0:
                self.sim_time = self.sim_time*9/3/3
            elif photonN == 1:
                self.sim_time = self.sim_time*47/3/3
            elif photonN == 2:
                self.sim_time = self.sim_time*115/3/3
            elif photonN == 3:
                self.sim_time = self.sim_time*115*6/3/3
            elif photonN == 4:
                self.sim_time = self.sim_time*60*6*4/3/3
            elif photonN == 5:
                self.sim_time = self.sim_time*80*6*4*7/3/3
            if self.dexa == True:
                self.sim_time = self.sim_time*2

        elif self.current_st == 1:
            angle = self.step_size.value()
            if sp_type == "Real":
                self.sim_time = self.sim_time*4
            if photonN == 0:
                self.sim_time = self.sim_time*9*3/3
            elif photonN == 1:
                self.sim_time = self.sim_time*22*3/3/3
            elif photonN == 2:
                self.sim_time = self.sim_time*23*3/3/3
            elif photonN == 3:
                self.sim_time = self.sim_time*30*6*3/3/3
            elif photonN == 4:
                self.sim_time = self.sim_time*60*6*4*2/3/3
            elif photonN == 5:
                self.sim_time = self.sim_time*80*6*4*2*3/3/3
            if angle == 0:
                self.sim_time = self.sim_time*1/3
            elif angle == 1:
                self.sim_time = self.sim_time*2/3
            elif angle == 2:
                self.sim_time = self.sim_time*3/3
            elif angle == 3:
                self.sim_time = self.sim_time*6/3
            elif angle == 4:
                self.sim_time = self.sim_time*10/3/3
            elif angle == 5:
                self.sim_time = self.sim_time*15/3/3

        elif self.current_st == 2:
            self.read_energy_values()
            self.sim_time = (math.ceil(abs(self.max_energy_value-self.min_energy_value)/self.step_size_value))*0.1
            print(self.max_energy_value)
            print(self.min_energy_value)
            print(self.step_size_value)

        elif self.current_st == 3:
            self.sim_time = self.sim_time*53

        if self.current_st == 0 and self.dexa == False:

            self.results_tab.setStyleSheet("""
                QWidget {
                    background-image: url('SE0.png');
                    background-repeat: no-repeat;
                    background-position: center;
                    background-clip: padding;
                }
            """)
        elif self.current_st == 0 and self.dexa == True:

            self.results_tab.setStyleSheet("""
                QWidget {
                    background-image: url('DE0.png');
                    background-repeat: no-repeat;
                    background-position: center;
                    background-clip: padding;
                }
            """) 
        elif self.current_st == 1:

            self.results_tab.setStyleSheet("""
                QWidget {
                    background-image: url('CT.png');
                    background-repeat: no-repeat;
                    background-position: center;
                    background-clip: padding;
                }
            """)  
        elif self.current_st == 2:

            self.results_tab.setStyleSheet("""
                QWidget {
                    background-image: url('Att.png');
                    background-repeat: no-repeat;
                    background-position: center;
                    background-clip: padding;
                }
            """)  
        elif self.current_st == 3:

            self.results_tab.setStyleSheet("""
                QWidget {
                    background-image: url('spec.png');
                    background-repeat: no-repeat;
                    background-position: center;
                    background-clip: padding;
                }
            """)         

        self.show_simulation_time()
    def show_simulation_time(self):
        # Suponiendo que self.sim_time contiene el tiempo de simulación en minutos
        sim_time = math.ceil(self.sim_time)  # Esto puede ser el tiempo en minutos que defines en tu clase

        # Crear un cuadro de mensaje con la información
        QMessageBox.information(
            self,
            "Simulation Time",
            f"Your simulation will take approximately {sim_time} minutes.",
            QMessageBox.Ok
        )
        # self.progress_bar.setValue(0)
        self.update_progress_sim()
    def update_progress_sim(self):
        # Revisar si la bandera está activada
        if self.force_complete_progress:
            self.progress_value = 100
            self.progress_bar.setValue(self.progress_value)
            self.statusBar().showMessage("Simulation sFinished")  # Mensaje cuando termine la simulación
            self.progress_timer.stop()  # Detener el temporizador
        else:
            # Cambiar el mensaje de la barra de estado mientras la simulación está en curso
            self.statusBar().showMessage("Simulating...")
            
            # Iniciar un temporizador que se ejecutará cada 100 milisegundos
            self.progress_timer = QTimer(self)
            self.progress_timer.timeout.connect(self.advance_progress)
            self.progress_timer.start(30)  # Ejecutar cada 100 milisegundos

    def advance_progress(self):
        if self.progress_value < 100:
            self.progress_value += 1
            self.progress_bar.setValue(self.progress_value)
        else:
            self.progress_timer.stop()  # Detener el temporizador cuando llegue a 100
            self.statusBar().showMessage("Simulation Finished")  # Mensaje al finalizar la simulación
            # self.change_image_in_results()

    # # Función para cambiar la imagen dinámicamente
    # def change_image_in_results(self):
    #     # Crear un QPixmap con la nueva imagen
    #     pixmap = QPixmap('Medx_logo.jpg')

    #     # Escalar la imagen al tamaño fijo que queremos
    #     scaled_pixmap = pixmap.scaled(self.fixed_image_size, Qt.KeepAspectRatio, Qt.SmoothTransformation)

    #     # Solo cambiar la imagen si es diferente a la actual
    #     if self.image_label.pixmap() != scaled_pixmap:
    #         self.image_label.setPixmap(scaled_pixmap)
    # Función para cambiar la imagen dinámicamente
    # Función para cambiar la imagen dinámicamente
    # def change_image_in_results(self):
    #     # Limpiar cualquier contenido existente en `results_tab`
    #     if not hasattr(self, 'results_tab') or self.results_tab is None:
    #         self.results_tab = QWidget()
    #         self.tab_widget.addTab(self.results_tab, "Results")

    #     # Crear un nuevo contenedor para el `results_tab`
    #     container = QWidget(self.results_tab)
    #     layout = QVBoxLayout(container)

    #     # Crear un QLabel para la imagen
    #     image_label = QLabel(container)
    #     image_label.setAlignment(Qt.AlignCenter)  # Centrar la imagen
    #     fixed_image_size = QSize(600 * 2, 400 * 2)  # Tamaño fijo

    #     # Crear un QPixmap con la nueva imagen
    #     pixmap = QPixmap('Medx_logo.jpg')

    #     # Escalar la imagen al tamaño fijo deseado
    #     scaled_pixmap = pixmap.scaled(fixed_image_size, Qt.KeepAspectRatio, Qt.SmoothTransformation)

    #     # Establecer la imagen en el QLabel
    #     image_label.setPixmap(scaled_pixmap)

    #     # Agregar el QLabel al layout del contenedor
    #     layout.addWidget(image_label)

    #     # Configurar el nuevo contenedor como el layout principal de `results_tab`
    #     if not self.results_tab.layout():
    #         self.results_tab.setLayout(QVBoxLayout())
    #     # Limpiar el layout previo y agregar el nuevo contenedor
    #     for i in reversed(range(self.results_tab.layout().count())):
    #         widget = self.results_tab.layout().itemAt(i).widget()
    #         if widget:
    #             widget.deleteLater()
    #     self.results_tab.layout().addWidget(container)
# Función para cambiar la imagen dinámicamente
    # def change_image_in_results(self):
    #     # Crear un nuevo contenedor para la `results_tab`
    #     container = QWidget()
    #     layout = QVBoxLayout(container)

    #     # Crear un QLabel para la imagen
    #     image_label = QLabel(container)
    #     image_label.setAlignment(Qt.AlignCenter)  # Centrar la imagen
    #     fixed_image_size = QSize(600 * 2, 400 * 2)  # Tamaño fijo

    #     # Crear un QPixmap con la nueva imagen
    #     pixmap = QPixmap('Medx_logo.jpg')

    #     # Escalar la imagen al tamaño fijo deseado
    #     scaled_pixmap = pixmap.scaled(fixed_image_size, Qt.KeepAspectRatio, Qt.SmoothTransformation)

    #     # Establecer la imagen en el QLabel
    #     image_label.setPixmap(scaled_pixmap)

    #     # Agregar el QLabel al layout del contenedor
    #     layout.addWidget(image_label)

    #     # Configurar el nuevo contenedor como el layout principal de `results_tab`
    #     self.results_tab = QWidget()
    #     self.results_tab.setLayout(QVBoxLayout())
    #     self.results_tab.layout().addWidget(container)

    #     # Añadir la pestaña `results_tab` al QTabWidget
    #     index = self.tab_widget.indexOf(self.results_tab)
    #     if index == -1:
    #         self.tab_widget.addTab(self.results_tab, "Results")
    #     else:
    #         self.tab_widget.setTabText(index, "Results")
    # Función para cambiar la imagen dinámicamente y reiniciar la pestaña results_tab
    # def change_image_in_results(self):
    #     # Si la pestaña ya existe, eliminarla
    #     index = self.tab_widget.indexOf(self.results_tab)
    #     if index != -1:
    #         self.tab_widget.removeTab(index)

    #     # Crear nuevamente la pestaña `results_tab`
    #     self.results_tab = QWidget()
    #     layout = QVBoxLayout(self.results_tab)

    #     # Crear un QLabel para la imagen
    #     image_label = QLabel(self.results_tab)
    #     image_label.setAlignment(Qt.AlignCenter)  # Centrar la imagen
    #     fixed_image_size = QSize(600 * 2, 400 * 2)  # Tamaño fijo

    #     # Crear un QPixmap con la nueva imagen
    #     pixmap = QPixmap('Medx_logo.jpg')

    #     # Escalar la imagen al tamaño fijo deseado
    #     scaled_pixmap = pixmap.scaled(fixed_image_size, Qt.KeepAspectRatio, Qt.SmoothTransformation)

    #     # Establecer la imagen en el QLabel
    #     image_label.setPixmap(scaled_pixmap)

    #     # Agregar el QLabel al layout
    #     layout.addWidget(image_label)

    #     # Añadir la pestaña nuevamente al `QTabWidget`
    #     self.tab_widget.addTab(self.results_tab, "Results")
    # def remove_results_tab(self):
    #     """Elimina la pestaña results_tab si ya existe."""
    #     index = self.tab_widget.indexOf(self.results_tab)
    #     if index != -1:
    #         self.tab_widget.removeTab(index)
    #     self.create_results_tab()
    #     self.add_image_to_results('Medx_logo.jpg')
    # def create_results_tab(self):
    #     """Crea una nueva pestaña results_tab con un contenedor vacío."""
    #     # Crear una nueva pestaña
    #     self.results_tab = QWidget()

    #     # Crear un layout para la pestaña
    #     layout = QVBoxLayout(self.results_tab)

    #     # Crear un contenedor vacío
    #     container = QWidget(self.results_tab)
    #     layout.addWidget(container)

    #     # Añadir la pestaña al QTabWidget
    #     self.tab_widget.addTab(self.results_tab, "Results")
    # def add_image_to_results(self, image_path):
    #     """Agrega una imagen al contenedor de results_tab."""
    #     # Crear un QLabel para la imagen
    #     image_label = QLabel(self.results_tab)
    #     image_label.setAlignment(Qt.AlignCenter)  # Centrar la imagen

    #     # Crear un QPixmap con la imagen
    #     pixmap = QPixmap(image_path)

    #     # Escalar la imagen a un tamaño fijo
    #     fixed_image_size = QSize(600 * 2, 400 * 2)
    #     scaled_pixmap = pixmap.scaled(fixed_image_size, Qt.KeepAspectRatio, Qt.SmoothTransformation)

    #     # Establecer la imagen en el QLabel
    #     image_label.setPixmap(scaled_pixmap)

    #     # Añadir el QLabel al layout de la pestaña
    #     self.results_tab.layout().addWidget(image_label)

    #     # self.stop_visualization()




# Ejecutar aplicación
app = QApplication(sys.argv)
window = SimulationApp()
window.show()
sys.exit(app.exec_())
