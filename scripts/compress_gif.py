# Compresses GIFs in place with the smallest of several encodings, keeping the
# original if none is smaller. Requires gifsicle and gif2webp (libwebp) on PATH.
#
# The lossy encodings, used only when allowed, have no noticeable quality loss
# on their own, but the loss compounds, so a GIF shouldn't be compressed lossily
# more than once.

import argparse
import shutil
import subprocess
import tempfile
from collections.abc import Callable
from concurrent.futures import ThreadPoolExecutor
from pathlib import Path

Candidates = dict[str, Callable[[Path, Path], list[str]]]

LOSSLESS_CANDIDATES: Candidates = {
    "gifsicle -O3": lambda src, dst: ["gifsicle", "-O3", str(src), "-o", str(dst)],
}

LOSSY_CANDIDATES: Candidates = {
    "gifsicle -O3 --lossy=40": lambda src, dst: [
        "gifsicle",
        "-O3",
        "--lossy=40",
        str(src),
        "-o",
        str(dst),
    ],
    # Keeps the .gif name, browsers detect the image format from the content.
    "gif2webp -lossy -q 80": lambda src, dst: [
        "gif2webp",
        "-mt",
        "-lossy",
        "-q",
        "80",
        "-m",
        "4",
        str(src),
        "-o",
        str(dst),
    ],
}


def is_gif(path: Path) -> bool:
    with path.open("rb") as file:
        return file.read(6) in (b"GIF87a", b"GIF89a")


def run(command: list[str]) -> subprocess.CompletedProcess[str]:
    return subprocess.run(
        command, check=False, capture_output=True, text=True, errors="replace"
    )


def compress_gif(path: Path, allow_lossy: bool = False) -> bool:
    """Return whether the file was replaced with a compressed version."""
    if not is_gif(path):
        print(f"{path}: not a GIF, skipped")
        return False

    original_size = path.stat().st_size

    candidates = LOSSLESS_CANDIDATES
    if allow_lossy:
        candidates = candidates | LOSSY_CANDIDATES

    with tempfile.TemporaryDirectory() as temp_dir:
        # The tools fail on paths longer than MAX_PATH, so they work on a copy
        # in the temporary directory.
        input_path = Path(temp_dir) / "input.gif"
        shutil.copyfile(path, input_path)

        output_paths = {
            name: Path(temp_dir) / str(i) for i, name in enumerate(candidates)
        }
        commands = [
            candidates[name](input_path, output_paths[name]) for name in candidates
        ]

        with ThreadPoolExecutor() as executor:
            results = dict(zip(candidates, executor.map(run, commands)))

        sizes: dict[str, int] = {}
        for name, result in results.items():
            if result.returncode == 0:
                sizes[name] = output_paths[name].stat().st_size
            else:
                print(f"{path}: {name} failed: {result.stderr.strip()}")

        best = min(sizes, key=sizes.__getitem__, default=None)
        if best is None or sizes[best] >= original_size:
            print(f"{path}: {original_size:,} bytes, kept original")
            return False

        shutil.copyfile(output_paths[best], path)
        print(
            f"{path}: {original_size:,} -> {sizes[best]:,} bytes"
            f" ({100 * sizes[best] / original_size:.1f}%, {best})"
        )
        return True


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("gif_files", type=Path, nargs="+")
    parser.add_argument("--allow-lossy", action="store_true")
    args = parser.parse_args()

    for gif_file in args.gif_files:
        compress_gif(gif_file, args.allow_lossy)


if __name__ == "__main__":
    main()
