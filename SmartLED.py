import tkinter as tk
import requests
import tooltip
import threading

# ---------------------- Global Variables ----------------------
ESP32_IP = "http://10.0.0.186" # Change to your ESP32's IP address
BRIGHTNESS_STEP = 10            # Step for arrow key adjustments
slider_update_job = None

# ---------------------- Helper Functions ----------------------

def center_window(window, width=400, height=300):
    """Centers the window on the screen."""
    screen_width = window.winfo_screenwidth()
    screen_height = window.winfo_screenheight()
    x = (screen_width - width) // 2
    y = (screen_height - height) // 2
    window.geometry(f"{width}x{height}+{x}+{y}")

def threaded(*args, **kwargs):
    """Run update_light in a separate thread to avoid blocking the GUI."""
    threading.Thread(target=update_light, args=args, kwargs=kwargs, daemon=True).start()

def update_light(*args, **kwargs):
    """Send color and brightness values to the ESP32."""
    color = color_var.get()
    brightness = brightness_var.get()
    try:
        response = requests.get(f"{ESP32_IP}/set", params={"color": color, "brightness": brightness}, timeout=2)
        status_label.config(text=f"Sent {color} @ {brightness}% | ESP32: {response.text}")
    except requests.exceptions.RequestException as e:
        status_label.config(text=f"Error | Cannot Reach ESP32")
        tooltip.ToolTip(status_label, f"Details: {e}")

def slider_changed(val):
    global slider_update_job
    if slider_update_job is not None:
        window.after_cancel(slider_update_job)
    # Wait 100ms after last movement before sending
    slider_update_job = window.after(100, threaded)

def increase_brightness(event=None):
    """Increase brightness using Up arrow key."""
    current = brightness_var.get()
    if current < 100:
        brightness_var.set(min(current + BRIGHTNESS_STEP, 100))
        threaded()

def decrease_brightness(event=None):
    """Decrease brightness using Down arrow key."""
    current = brightness_var.get()
    if current > 0:
        brightness_var.set(max(current - BRIGHTNESS_STEP, 0))
        threaded()

# ---------------------- GUI Setup ----------------------
window = tk.Tk()
window.title("RGB Smart Light Controller")
center_window(window)
window.resizable(False, False)

# Title label
title_lbl = tk.Label(window, text="RGB Smart Light Controller", font=("Arial Bold", 16))
title_lbl.grid(column=0, row=0, columnspan=2, pady=10)

# Status label
status_label = tk.Label(window, text="")
status_label.grid(column=0, row=4, columnspan=2, pady=10)

# Color selection dropdown
color_var = tk.StringVar(value="White")
color_menu = tk.OptionMenu(window, color_var, "Red", "Green", "Blue", "Yellow", "Purple", "Cyan", "White")
color_menu.grid(column=1, row=1, padx=10, pady=10)

# Brightness slider
brightness_var = tk.IntVar(value=0)
brightness_slider = tk.Scale(window, from_=100, to=0, orient=tk.VERTICAL, variable=brightness_var, command=slider_changed)
brightness_slider.grid(column=1, row=2, padx=10, pady=10)


# Label for slider
slider_lbl = tk.Label(window, text="Set Brightness:")
slider_lbl.grid(column=0, row=2, padx=10, pady=10)

# Bindings
color_var.trace_add("write", threaded)  # update when color changes
window.bind("<Up>", increase_brightness)    # arrow keys
window.bind("<Down>", decrease_brightness)



# ---------------------- Run GUI ----------------------
window.mainloop()
