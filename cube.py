import serial
import tkinter as tk
from tkinter import ttk
import colorsys
import math
import serial.tools.list_ports
import threading

ser = None
update = False
updateVal = 0x00

def connect_serial(port):
    """Set up the serial connection."""
    try:
        return serial.Serial(port, baudrate=9600, timeout=1)
    except serial.SerialException as e:
        print(f"Error opening serial port {port}: {e}")
        return None

def update_mode(new_mode):
    global ser, update, updateVal # Access the global variable
    update = True
    updateVal = new_mode  # Update mode
    send_byte(updateVal, ser)

def send_byte(hue, ser):
    """Send the hue value (as a byte) over the serial connection."""
    hue_byte = 0
    if(type(hue) == float): # if its an acutal hue value
        hue_byte = int(hue * 255)  # Normalize hue to byte range (0-255)
    
    if ser and ser.is_open:
        global update, updateVal
        if update:
            update = False
            payload = bytes([0xff, updateVal, hue_byte])
            
        else:
            hue_byte = hue_byte if hue_byte != 255 else 254
            payload = bytes([hue_byte])

        ser.write(payload)
        print(payload)

def hsv_to_rgb(hue):
    """Convert HSV hue value to RGB color."""
    r, g, b = colorsys.hsv_to_rgb(hue, 1, 1)  # Full saturation and brightness
    return (int(r * 255), int(g * 255), int(b * 255))  # Convert to 0-255 range

def on_color_wheel_click(event, wheel_radius, ser):
    """Handle mouse click or drag on the color wheel."""
    # Get mouse coordinates
    x = event.x - wheel_radius
    y = event.y - wheel_radius

    # Calculate the angle of the point on the wheel
    angle = math.atan2(y, x)
    angle_deg = math.degrees(angle)
    if angle_deg < 0:
        angle_deg += 360  # Convert negative angles to positive

    hue = angle_deg / 360  # Normalize to 0-1 range (hue)
    hue_display = int(hue * 255)  # Convert hue to a byte value (0-255)
    
    # Update the hue value and send the byte
    hue_label.config(text=f"Hue: {hue_display}")
    send_byte(hue, ser)

def draw_color_wheel(canvas, wheel_radius):
    """Draw a color wheel on the canvas."""
    for angle in range(0, 360):
        hue = angle / 360  # Normalize to 0-1 range (hue)
        color = hsv_to_rgb(hue)
        canvas.create_line(wheel_radius, wheel_radius,
                           wheel_radius + math.cos(math.radians(angle)) * wheel_radius,
                           wheel_radius + math.sin(math.radians(angle)) * wheel_radius,
                           fill='#%02x%02x%02x' % color, width=2)

def get_com_ports():
    """Returns a list of available COM ports on Windows."""
    ports = serial.tools.list_ports.comports()  # List of available serial ports
    return [port.device for port in ports]  # Return just the port names (e.g., 'COM1', 'COM2')

def on_select_port(event, wheel_radius, canvas):
    """Callback when a COM port is selected from the dropdown."""
    global ser  # Declare 'ser' as global to modify the global variable
    selected_port = com_port_combobox.get()

    # Only close the previous connection if ser is not None
    if ser:
        ser.close()  # Close previous connection if any
    
    try:
        ser = connect_serial(selected_port)
        if ser:
            print(f"Connected to {selected_port}")
            # Start reading serial data in a separate thread
            threading.Thread(target=read_serial_data, args=(ser,), daemon=True).start()
        else:
            print(f"Failed to connect to {selected_port}")
    except Exception as e:
        print(f"Error connecting to {selected_port}: {e}")

def read_serial_data(ser):
    """Reads data from the serial port and updates the serial monitor."""
    while True:
        if ser and ser.is_open:
            # Read available data from serial
            data = ser.readline().decode('utf-8').strip()  # Read a line from the serial port
            if data:
                # Update the serial monitor with incoming data in the main thread
                window.after(0, update_serial_monitor, data)

def update_serial_monitor(data):
    """Updates the serial monitor with new data."""
    serial_monitor.insert(tk.END, data + "\n")
    serial_monitor.yview(tk.END)  # Auto-scroll to the latest data

def update_sat(scale):
    sat = scale.get()  # Get the hue value from the scale widget
    global update, updateVal
    update = True
    updateVal = 0x01
    send_byte(sat, ser)

