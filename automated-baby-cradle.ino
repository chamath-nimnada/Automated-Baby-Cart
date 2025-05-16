#define IR_LEFT A1
#define IR_RIGHT A0

#define TRIG A5
#define ECHO A4

#define ENA 3
#define ENB 5
#define IN1 9
#define IN2 10
#define IN3 11
#define IN4 12

#define S0 6
#define S1 4
#define S2 13
#define S3 8
#define COLOR_OUT 7

#define NORMAL_SPEED 120
#define SLOW_SPEED 80

#define YELLOW_FREQ 200
#define RED_MIN_FREQ 111
#define RED_MAX_FREQ 125
#define BLUE_MIN_FREQ 62
#define BLUE_MAX_FREQ 65
#define GREEN_MIN_FREQ 41
#define GREEN_MAX_FREQ 45

const int obstacleDistance = 10;
bool carRunning = false;
String destination = "";

void setup() {
    pinMode(ENA, OUTPUT);
    pinMode(ENB, OUTPUT);
    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
    pinMode(IN3, OUTPUT);
    pinMode(IN4, OUTPUT);

    pinMode(TRIG, OUTPUT);
    pinMode(ECHO, INPUT);

    pinMode(S0, OUTPUT);
    pinMode(S1, OUTPUT);
    pinMode(S2, OUTPUT);
    pinMode(S3, OUTPUT);
    pinMode(COLOR_OUT, INPUT);
    
    digitalWrite(S0, HIGH);
    digitalWrite(S1, LOW);

    analogWrite(ENA, NORMAL_SPEED);
    analogWrite(ENB, NORMAL_SPEED);
    
    Serial.begin(9600);
}

void loop() {
    int left = analogRead(IR_LEFT);
    int right = analogRead(IR_RIGHT);
    int distance = getDistance();
    String color = detectColor();

    if (Serial.available()) {
        String command = Serial.readStringUntil('\n');
        command.trim();

        if (command == "r1" || command == "r2" || command == "r3") {
            carRunning = true;
            destination = command;
            Serial.println("Car started!");
        } else if (command == "s") {
            carRunning = false;
            stopCar();
            Serial.println("Car stopped!");
        }
    }

    if (distance > 0 && distance < obstacleDistance) {
        stopCar();
        Serial.println("Obstacle detected! Car stopped.");
        return;
    }
    
    if (carRunning) {
        if (color == "RED") {
            stopCar();
            Serial.println("Destination reached!");
            carRunning = false;
        } else if (color == "YELLOW") {
            analogWrite(ENA, SLOW_SPEED);
            analogWrite(ENB, SLOW_SPEED);
        } else {
            analogWrite(ENA, NORMAL_SPEED);
            analogWrite(ENB, NORMAL_SPEED);
        }
        
        navigate(left, right);
    }
}

void navigate(int left, int right) {
    static bool passedJunction1 = false;
    static bool passedJunction2 = false;

    if (destination == "r1" && !passedJunction1) {
        turnLeft();
        passedJunction1 = true;
    } else if (destination == "r2") {
        if (!passedJunction1) {
            passedJunction1 = true;
        } else if (!passedJunction2) {
            turnRight();
            passedJunction2 = true;
        }
    }
    
    if (left > 900 && right > 900) {
        stopCar();
    } else if (left < 50 && right < 50) {
        moveForward();
    } else if (left > 900 && right < 50) {
        turnRight();
    } else if (left < 50 && right > 900) {
        turnLeft();
    }
}

int getDistance() {
    digitalWrite(TRIG, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG, LOW);

    long duration = pulseIn(ECHO, HIGH);
    return duration * 0.034 / 2;
}

String detectColor() {
    digitalWrite(S2, LOW);
    digitalWrite(S3, LOW);
    int red = pulseIn(COLOR_OUT, LOW);
    
    digitalWrite(S2, HIGH);
    digitalWrite(S3, HIGH);
    int green = pulseIn(COLOR_OUT, LOW);
    
    digitalWrite(S2, LOW);
    digitalWrite(S3, HIGH);
    int blue = pulseIn(COLOR_OUT, LOW);
    
    if (red >= RED_MIN_FREQ && red <= RED_MAX_FREQ) return "RED";
    if (green >= GREEN_MIN_FREQ && green <= GREEN_MAX_FREQ) return "YELLOW";
    return "OTHER";
}

void moveForward() {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
}

void turnRight() {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
}

void turnLeft() {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
}

void stopCar() {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
}
