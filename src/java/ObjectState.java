package JAVA;

// Class to hold physics object state information
public class ObjectState {
    private final int id;
    private final double posX, posY;
    private final double velX, velY;
    private final double accX, accY;

    public ObjectState(int id, double posX, double posY, double velX, double velY,
                      double accX, double accY) {
        this.id = id;
        this.posX = posX;
        this.posY = posY;
        this.velX = velX;
        this.velY = velY;
        this.accX = accX;
        this.accY = accY;
    }

    // Getters
    public int getId() { return id; }
    public double getPosX() { return posX; }
    public double getPosY() { return posY; }
    public double getVelX() { return velX; }
    public double getVelY() { return velY; }
    public double getAccX() { return accX; }
    public double getAccY() { return accY; }
}
