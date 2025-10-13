# DigitalHourglassLED

A **tilt-activated digital hourglass** using **dual 8x8 LED matrices**, a **tilt sensor**, and a **buzzer**. Sand flows visually across the matrices, with realistic particle motion and sound feedback.

---

## **Table of Contents**
- [Features](#features)
- [Circuit Diagram](#circuit-diagram)
- [Block Diagram](#block-diagram)
- [Flowchart](#flowchart)
- [Bill of Materials (BOM)](#bill-of-materials-bom)
- [Software Setup](#software-setup)
- [Usage](#usage)
- [License](#license)

---

## **Features**
- Dual 8x8 LED matrix simulation of falling sand
- Tilt sensor detects orientation and changes sand flow
- Buzzer feedback for flow start and completion
- Adjustable speed and sound effects
- Non-blocking frame update for smooth animation
- Particle-based sand simulation

---

## **Circuit Diagram**
![Circuit Diagram](hardware/circuit_diagram.png)

- **Arduino Nano**
- **2x 8x8 LED Matrix** (MAX7219)
- **Tilt Sensor**
- **Buzzer**
- **Wires & Breadboard / PCB**

---

## **Block Diagram**
![Block Diagram](hardware/block_diagram.png)

- Arduino Nano → Controls matrices and reads tilt sensor
- Tilt Sensor → Provides gravity orientation
- LED Matrices → Display sand particles
- Buzzer → Plays sounds on events

---

## **Flowchart**
![Flowchart](hardware/flowchart.png)

1. Start
2. Initialize matrices
3. Read tilt sensor
4. Update particle positions
5. Drop new particle if possible
6. Play sounds on flow start/completion
7. Repeat

---

## **Bill of Materials (BOM)**
| Component             | Quantity | Notes |
|-----------------------|----------|-------|
| Arduino Nano          | 1        |       |
| 8x8 LED Matrix (MAX7219) | 2    |       |
| Tilt Sensor           | 1        | Normally open / pull-up resistor |
| Buzzer                | 1        | Active/passive OK |
| Jumper Wires          | -        |       |
| Breadboard or PCB     | 1        | Optional for testing |
| 220Ω Resistors        | 2        | For buzzer optional |

---

## **Software Setup**
1. Install Arduino IDE  
2. Install **LedControl library**  
3. Connect the components according to the circuit diagram  
4. Open `software/Arduino/HourglassLED.ino`  
5. Upload to Arduino Nano  

---

## **Usage**
- Tilt the hourglass to start the sand flow  
- Watch the sand particles move realistically across matrices  
- Buzzer sounds when flow starts or when a matrix is full  
- Reset by reorienting the device or pressing reset on Arduino  

---

## **License**
MIT License – see [LICENSE](LICENSE) for details.

---

### Optional Additions: 
- Include **PCB layout** if you make one  
- Optional **enhanced features** in the future: brightness control, Wi-Fi logging, RGB matrices

---