def update_vib(scale):
    vib = scale.get()  # Get the hue value from the scale widget
    global update, updateVal
    update = True
    updateVal = 0x02
    send_byte(vib, ser)

def update_brightness(scale):
    brightness = scale.get()  # Get the hue value from the scale widget
    global update, updateVal
    update = True
    updateVal = 0x03
    send_byte(brightness, ser)

def setup_gui(wheel_radius):
    """Set up the GUI window with the color wheel and other widgets."""
    global hue_label, canvas, com_port_combobox, serial_monitor, window

    window = tk.Tk()
    window.title("GAMER CUBE")

    # Fetch the available COM ports
    com_ports = get_com_ports()

    # Create a label for COM Port selection
    com_label = tk.Label(window, text="Select COM Port:")
    com_label.pack(pady=10)

    # Create a dropdown (combobox) with available COM ports
    com_port_combobox = ttk.Combobox(window, values=com_ports)
    com_port_combobox.pack(pady=10)

    # Set the first COM port as the default selection (optional)
    if com_ports:
        com_port_combobox.current(0)

    # Bind the event when the user selects a COM port
    com_port_combobox.bind("<<ComboboxSelected>>", lambda event: on_select_port(event, wheel_radius, canvas))

    # Create the color wheel canvas
    canvas = tk.Canvas(window, width=2 * wheel_radius, height=2 * wheel_radius, bg='white')
    canvas.pack(padx=10, pady=4)

    # Draw the color wheel
    draw_color_wheel(canvas, wheel_radius)

    # Label to display the current hue value
    hue_label = tk.Label(window, text="Hue: 0", font=("Arial", 14))
    hue_label.pack(pady=2)



    sat_scale = ttk.Scale(window, from_=0, to=1, orient="horizontal", command=lambda e: update_sat(sat_scale))
    sat_scale.pack(padx=20, pady=0)
    sat_label = tk.Label(window, text="Adjust Saturation")
    sat_label.pack()
    
    vib_scale = ttk.Scale(window, from_=0, to=.075, orient="horizontal", command=lambda e: update_vib(vib_scale))
    vib_scale.pack(padx=20, pady=0)
    vib_label = tk.Label(window, text="Adjust Vibrance")
    vib_label.pack()
    
    brightness_scale = ttk.Scale(window, from_=0, to=.06, orient="horizontal", command=lambda e: update_brightness(brightness_scale))
    brightness_scale.pack(padx=20, pady=0)
    brightness_label = tk.Label(window, text="Adjust Brightness")
    brightness_label.pack()



    # Creating the radio buttons with their corresponding variables and values
    radio1 = tk.Radiobutton(window, text="Update all", variable=updateVal, value=1, command=lambda: update_mode(0x10))
    radio1.pack()

    radio2 = tk.Radiobutton(window, text="Update U", variable=updateVal, value=2, command=lambda: update_mode(0x11))
    radio2.pack()

    radio3 = tk.Radiobutton(window, text="Update D", variable=updateVal, value=3, command=lambda: update_mode(0x12))
    radio3.pack()

    radio4 = tk.Radiobutton(window, text="Update N", variable=updateVal, value=4, command=lambda: update_mode(0x13))
    radio4.pack()

    radio5 = tk.Radiobutton(window, text="Update S", variable=updateVal, value=5, command=lambda: update_mode(0x14))
    radio5.pack()

    radio6 = tk.Radiobutton(window, text="Update E", variable=updateVal, value=6, command=lambda: update_mode(0x15))
    radio6.pack()

    radio7 = tk.Radiobutton(window, text="Update W", variable=updateVal, value=7, command=lambda: update_mode(0x16))
    radio7.pack()


    # Create a Text widget for the serial monitor (to display incoming data)
    serial_monitor = tk.Text(window, height=10, width=50)
    serial_monitor.pack(padx=10, pady=1)
    
    # Bind mouse events for color selection
    canvas.bind("<Button-1>", lambda event: on_color_wheel_click(event, wheel_radius,  ser))
    canvas.bind("<B1-Motion>", lambda event: on_color_wheel_click(event, wheel_radius, ser))  # Dragging
    
    return window

if __name__ == "__main__":
    wheel_radius = 150  # Radius of the color wheel
    
    # Initialize the GUI
    gui_window = setup_gui(wheel_radius)

    # Start the GUI main loop
    gui_window.mainloop()