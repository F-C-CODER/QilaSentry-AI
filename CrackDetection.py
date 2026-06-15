import cv2
import numpy as np
import tkinter as tk
from tkinter import filedialog, messagebox


def upload_image():
    status_label.config(
        text="Processing Image...",
        fg="orange"
    )

    file_path = filedialog.askopenfilename(
        title="Select Crack Image",
        filetypes=[("Image Files", "*.jpg *.jpeg *.png")]
    )

    if not file_path:
        status_label.config(
            text="No Image Selected",
            fg="red"
        )
        return

    image = cv2.imread(file_path)

    if image is None:
        messagebox.showerror(
            "Error",
            "Could not open image!"
        )
        return

    image = cv2.resize(image, (900, 550))

    gray = cv2.cvtColor(
        image,
        cv2.COLOR_BGR2GRAY
    )

    blur = cv2.GaussianBlur(
        gray,
        (5, 5),
        0
    )

    edges = cv2.Canny(
        blur,
        50,
        150
    )

    kernel = np.ones((3, 3), np.uint8)

    dilated = cv2.dilate(
        edges,
        kernel,
        iterations=1
    )

    contours, _ = cv2.findContours(
        dilated,
        cv2.RETR_EXTERNAL,
        cv2.CHAIN_APPROX_SIMPLE
    )

    crack_found = False

    for contour in contours:

        area = cv2.contourArea(contour)

        if area > 100:

            x, y, w, h = cv2.boundingRect(contour)

            aspect_ratio = w / float(h)

            if aspect_ratio > 2 or aspect_ratio < 0.5:

                crack_found = True

                cv2.rectangle(
                    image,
                    (x, y),
                    (x + w, y + h),
                    (0, 255, 0),
                    3
                )

                cv2.putText(
                    image,
                    "Crack Detected",
                    (x, y - 10),
                    cv2.FONT_HERSHEY_SIMPLEX,
                    0.7,
                    (0, 0, 255),
                    2
                )

    if crack_found:
        status_label.config(
            text="Crack Detected ✅",
            fg="#22C55E"
        )
    else:
        status_label.config(
            text="No Crack Found ❌",
            fg="#EF4444"
        )

    cv2.imshow(
        "Detected Crack Result",
        image
    )

    cv2.imshow(
        "Processed Edge View",
        dilated
    )


root = tk.Tk()
root.title("Crack Detection System")
root.geometry("1350x900")
root.configure(bg="#0F172A")
root.resizable(False, False)


main_frame = tk.Frame(
    root,
    bg="#0F172A"
)

main_frame.pack(expand=True)


desc = tk.Label(
    main_frame,
    text="Upload an image to detect wall or surface cracks",
    font=("Arial", 14),
    fg="#CBD5E1",
    bg="#0F172A"
)

desc.pack(pady=(40, 20))


upload_btn = tk.Button(
    main_frame,
    text="📂 Browse Image",
    font=("Arial", 16, "bold"),
    bg="#2563EB",
    fg="white",
    padx=25,
    pady=12,
    relief="flat",
    cursor="hand2",
    command=upload_image
)

upload_btn.pack(pady=20)


status_label = tk.Label(
    main_frame,
    text="Waiting for image...",
    font=("Arial", 14),
    fg="yellow",
    bg="#0F172A"
)

status_label.pack(pady=15)


exit_btn = tk.Button(
    main_frame,
    text="Exit",
    font=("Arial", 13, "bold"),
    bg="#DC2626",
    fg="white",
    padx=22,
    pady=8,
    relief="flat",
    cursor="hand2",
    command=root.quit
)

exit_btn.pack(pady=25)


footer = tk.Label(
    root,
    text="Powered by OpenCV + Python",
    font=("Arial", 10),
    fg="#94A3B8",
    bg="#0F172A"
)

footer.pack(side="bottom", pady=10)

root.mainloop()
