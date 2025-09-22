#!/usr/bin/env python3
"""
Script to create placeholder icons for LDM interface
This creates simple PNG icons that can be replaced with better designs later
"""

import os
from PIL import Image, ImageDraw, ImageFont
import colorsys

def create_icon(size, color, text="", filename="icon.png"):
    """Create a simple colored icon with optional text"""
    # Create image with transparent background
    img = Image.new('RGBA', (size, size), (0, 0, 0, 0))
    draw = ImageDraw.Draw(img)

    # Draw rounded rectangle background
    margin = 2
    draw.rounded_rectangle(
        [margin, margin, size-margin, size-margin],
        radius=4,
        fill=color,
        outline=(0, 0, 0, 100),
        width=1
    )

    # Add text if provided
    if text:
        try:
            # Try to use a better font
            font_size = max(8, size // 4)
            font = ImageFont.truetype("/usr/share/fonts/truetype/liberation/LiberationSans-Bold.ttf", font_size)
        except:
            # Fallback to default font
            font = ImageFont.load_default()

        # Get text bounding box
        bbox = draw.textbbox((0, 0), text, font=font)
        text_width = bbox[2] - bbox[0]
        text_height = bbox[3] - bbox[1]

        # Center text
        x = (size - text_width) // 2
        y = (size - text_height) // 2

        draw.text((x, y), text, fill=(255, 255, 255, 255), font=font)

    # Save icon
    img.save(filename, 'PNG')
    print(f"Created {filename}")

def create_file_type_icon(size, color, symbol, filename):
    """Create file type icons with symbols"""
    img = Image.new('RGBA', (size, size), (0, 0, 0, 0))
    draw = ImageDraw.Draw(img)

    # Draw file shape
    margin = 2
    corner = size // 4

    # File body
    points = [
        (margin, margin),
        (size - corner - margin, margin),
        (size - margin, corner + margin),
        (size - margin, size - margin),
        (margin, size - margin)
    ]
    draw.polygon(points, fill=color, outline=(0, 0, 0, 100))

    # File corner fold
    fold_points = [
        (size - corner - margin, margin),
        (size - margin, corner + margin),
        (size - corner - margin, corner + margin)
    ]
    draw.polygon(fold_points, fill=(200, 200, 200, 150))

    # Add symbol
    try:
        font_size = max(10, size // 3)
        font = ImageFont.truetype("/usr/share/fonts/truetype/liberation/LiberationSans-Bold.ttf", font_size)
    except:
        font = ImageFont.load_default()

    bbox = draw.textbbox((0, 0), symbol, font=font)
    text_width = bbox[2] - bbox[0]
    text_height = bbox[3] - bbox[1]

    x = (size - text_width) // 2
    y = (size - text_height) // 2 + size // 8

    draw.text((x, y), symbol, fill=(255, 255, 255, 255), font=font)

    img.save(filename, 'PNG')
    print(f"Created {filename}")

def create_category_icon(size, color, symbol, filename):
    """Create category icons with folder-like appearance"""
    img = Image.new('RGBA', (size, size), (0, 0, 0, 0))
    draw = ImageDraw.Draw(img)

    # Draw folder shape
    margin = 2
    tab_width = size // 3
    tab_height = size // 6

    # Folder tab
    draw.rounded_rectangle(
        [margin, margin + tab_height, margin + tab_width, margin + tab_height * 2],
        radius=2,
        fill=color
    )

    # Folder body
    draw.rounded_rectangle(
        [margin, margin + tab_height, size - margin, size - margin],
        radius=3,
        fill=color,
        outline=(0, 0, 0, 100),
        width=1
    )

    # Add symbol
    if symbol:
        try:
            font_size = max(8, size // 4)
            font = ImageFont.truetype("/usr/share/fonts/truetype/liberation/LiberationSans-Bold.ttf", font_size)
        except:
            font = ImageFont.load_default()

        bbox = draw.textbbox((0, 0), symbol, font=font)
        text_width = bbox[2] - bbox[0]
        text_height = bbox[3] - bbox[1]

        x = (size - text_width) // 2
        y = (size - text_height) // 2 + tab_height

        draw.text((x, y), symbol, fill=(255, 255, 255, 255), font=font)

    img.save(filename, 'PNG')
    print(f"Created {filename}")

def main():
    # Ensure icons directory exists
    icons_dir = "icons"
    if not os.path.exists(icons_dir):
        os.makedirs(icons_dir)

    # Icon size
    size = 32

    # Color scheme - modern blue tones
    primary_blue = (52, 152, 219, 255)
    secondary_blue = (41, 128, 185, 255)
    green = (46, 204, 113, 255)
    red = (231, 76, 60, 255)
    orange = (230, 126, 34, 255)
    purple = (155, 89, 182, 255)
    gray = (149, 165, 166, 255)

    # Main toolbar icons
    toolbar_icons = [
        ("add_url.png", primary_blue, "+"),
        ("resume.png", green, "▶"),
        ("stop.png", red, "■"),
        ("stop_all.png", red, "■■"),
        ("delete.png", red, "×"),
        ("delete_completed.png", orange, "✓×"),
        ("options.png", gray, "⚙"),
        ("scheduler.png", purple, "⏰"),
        ("start_queue.png", green, "▶▶"),
        ("stop_queue.png", red, "■■"),
        ("grabber.png", secondary_blue, "📹"),
        ("tell_friend.png", orange, "👥"),
    ]

    # Create toolbar icons
    for filename, color, symbol in toolbar_icons:
        create_icon(size, color, symbol, os.path.join(icons_dir, filename))

    # Category icons
    category_icons = [
        ("category_all.png", primary_blue, "📁"),
        ("category_compressed.png", orange, "📦"),
        ("category_documents.png", gray, "📄"),
        ("category_music.png", purple, "🎵"),
        ("category_programs.png", red, "⚙"),
        ("category_video.png", secondary_blue, "🎬"),
        ("category_unfinished.png", orange, "⏳"),
        ("category_finished.png", green, "✓"),
        ("category_queues.png", gray, "📋"),
    ]

    # Create category icons
    for filename, color, symbol in category_icons:
        create_category_icon(size, color, symbol, os.path.join(icons_dir, filename))

    # File type icons
    file_type_icons = [
        ("file_audio.png", purple, "♪"),
        ("file_video.png", secondary_blue, "🎬"),
        ("file_archive.png", orange, "📦"),
        ("file_document.png", gray, "📄"),
        ("file_program.png", red, "⚙"),
        ("file_generic.png", gray, "📄"),
    ]

    # Create file type icons
    for filename, color, symbol in file_type_icons:
        create_file_type_icon(size, color, symbol, os.path.join(icons_dir, filename))

    # Additional UI icons
    ui_icons = [
        ("pause.png", orange, "⏸"),
        ("play.png", green, "▶"),
        ("cancel.png", red, "×"),
        ("settings.png", gray, "⚙"),
        ("folder.png", primary_blue, "📁"),
        ("download.png", green, "⬇"),
        ("app.png", primary_blue, "LDM"),
    ]

    # Create UI icons
    for filename, color, symbol in ui_icons:
        create_icon(size, color, symbol, os.path.join(icons_dir, filename))

    print("\nAll icons created successfully!")
    print("You can replace these placeholder icons with better designs later.")

if __name__ == "__main__":
    main()
