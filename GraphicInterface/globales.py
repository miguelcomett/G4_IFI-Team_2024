# globales.py
class EstadoGlobal:
    _instancia = None

    def __new__(cls):
        if cls._instancia is None:
            cls._instancia = super(EstadoGlobal, cls).__new__(cls)
            cls._instancia.frame_widgets = None
            cls._instancia.etiqueta_imagen = None
            cls._instancia.canvas = None
        return cls._instancia