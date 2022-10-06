# ITKTopologyControl

[![Build Status](https://github.com/dyollb/ITKTopologyControl/workflows/Build,%20test,%20package/badge.svg)](https://github.com/dyollb/ITKTopologyControl/actions)
[![License](https://img.shields.io/badge/License-Apache_2.0-blue.svg)](https://github.com/dyollb/ITKTopologyControl/blob/main/LICENSE)
[![PyPI version](https://img.shields.io/pypi/v/itk-topologycontrol.svg)](https://badge.fury.io/py/itk-topologycontrol)

## Overview

This is a module for the Insight Toolkit ([ITK](https://github.com/InsightSoftwareConsortium/ITK)). The module includes a filter called `FixTopologyCarveOutside` which works like morphological closing, except that in the "erode" phase topological constraints are enforced to avoid re-opening holes. It is able to close holes in thin layers (e.g. skull) with a minimal thickness.

```python
    import itk
    skull_mask = itk.imread('path/to/skull_with_holes.mha').astype(itk.US)

    ImageType = type(skull_mask)
    MaskType = itk.Image[itk.UC, 3]

    top_control = itk.FixTopologyCarveOutside[ImageType, ImageType, MaskType].New()
    top_control.SetInput(skull_mask)
    top_control.SetRadius(5)
    top_control.Update()
    skull_mask_closed = top_control.GetOutput()

    itk.imwrite(skull_mask_closed, 'skull_mask_closed.mha')
```

Or using a custom mask (e.g. a sphere around a hole):

```python
    import itk
    skull_mask = itk.imread('path/to/skull_with_holes.mha').astype(itk.US)

    custom_mask = skull_mask.astype(itk.UC)
    mask_np = itk.array_view_from_image(custom_mask)
    mask_np[135:175, 100:130, 145:170] = 1

    skull_mask_closed = itk.fix_topology_carve_outside(skull_mask, MaskImage=custom_mask, Radius=5)
    itk.imwrite(skull_mask_closed, 'skull_mask_closed.mha')
```

![Closing holes in skull](https://raw.githubusercontent.com/dyollb/ITKTopologyControl/main/doc/close_holes_skull.gif)

## Installation

To install the binary Python packages:

```shell
  python -m pip install itk-topologycontrol
```
