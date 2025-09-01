# Visual Computing – Praktikum 

Dieses Projekt wurde im Rahmen des Moduls Visual Computing entwickelt und demonstriert die grundlegende Verwendung von OpenGL zur Darstellung und Animation eines 3D-Roboters mit Beleuchtung, Kamera-Transformation und Interaktivität.

# Features

Shader-Programmierung (GLSL): Ein Vertex- und Fragment-Shader wird geladen und mit Uniforms für Transformationen, Beleuchtung und Materialien konfiguriert.

Geometrie mit Cube-Daten: Alle Körperteile des Roboters werden durch skalierte und transformierte Würfel dargestellt.

# Transformations-Matrizen:

View-Matrix: Positioniert und orientiert die Kamera in der Szene.

Projection-Matrix: Sorgt für eine perspektivische Projektion der 3D-Szene auf den Bildschirm.

Model-Matrix: Wird individuell für jedes Körperteil des Roboters berechnet.

# Licht & Material:

Punktlichtquelle mit Intensität und Ambient-Light

Diffuse, spekulare und Emissions-Materialeigenschaften

# Animation:

Rotation des gesamten Roboters um die Y-Achse

Armschwingen und Beinschritt-Animation mithilfe von sin() und cos()
