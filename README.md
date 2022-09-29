# ITKTopologyControl

[![Build Status](https://github.com/dyollb/ITKTopologyControl/workflows/Build,%20test,%20package/badge.svg)](https://github.com/dyollb/ITKTopologyControl/actions)
[![License](https://img.shields.io/github/license/dyollb/ITKTopologyControl?color=blue)](https://github.com/dyollb/ITKTopologyControl/blob/main/LICENSE)
[![PyPI version](https://img.shields.io/pypi/v/itk-topologycontrol.svg)](https://badge.fury.io/py/itk-topologycontrol)

## Overview

This is a module for the Insight Toolkit (ITK). The module includes a filter called `FixTopologyCarveOutside` which works like morphological closing, except that in the "erode" phase topological constraints are enforced to avoid re-opening holes. It is able to close holes in thin layers (e.g. skull) with a minimal thickness.

```python
    import itk
    skull_mask = itk.imread('path/to/skull_with_holes.mha').astype(itk.US)

    ImageType = type(skull_mask)

    top_control = itk.FixTopologyCarveOutside[ImageType, MaskType].New()
    top_control.SetInput(skull_mask)
    top_control.Update()
    skull_mask_closed = top_control.GetOutput()

    itk.imwrite(skull_mask_closed, 'skull_mask_closed.mha')
```

Or using the pythonic API:

```python
    import itk
    skull_mask = itk.imread('path/to/skull_with_holes.mha').astype(itk.US)
    skull_mask_closed = itk.fix_topology_carve_outside(skull_mask)
    itk.imwrite(skull_mask_closed, 'skull_mask_closed.mha')
```

## Installation

To install the binary Python packages:

```shell
  python -m pip install itk-topologycontrol
```
