#!/usr/bin/env python3
"""
LDM Logo Variants Generator
Creates different sizes and formats of the LDM logo for various use cases.

Author: Anna-el Gerard RABENANDRASANA (aerab243)
Project: LDM - Linux Download Manager
"""

import os
import sys
from pathlib import Path

try:
    from PIL import Image, ImageDraw, ImageFont
    import cairosvg
except ImportError:
    print("Required packages not found. Please install:")
    print("pip install Pillow cairosvg")
    sys.exit(1)

def create_logo_variants():
    """Create various sizes and formats of the LDM logo."""

    # Get the directory containing this script
    script_dir = Path(__file__).parent
    icons_dir = script_dir / "icons"

    # Ensure icons directory exists
    icons_dir.mkdir(exist_ok=True)

    # Source SVG file
    svg_file = icons_dir / "logo-ldm-opensource.svg"

    if not svg_file.exists():
        print(f"Error: SVG file not found at {svg_file}")
        return False

    print(f"Creating logo variants from {svg_file}")

    # Define sizes for different use cases
    sizes = {
        'icon_16': 16,      # Small icons
        'icon_24': 24,      # Toolbar icons
        'icon_32': 32,      # Standard icons
        'icon_48': 48,      # Medium icons
        'icon_64': 64,      # Large icons
        'icon_128': 128,    # High-res icons
        'icon_256': 256,    # Very high-res icons
        'splash_400': 400,  # Splash screen
        'app_512': 512,     # Application icon
    }

    success_count = 0

    for name, size in sizes.items():
        try:
            # Convert SVG to PNG at specified size
            png_data = cairosvg.svg2png(
                url=str(svg_file),
                output_width=size,
                output_height=size
            )

            # Save PNG file
            png_file = icons_dir / f"logo-ldm-{size}.png"
            with open(png_file, 'wb') as f:
                f.write(png_data)

            print(f"✓ Created {png_file.name} ({size}x{size})")
            success_count += 1

        except Exception as e:
            print(f"✗ Failed to create {name}: {e}")

    # Create the main PNG version (64x64 as default)
    try:
        png_data = cairosvg.svg2png(
            url=str(svg_file),
            output_width=64,
            output_height=64
        )

        main_png = icons_dir / "logo-ldm-opensource.png"
        with open(main_png, 'wb') as f:
            f.write(png_data)

        print(f"✓ Created main PNG: {main_png.name}")
        success_count += 1

    except Exception as e:
        print(f"✗ Failed to create main PNG: {e}")

    # Create ICO file for Windows compatibility
    try:
        # Create multi-size ICO
        ico_sizes = [16, 24, 32, 48, 64, 128, 256]
        images = []

        for size in ico_sizes:
            png_data = cairosvg.svg2png(
                url=str(svg_file),
                output_width=size,
                output_height=size
            )

            # Convert to PIL Image
            from io import BytesIO
            img = Image.open(BytesIO(png_data))
            images.append(img)

        # Save as ICO
        ico_file = icons_dir / "logo-ldm-opensource.ico"
        images[0].save(
            ico_file,
            format='ICO',
            sizes=[(img.width, img.height) for img in images]
        )

        print(f"✓ Created ICO file: {ico_file.name}")
        success_count += 1

    except Exception as e:
        print(f"✗ Failed to create ICO file: {e}")

    # Create ICNS file for macOS compatibility
    try:
        # macOS icon sizes
        icns_sizes = [16, 32, 64, 128, 256, 512, 1024]
        temp_dir = icons_dir / "temp_iconset"
        temp_dir.mkdir(exist_ok=True)

        for size in icns_sizes:
            png_data = cairosvg.svg2png(
                url=str(svg_file),
                output_width=size,
                output_height=size
            )

            # Save temporary PNG files
            if size == 1024:
                png_file = temp_dir / f"icon_{size}x{size}.png"
            else:
                png_file = temp_dir / f"icon_{size}x{size}.png"
                # Also create @2x versions for some sizes
                if size <= 512:
                    png_file_2x = temp_dir / f"icon_{size//2}x{size//2}@2x.png"
                    with open(png_file_2x, 'wb') as f:
                        f.write(png_data)

            with open(png_file, 'wb') as f:
                f.write(png_data)

        print(f"✓ Created macOS iconset in {temp_dir}")
        print(f"  Use: iconutil -c icns {temp_dir} to create .icns file")
        success_count += 1

    except Exception as e:
        print(f"✗ Failed to create ICNS preparation: {e}")

    print(f"\nCompleted: {success_count} operations successful")
    return success_count > 0

def create_branded_variants():
    """Create additional branded variants of the logo."""

    script_dir = Path(__file__).parent
    icons_dir = script_dir / "icons"

    try:
        # Load the main logo PNG
        logo_file = icons_dir / "logo-ldm-opensource.png"
        if not logo_file.exists():
            print("Main PNG logo not found. Creating variants first...")
            return False

        logo = Image.open(logo_file)

        # Create a version with text
        logo_with_text = Image.new('RGBA', (200, 80), (0, 0, 0, 0))

        # Resize logo to fit
        logo_resized = logo.resize((64, 64), Image.Resampling.LANCZOS)
        logo_with_text.paste(logo_resized, (10, 8), logo_resized)

        # Add text (simplified - would need proper font handling)
        draw = ImageDraw.Draw(logo_with_text)
        try:
            # Try to use a better font if available
            font = ImageFont.truetype("arial.ttf", 16)
        except:
            font = ImageFont.load_default()

        draw.text((80, 20), "LDM", fill=(0, 212, 255), font=font)
        draw.text((80, 40), "Linux Download", fill=(255, 255, 255), font=ImageFont.load_default())
        draw.text((80, 55), "Manager", fill=(255, 255, 255), font=ImageFont.load_default())

        # Save branded version
        branded_file = icons_dir / "logo-ldm-branded.png"
        logo_with_text.save(branded_file)
        print(f"✓ Created branded logo: {branded_file.name}")

        return True

    except Exception as e:
        print(f"✗ Failed to create branded variants: {e}")
        return False

def main():
    """Main function to create all logo variants."""

    print("====================================")
    print("LDM Logo Variants Generator")
    print("====================================")
    print()

    # Create basic variants
    if create_logo_variants():
        print("\n✓ Basic logo variants created successfully")
    else:
        print("\n✗ Failed to create basic logo variants")
        return 1

    # Create branded variants
    if create_branded_variants():
        print("✓ Branded logo variants created successfully")
    else:
        print("⚠ Could not create branded variants (not critical)")

    print("\n====================================")
    print("Logo Generation Complete!")
    print("====================================")
    print("\nGenerated files:")

    script_dir = Path(__file__).parent
    icons_dir = script_dir / "icons"

    for file in icons_dir.glob("logo-ldm-*.png"):
        size = os.path.getsize(file)
        print(f"  {file.name} ({size} bytes)")

    for file in icons_dir.glob("logo-ldm-*.ico"):
        size = os.path.getsize(file)
        print(f"  {file.name} ({size} bytes)")

    print("\nUsage in Qt application:")
    print("  QIcon(\":/icons/logo-ldm-opensource.svg\")  // Vector")
    print("  QIcon(\":/icons/logo-ldm-opensource.png\")  // Raster")
    print("  QIcon(\":/icons/logo-ldm-64.png\")         // Specific size")

    return 0

if __name__ == "__main__":
    sys.exit(main())
