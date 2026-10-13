import hashlib
import json
import re
from pathlib import Path
from typing import List
from urllib.parse import unquote

import requests
from compress_gif import compress_gif, is_gif

URL_PATTERN = r"!\[[^\[\]]*\]\(\s*((?:https://i\.imgur\.com/|https://raw\.githubusercontent\.com)[^)]+?)\s*\)"
SCRIPT_DIR = Path(__file__).parent
CODE_FOLDER_PATH = SCRIPT_DIR.parent / "mods"
IMAGES_FOLDER_PATH = SCRIPT_DIR.parent / "images"

# Smaller GIFs are only compressed losslessly.
LOSSY_GIF_MIN_SIZE = 20 * 1024 * 1024

# Maps the paths of compressed images, relative to the images folder, to the
# SHA-256 of their originals.
ORIGINAL_HASHES_FILE_NAME = "original_hashes.json"

EXCLUDED_IMAGE_URLS = {
    # No longer available.
    "https://raw.githubusercontent.com/u3l6/force-chinese-ime/refs/heads/main/Before.png",
    "https://raw.githubusercontent.com/u3l6/force-chinese-ime/refs/heads/main/After.png",
}

# Temporary workaround: fetch a different URL than the one referenced in the
# readme, keeping the original path for the saved image.
FETCH_URL_WORKAROUNDS = {
    # https://github.com/ramensoftware/windhawk-mods/pull/4512#discussion_r3531474802
    # "https://raw.githubusercontent.com/QCQ171-C/mods-collection/refs/heads/main/Gallery/folder-thumbnail-tweaker/11-7.PNG": "https://raw.githubusercontent.com/QCQ171-C/mods-collection/refs/heads/main/Gallery/folder-thumbnail-tweaker/11-7.png",
    # "https://raw.githubusercontent.com/QCQ171-C/mods-collection/refs/heads/main/Gallery/folder-thumbnail-tweaker/10-11.png": "https://raw.githubusercontent.com/QCQ171-C/mods-collection/refs/heads/main/Gallery/folder-thumbnail-tweaker/10-11.PNG",
}

session = requests.Session()
session.headers.update({"User-Agent": "Mozilla/5.0"})


def find_image_urls(file_path: Path) -> List[str]:
    with file_path.open("r", encoding="utf-8") as file:
        content = file.read()

    readme = ""

    readme_pattern = r"^//[ \t]+==WindhawkModReadme==[ \t]*$\s*/\*\s*([\s\S]+?)\s*\*/\s*^//[ \t]+==/WindhawkModReadme==[ \t]*$"
    if match := re.search(readme_pattern, content, re.MULTILINE):
        readme = match.group(1)

    return re.findall(URL_PATTERN, readme)


def image_url_to_path(url: str):
    if not url.startswith("https://"):
        raise ValueError(f"Unsupported URL: {url}")

    path = url[len("https://") :]
    path = unquote(path)

    if (
        path.startswith("/")
        or path.startswith("\\")
        or re.search(r"(^|/|\\)\.\.", path)
    ):
        raise RuntimeError(f"Unsafe URL path: {path}")

    return path


def download_image(url: str, save_path: Path):
    url = FETCH_URL_WORKAROUNDS.get(url, url)
    response = session.get(url, stream=True)
    response.raise_for_status()

    save_path.parent.mkdir(parents=True, exist_ok=True)
    with save_path.open("wb") as file:
        for chunk in response.iter_content(chunk_size=8192):
            if chunk:
                file.write(chunk)

    print(f"Downloaded and saved: {save_path.name}")


def process_code_files(code_folder: Path, images_folder: Path):
    original_hashes_path = images_folder / ORIGINAL_HASHES_FILE_NAME
    previous_original_hashes: dict[str, str] = {}
    if original_hashes_path.exists():
        previous_original_hashes = json.loads(
            original_hashes_path.read_text(encoding="utf-8")
        )
    original_hashes: dict[str, str] = {}

    stale_images = [
        p for p in images_folder.rglob("*") if p.is_file() and p != original_hashes_path
    ]

    image_urls: set[str] = set()
    for file_path in code_folder.rglob("*.wh.cpp"):
        image_urls.update(find_image_urls(file_path))

    image_urls -= EXCLUDED_IMAGE_URLS

    for url in image_urls:
        relative_path = image_url_to_path(url)
        image_path = images_folder / relative_path
        if not image_path.resolve().is_relative_to(images_folder.resolve()):
            raise RuntimeError(
                f"Image path {image_path} is outside of images folder {images_folder}"
            )

        if not image_path.exists():
            download_image(url, image_path)
            if is_gif(image_path):
                with image_path.open("rb") as file:
                    original_hash = hashlib.file_digest(file, "sha256").hexdigest()
                if compress_gif(
                    image_path,
                    allow_lossy=image_path.stat().st_size >= LOSSY_GIF_MIN_SIZE,
                ):
                    original_hashes[relative_path] = original_hash
        elif relative_path in previous_original_hashes:
            original_hashes[relative_path] = previous_original_hashes[relative_path]

        if image_path in stale_images:
            stale_images.remove(image_path)

    if stale_images:
        print("Deleting stale images:")
        for image_path in stale_images:
            print(f"- {image_path.name}")
            image_path.unlink()

    original_hashes_path.write_text(
        json.dumps(original_hashes, indent=2, sort_keys=True) + "\n",
        encoding="utf-8",
    )


if __name__ == "__main__":
    process_code_files(CODE_FOLDER_PATH, IMAGES_FOLDER_PATH)
