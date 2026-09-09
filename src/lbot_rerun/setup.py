from pathlib import Path

from setuptools import find_packages, setup


PACKAGE_NAME = "lbot_rerun"
PACKAGE_ROOT = Path(__file__).resolve().parent
REPOSITORY_ROOT = PACKAGE_ROOT.parents[1]


def asset_data_files():
    """Install only the URDF meshes that the left-task visualizer consumes."""
    source_root = REPOSITORY_ROOT / "assets"
    required = {
        source_root / "workstations/lkls73_i1_o6_bimanual/workstation.urdf",
        source_root / "components/bases/lkls73_torso/variants/default/meshes/base_link.STL",
    }
    required.update(
        (source_root / "components/arms/lkls73_arm/variants/left/meshes").glob("*.STL")
    )
    required.update(
        (source_root / "components/hands/linkerhand_o6/variants/left/meshes").glob("*.STL")
    )
    result = []
    for path in sorted(required):
        relative = path.relative_to(source_root)
        destination = Path("share") / PACKAGE_NAME / "assets" / relative.parent
        result.append((str(destination), [str(Path("..") / ".." / "assets" / relative)]))
    return result


data_files = [
    ("share/ament_index/resource_index/packages", [f"resource/{PACKAGE_NAME}"]),
    (f"share/{PACKAGE_NAME}", ["package.xml", "requirements.txt"]),
    (f"share/{PACKAGE_NAME}/launch", ["launch/lbot_rerun.launch.py"]),
    (f"share/{PACKAGE_NAME}/config", ["../../config/viewer/rerun.yaml"]),
]
data_files.extend(asset_data_files())

setup(
    name=PACKAGE_NAME,
    version="0.1.0",
    packages=find_packages(exclude=["test"]),
    data_files=data_files,
    install_requires=[],
    zip_safe=True,
    entry_points={
        "console_scripts": [
            "rerun_visualizer = lbot_rerun.rerun_visualizer_node:main",
        ],
    },
)
