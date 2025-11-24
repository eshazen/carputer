import tkinter as tk
import sys

def read_polygons(filename):
    polygons = []
    with open(filename, "r") as f:
        lines = [line.strip() for line in f if line.strip()]

    i = 0
    while i < len(lines):
        n = int(lines[i])
        i += 1
        pts = []
        for _ in range(n):
            x, y = map(float, lines[i].split())
            pts.append((x, y))
            i += 1
        polygons.append(pts)
    return polygons


def draw_polygons(polygons):
    # Compute bounds so we can scale and center the drawing
    all_x = [p[0] for poly in polygons for p in poly]
    all_y = [p[1] for poly in polygons for p in poly]

    min_x, max_x = min(all_x), max(all_x)
    min_y, max_y = min(all_y), max(all_y)

    width, height = 800, 600
    margin = 20

    scale_x = (width - 2 * margin) / (max_x - min_x) if max_x > min_x else 1
    scale_y = (height - 2 * margin) / (max_y - min_y) if max_y > min_y else 1
    scale = min(scale_x, scale_y)

    # Tkinter window
    root = tk.Tk()
    root.title("Polygon Viewer")

    canvas = tk.Canvas(root, width=width, height=height, bg="white")
    canvas.pack()

    # Draw each polygon
    for poly in polygons:
        coords = []
        for (x, y) in poly:
            # Transform coordinates to window space
            sx = margin + (x - min_x) * scale
            sy = height - (margin + (y - min_y) * scale)  # invert Y
            coords.extend([sx, sy])

        canvas.create_polygon(coords, outline="black", fill="", width=2)

    root.mainloop()


if __name__ == "__main__":
    filename = "polygons.txt"   # Change to your file name
    if len(sys.argv) > 1:
        filename = sys.argv[1]
    polys = read_polygons(filename)
    draw_polygons(polys)
