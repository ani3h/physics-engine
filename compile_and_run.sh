#!/bin/bash

# Get the absolute path to the project directory
PROJECT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

# Set the JavaFX path relative to the project
JAVAFX_PATH="$PROJECT_DIR/resources/javafx-sdk-23.0.1/lib"

echo "Using JavaFX from: $JAVAFX_PATH"

# Clean previous compilation
rm -rf "$PROJECT_DIR/bin"
mkdir -p "$PROJECT_DIR/bin"

# Compile
echo "Compiling..."
javac -d bin \
      --module-path "$JAVAFX_PATH" \
      --add-modules javafx.controls,javafx.graphics \
      -cp "$JAVAFX_PATH/*" \
      $(find src -name "*.java")

if [ $? -eq 0 ]; then
    echo "Compilation successful"
    
    # Run
    echo "Running application..."
    java --module-path "$JAVAFX_PATH:bin" \
         --add-modules javafx.controls,javafx.graphics \
         -cp "bin:$JAVAFX_PATH/*" \
         JAVA.SimulationApp
else
    echo "Compilation failed"
    exit 1
fi