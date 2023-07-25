/*=========================================================================
 *
 *  Copyright NumFOCUS
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         https://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/

#ifndef itkFixTopologyCarveOutside_h
#define itkFixTopologyCarveOutside_h

#include "itkFixTopologyBase.h"

namespace itk
{
/** \class FixTopologyCarveOutside
 *
 * \brief This filter does morphological closing with topology constraints
 *
 * It works by doing following steps:
 * 1. dilate the foreground
 * 2. erode/carve the dilated voxels from the "outside" while preserving the topology of the dilated region.
 *
 * The first step closes holes and the second returns as close as possible to the input mask, while ensuring that the
 * holes are not re-opened.
 *
 * This filter implements ideas from: Vanderhyde, James. "Topology control of volumetric data.", PhD dissertation,
 * Georgia Institute of Technology, 2007..
 *
 * \author Bryn Lloyd
 * \ingroup TopologyControl
 */
template <class TInputImage, class TOutputImage, class TMaskImage = itk::Image<unsigned char, 3>>
class ITK_TEMPLATE_EXPORT FixTopologyCarveOutside : public FixTopologyBase<TInputImage, TOutputImage, TMaskImage>
{
public:
  /** Extract dimension from input and output image. */
  itkStaticConstMacro(InputImageDimension, unsigned int, TInputImage::ImageDimension);
  itkStaticConstMacro(OutputImageDimension, unsigned int, TOutputImage::ImageDimension);

  static const unsigned int ImageDimension = InputImageDimension;

  /** Standard class typedefs. */
  using Self = FixTopologyCarveOutside;
  using Superclass = FixTopologyBase<TInputImage, TOutputImage, TMaskImage>;
  using Pointer = SmartPointer<Self>;
  using ConstPointer = SmartPointer<const Self>;

  /** Method for creation through the object factory */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(FixTopologyCarveOutside, FixTopologyBase);

  /** Type for input image. */
  using InputImageType = TInputImage;

  /** Type for output image: Skeleton of the object.  */
  using OutputImageType = TOutputImage;

  /** Type for mask image  */
  using MaskImageType = TMaskImage;

  /** Pointer Type for input image. */
  using InputImagePointer = typename InputImageType::ConstPointer;

  /** Pointer Type for the output image. */
  using OutputImagePointer = typename OutputImageType::Pointer;

protected:
  FixTopologyCarveOutside() = default;
  ~FixTopologyCarveOutside() override = default;

  MaskImageType *
  CreateDefaultMask(ProgressAccumulator * progress) override;

  void
  ComputeThinImage(ProgressAccumulator * progress) override;

  using ePixelState = typename Superclass::ePixelState;

private:
  ITK_DISALLOW_COPY_AND_ASSIGN(FixTopologyCarveOutside);

}; // end of FixTopologyCarveOutside class

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#  include "itkFixTopologyCarveOutside.hxx"
#endif

#endif // itkFixTopologyCarveOutside
