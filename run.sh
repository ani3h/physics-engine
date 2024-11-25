#!/bin/bash

# Get the directory where the script is located
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

# Set the path to JavaFX SDK relative to the project
export PATH_TO_FX="$SCRIPT_DIR/resources/javafx-sdk-23.0.1/lib"

# Echo the path to verify
echo "JavaFX SDK path: $PATH_TO_FX"

# Compile the Java files
javac --module-path "$PATH_TO_FX" --add-modules javafx.controls,javafx.fxml src/JAVA/*.java src/JAVA/jni/*.java

# Run the application
java --module-path "$PATH_TO_FX" --add-modules javafx.controls,javafx.fxml -cp src JAVA.SimulationApp