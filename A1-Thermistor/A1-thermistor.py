import math
import pygame
import serial

arduino = serial.Serial('COM3', 9600)

pygame.init()
screen = pygame.display.set_mode((640, 480))
pygame.display.set_caption('Temperature Sensor Visualization')

# Initialize the font module
pygame.font.init()

# Choose a font and size
font = pygame.font.SysFont('arial', 24)  # You can change 'arial' to any available font

SERIES_RESISTOR = 10000.0
NOMINAL_RESISTANCE = 10000.0 # nominal resistance at nominal temperature
NOMINAL_TEMPERATURE = 298.15 # temperature at which the nominal resistance is specified
BETA = 3950.0 # Beta constant of the thermistor
TEMP_MIN = 5  # Minimum temperature expected
TEMP_MAX = 45  # Maximum temperature expected

def calculate_temperature(sensor_value):
    R_thermistor = SERIES_RESISTOR * (1023.0 / sensor_value - 1)  # Thermistor resistance
    temp_k = 1 / (1 / NOMINAL_TEMPERATURE + (1 / BETA) * math.log(R_thermistor / NOMINAL_RESISTANCE))  # Temperature in Kelvin
    temp_c = temp_k - 273.15  # Convert Kelvin to Celsius
    return temp_c

def temperature_to_color(temp_c):
    normalized_temp = (temp_c - TEMP_MIN) / (TEMP_MAX - TEMP_MIN) # Normalize temperature into the 0-1 range
    red = max(0, min(255, int(255 * normalized_temp)))  # More red as the temperature increases
    blue = max(0, min(255, int(255 * (1 - normalized_temp))))  # More blue as the temperature decreases
    return (red, 0, blue)

running = True
while running:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False

    sensor_value = arduino.readline().decode().strip()
    if sensor_value.isdigit():
        temp_c = calculate_temperature(int(sensor_value))
        color = temperature_to_color(temp_c)
        screen.fill((255, 255, 255))  # Clear the screen
        pygame.draw.circle(screen, color, (320, 240), 100)  # Draw the circle

        # Render the temperature text
        temp_text = f"{temp_c:.2f}°C"  # Format the temperature to two decimal places
        text_surface = font.render(temp_text, True, (0, 0, 0))  # Black font color
        screen.blit(text_surface, (450, 230))  # Position the text next to the circle

        pygame.display.flip()

pygame.quit()