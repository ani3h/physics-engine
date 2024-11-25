module PhysicsSimulation {
    requires javafx.controls;
    requires javafx.graphics;
    requires java.desktop;
    
    exports JAVA;
    opens JAVA to javafx.graphics, javafx.base, javafx.controls;
}