import qrcode

# Wi-Fi credentials
ssid = "ESP32-Access-Point"
password = "123456789"
auth_type = "WPA2"  

# Wi-Fi QR code format
wifi_format = f"WIFI:S:{ssid};T:{auth_type};P:{password};;"

# Generate QR code
qr = qrcode.QRCode(
    version=1,
    error_correction=qrcode.constants.ERROR_CORRECT_L,
    box_size=10,
    border=4,
)
qr.add_data(wifi_format)
qr.make(fit=True)

# Create an image
img = qr.make_image(fill='black', back_color='white')

# Save the image
img.save("wifi_qr.png")