# -*- coding: utf-8 -*-
from os import sys
from pathlib import Path

try:
    from skbuild import setup
except ImportError:
    print("scikit-build is required to build from source.", file=sys.stderr)
    print("Please run:", file=sys.stderr)
    print("", file=sys.stderr)
    print("  python -m pip install scikit-build")
    sys.exit(1)

setup(
    name="itk-topologycontrol",
    version="1.1.0",
    author="Bryn Lloyd",
    author_email="lloyd@itis.swiss",
    packages=["itk"],
    package_dir={"itk": "itk"},
    download_url=r"https://github.com/dyollb/ITKTopologyControl",
    description=r"ITK external module to control topology of binary mask regions",
    long_description=(Path(__file__).parent / "README.md").read_text(),
    long_description_content_type="text/markdown",
    classifiers=[
        "License :: OSI Approved :: Apache Software License",
        "Programming Language :: Python",
        "Programming Language :: C++",
        "Development Status :: 4 - Beta",
        "Intended Audience :: Developers",
        "Intended Audience :: Education",
        "Intended Audience :: Healthcare Industry",
        "Intended Audience :: Science/Research",
        "Topic :: Scientific/Engineering",
        "Topic :: Scientific/Engineering :: Medical Science Apps.",
        "Topic :: Scientific/Engineering :: Information Analysis",
        "Topic :: Software Development :: Libraries",
        "Operating System :: Microsoft :: Windows",
        "Operating System :: POSIX",
        "Operating System :: Unix",
        "Operating System :: MacOS",
    ],
    license="Apache",
    keywords="ITK InsightToolkit",
    url=r"https://github.com/dyollb/ITKTopologyControl",
    install_requires=[r"itk-filtering>=5.3.0"],
)
