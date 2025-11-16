import re
from pathlib import Path
from typing import List
from urllib.parse import unquote

import requests

URL_PATTERN = r"!\[[^\]]*\]\(\s*((?:https://i\.imgur\.com/|https://raw\.githubusercontent\.com)[^)]+?)\s*\)"
SCRIPT_DIR = Path(__file__).parent
CODE_FOLDER_PATH = SCRIPT_DIR.parent / "mods"
IMAGES_FOLDER_PATH = SCRIPT_DIR.parent / "images"

EXCLUDED_IMAGE_URLS = {
    # No longer available.
    "https://raw.githubusercontent.com/u3l6/force-chinese-ime/refs/heads/main/Before.png",
    "https://raw.githubusercontent.com/u3l6/force-chinese-ime/refs/heads/main/After.png",
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

    if re.search(r"(^|/|\\)\.\.", path):
        raise RuntimeError(f"Unsafe URL path: {path}")

    return path


def download_image(url: str, save_path: Path):
    response = session.get(url, stream=True)
    response.raise_for_status()

    save_path.parent.mkdir(parents=True, exist_ok=True)
    with save_path.open("wb") as file:
        for chunk in response.iter_content(chunk_size=8192):
            if chunk:
                file.write(chunk)

    print(f"Downloaded and saved: {save_path.name}")


def process_code_files(code_folder: Path, images_folder: Path):
    stale_images = [p for p in images_folder.rglob("*") if p.is_file()]

    image_urls: set[str] = set()
    for file_path in code_folder.rglob("*.wh.cpp"):
        image_urls.update(find_image_urls(file_path))

    image_urls -= EXCLUDED_IMAGE_URLS

    for url in image_urls:
        image_path = images_folder / image_url_to_path(url)
        if not image_path.exists():
            download_image(url, image_path)

        if image_path in stale_images:
            stale_images.remove(image_path)

    if stale_images:
        print("Deleting stale images:")
        for image_path in stale_images:
            print(f"- {image_path.name}")
            image_path.unlink()


if __name__ == "__main__":
    process_code_files(CODE_FOLDER_PATH, IMAGES_FOLDER_PATH)
